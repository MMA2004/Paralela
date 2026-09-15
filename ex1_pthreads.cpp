#include <iostream>
#include <vector>
#include <cstdlib>
#include <pthread.h>

struct ThreadData {
    int thread_id;
    int num_threads;
    int rows;
    int cols;
    const double* A;
    const double* v;
    double* result;
};

void* multiply_row_block(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);

    // Reparto equilibrado de filas entre hilos
    int rows_per_thread = data->rows / data->num_threads;
    int remainder = data->rows % data->num_threads;

    int start_row = data->thread_id * rows_per_thread + std::min(data->thread_id, remainder);
    int end_row = start_row + rows_per_thread + (data->thread_id < remainder ? 1 : 0);

    // Multiplicar el bloque asignado
    for (int i = start_row; i < end_row; ++i) {
        double sum = 0.0;
        for (int j = 0; j < data->cols; ++j) {
            sum += data->A[i * data->cols + j] * data->v[j];
        }
        data->result[i] = sum;
    }

    pthread_exit(nullptr);
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Uso: " << argv[0] << " <threads> <filas> <columnas> [matriz...] [vector...]" << std::endl;
        return 1;
    }

    int num_threads = std::atoi(argv[1]);
    int rows = std::atoi(argv[2]);
    int cols = std::atoi(argv[3]);

    int expected_args = 4 + (rows * cols) + cols;
    if (argc != expected_args) {
        std::cerr << "Error: argumentos insuficientes o sobrantes." << std::endl;
        return 1;
    }

    std::vector<double> A(rows * cols);
    int idx = 4;
    for (int i = 0; i < rows * cols; ++i) {
        A[i] = std::atof(argv[idx++]);
    }

    std::vector<double> v(cols);
    for (int j = 0; j < cols; ++j) {
        v[j] = std::atof(argv[idx++]);
    }

    std::vector<double> result(rows, 0.0);

    // Ajustar número de hilos si supera el número de filas
    if (num_threads > rows) {
        num_threads = rows;
    }

    std::vector<pthread_t> threads(num_threads);
    std::vector<ThreadData> tdata(num_threads);

    for (int t = 0; t < num_threads; ++t) {
        tdata[t] = {t, num_threads, rows, cols, A.data(), v.data(), result.data()};
        pthread_create(&threads[t], nullptr, multiply_row_block, &tdata[t]);
    }

    for (int t = 0; t < num_threads; ++t) {
        pthread_join(threads[t], nullptr);
    }

    std::cout << "Resultado (A * v) con " << num_threads << " threads:" << std::endl;
    for (int i = 0; i < rows; ++i) {
        std::cout << "[ " << result[i] << " ]" << std::endl;
    }

    return 0;
}