#include <iostream>
#include <vector>
#include <iomanip>

void print_matrix(const std::string& name, const std::vector<double>& M, int r, int c) {
    std::cout << "\n--- " << name << " (" << r << "x" << c << ") ---" << std::endl;
    for (int i = 0; i < r; ++i) {
        std::cout << "[ ";
        for (int j = 0; j < c; ++j) {
            std::cout << std::setw(6) << M[i * c + j] << " ";
        }
        std::cout << "]" << std::endl;
    }
}

int main() {
    const int N = 3;

    // Matrices de prueba 3x3
    std::vector<double> A = {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    };

    std::vector<double> B = {
        9, 8, 7,
        6, 5, 4,
        3, 2, 1
    };

    std::vector<double> C_sum(N * N, 0.0);
    std::vector<double> C_mult(N * N, 0.0);
    std::vector<double> A_trans(N * N, 0.0);

    // 1. Suma: C = A + B
    for (int i = 0; i < N * N; ++i) {
        C_sum[i] = A[i] + B[i];
    }

    // 2. Multiplicación: C = A * B
    for (int i = 0; i < N; ++i) {
        for (int k = 0; k < N; ++k) {
            for (int j = 0; j < N; ++j) {
                C_mult[i * N + j] += A[i * N + k] * B[k * N + j];
            }
        }
    }

    // 3. Transpuesta: A^T
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            A_trans[j * N + i] = A[i * N + j];
        }
    }

    // Mostrar resultados
    print_matrix("Matriz A", A, N, N);
    print_matrix("Matriz B", B, N, N);
    print_matrix("Suma (A + B)", C_sum, N, N);
    print_matrix("Multiplicación (A * B)", C_mult, N, N);
    print_matrix("Transpuesta (A^T)", A_trans, N, N);

    return 0;
}