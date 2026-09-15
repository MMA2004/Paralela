#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <pthread.h>

struct WorkerArg {
    int tid;
    int num_threads;
    int N;
    double threshold;
    double smooth;
    double denom;
    int start_row;
    int end_row;
    double* grid_A;
    double* grid_B;
    pthread_barrier_t* barrier;
    bool* global_converged;
    int* local_converged_flags;
    int* iterations_count;
};

void* worker_thread(void* arg) {
    WorkerArg* w = static_cast<WorkerArg*>(arg);
    int N = w->N;
    double denom = w->denom;
    double threshold = w->threshold;
    double smooth = w->smooth;

    int iter = 0;

    while (true) {
        // Determinar qué malla lee y cuál escribe según la iteración
        double* current = (iter % 2 == 0) ? w->grid_A : w->grid_B;
        double* next_grid = (iter % 2 == 0) ? w->grid_B : w->grid_A;

        bool my_converged = true;

        // Cómputo del subdominio de filas
        for (int i = w->start_row; i <= w->end_row; ++i) {
            for (int j = 1; j < N - 1; ++j) {
                double top    = current[(i - 1) * N + j];
                double bottom = current[(i + 1) * N + j];
                double left   = current[i * N + (j - 1)];
                double right  = current[i * N + (j + 1)];
                double center = current[i * N + j];

                next_grid[i * N + j] = (top + bottom + left + right + smooth * center) / denom;

                double neighbor_avg = (top + bottom + left + right) / 4.0;
                if (std::abs(center - neighbor_avg) > threshold) {
                    my_converged = false;
                }
            }
        }

        w->local_converged_flags[w->tid] = my_converged ? 1 : 0;

        // Barrera 1: todos los hilos terminan el cómputo de la iteración
        pthread_barrier_wait(w->barrier);

        // Hilo 0 consolida convergencia
        if (w->tid == 0) {
            bool all_converged = true;
            for (int t = 0; t < w->num_threads; ++t) {
                if (w->local_converged_flags[t] == 0) {
                    all_converged = false;
                    break;
                }
            }
            *(w->global_converged) = all_converged;
            *(w->iterations_count) = iter + 1;
        }

        // Barrera 2: todos leen el veredicto de convergencia antes de la siguiente vuelta
        pthread_barrier_wait(w->barrier);

        if (*(w->global_converged)) {
            break;
        }

        iter++;
    }

    pthread_exit(nullptr);
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        std::cerr << "Uso: " << argv[0] << " <threads> <N> <threshold> <smooth> [hot_threshold]" << std::endl;
        std::cerr << "Ejemplo: " << argv[0] << " 4 256 0.1 4.0 50.0" << std::endl;
        return 1;
    }

    int num_threads = std::atoi(argv[1]);
    int N = std::atoi(argv[2]);
    double threshold = std::atof(argv[3]);
    double smooth = std::atof(argv[4]);
    double hot_threshold = (argc >= 6) ? std::atof(argv[5]) : 50.0;

    int inner_rows = N - 2;
    if (num_threads > inner_rows) {
        num_threads = inner_rows;
    }

    std::vector<double> grid_A(N * N, 0.0);
    std::vector<double> grid_B(N * N, 0.0);

    // Inicializar bordes a 100.0 en ambas mallas
    for (int j = 0; j < N; ++j) {
        grid_A[0 * N + j] = 100.0;
        grid_A[(N - 1) * N + j] = 100.0;
        grid_B[0 * N + j] = 100.0;
        grid_B[(N - 1) * N + j] = 100.0;
    }
    for (int i = 0; i < N; ++i) {
        grid_A[i * N + 0] = 100.0;
        grid_A[i * N + (N - 1)] = 100.0;
        grid_B[i * N + 0] = 100.0;
        grid_B[i * N + (N - 1)] = 100.0;
    }

    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, nullptr, num_threads);

    bool global_converged = false;
    int total_iterations = 0;
    std::vector<int> local_converged_flags(num_threads, 0);

    std::vector<pthread_t> threads(num_threads);
    std::vector<WorkerArg> args(num_threads);

    int rows_per_t = inner_rows / num_threads;
    int rem = inner_rows % num_threads;
    int current_start = 1;

    for (int t = 0; t < num_threads; ++t) {
        int count = rows_per_t + (t < rem ? 1 : 0);
        args[t].tid = t;
        args[t].num_threads = num_threads;
        args[t].N = N;
        args[t].threshold = threshold;
        args[t].smooth = smooth;
        args[t].denom = 4.0 + smooth;
        args[t].start_row = current_start;
        args[t].end_row = current_start + count - 1;
        args[t].grid_A = grid_A.data();
        args[t].grid_B = grid_B.data();
        args[t].barrier = &barrier;
        args[t].global_converged = &global_converged;
        args[t].local_converged_flags = local_converged_flags.data();
        args[t].iterations_count = &total_iterations;

        current_start += count;
        pthread_create(&threads[t], nullptr, worker_thread, &args[t]);
    }

    for (int t = 0; t < num_threads; ++t) {
        pthread_join(threads[t], nullptr);
    }
    pthread_barrier_destroy(&barrier);

    // Determinar cuál grid contiene el resultado final
    double* final_grid = (total_iterations % 2 == 0) ? grid_A.data() : grid_B.data();

    int hot_cells = 0;
    for (int i = 0; i < N * N; ++i) {
        if (final_grid[i] > hot_threshold) {
            hot_cells++;
        }
    }

    std::cout << "--- Pthreads Hot Plate ---" << std::endl;
    std::cout << "Hilos: " << num_threads << " | Dimensión: " << N << "x" << N << std::endl;
    std::cout << "Iteraciones: " << total_iterations << std::endl;
    std::cout << "Celdas calientes (> " << hot_threshold << "): " << hot_cells << std::endl;

    return 0;
}