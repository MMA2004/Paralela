#include <iostream>
#include <vector>
#include <cstdlib>
#include <omp.h>

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

    omp_set_num_threads(num_threads);

    // Descomposición por bloques de filas
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < rows; ++i) {
        double sum = 0.0;
        for (int j = 0; j < cols; ++j) {
            sum += A[i * cols + j] * v[j];
        }
        result[i] = sum;
    }

    std::cout << "Resultado (A * v) con OpenMP (" << num_threads << " threads):" << std::endl;
    for (int i = 0; i < rows; ++i) {
        std::cout << "[ " << result[i] << " ]" << std::endl;
    }

    return 0;
}