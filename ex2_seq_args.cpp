#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Uso: " << argv[0] << " <N> <threshold> <smooth_weight> [hot_threshold]" << std::endl;
        std::cerr << "Ejemplo: " << argv[0] << " 256 0.1 4.0 50.0" << std::endl;
        return 1;
    }

    int N = std::atoi(argv[1]);
    double threshold = std::atof(argv[2]);
    double smooth = std::atof(argv[3]);
    double hot_threshold = (argc >= 5) ? std::atof(argv[4]) : 50.0;

    if (N < 3) {
        std::cerr << "Error: N debe ser mayor o igual a 3." << std::endl;
        return 1;
    }

    std::vector<double> current(N * N, 0.0);
    std::vector<double> next_grid(N * N, 0.0);

    // Bordes fijos a 100.0, interior a 0.0
    for (int j = 0; j < N; ++j) {
        current[0 * N + j] = 100.0;
        current[(N - 1) * N + j] = 100.0;
        next_grid[0 * N + j] = 100.0;
        next_grid[(N - 1) * N + j] = 100.0;
    }
    for (int i = 0; i < N; ++i) {
        current[i * N + 0] = 100.0;
        current[i * N + (N - 1)] = 100.0;
        next_grid[i * N + 0] = 100.0;
        next_grid[i * N + (N - 1)] = 100.0;
    }

    double denom = 4.0 + smooth;
    int iterations = 0;
    bool converged = false;

    while (!converged) {
        iterations++;
        converged = true;

        for (int i = 1; i < N - 1; ++i) {
            for (int j = 1; j < N - 1; ++j) {
                double top    = current[(i - 1) * N + j];
                double bottom = current[(i + 1) * N + j];
                double left   = current[i * N + (j - 1)];
                double right  = current[i * N + (j + 1)];
                double center = current[i * N + j];

                next_grid[i * N + j] = (top + bottom + left + right + smooth * center) / denom;

                double neighbor_avg = (top + bottom + left + right) / 4.0;
                if (std::abs(center - neighbor_avg) > threshold) {
                    converged = false;
                }
            }
        }

        current.swap(next_grid);
    }

    int hot_cells = 0;
    for (int i = 0; i < N * N; ++i) {
        if (current[i] > hot_threshold) {
            hot_cells++;
        }
    }

    std::cout << "--- Resultados ---" << std::endl;
    std::cout << "Dimensión: " << N << "x" << N << std::endl;
    std::cout << "Threshold: " << threshold << " | Smooth: " << smooth << std::endl;
    std::cout << "Iteraciones: " << iterations << std::endl;
    std::cout << "Celdas calientes (> " << hot_threshold << "): " << hot_cells << std::endl;

    return 0;
}