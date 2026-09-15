#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

const int N = 1024;
const double EPSILON = 0.1;
const double HOT_THRESHOLD = 50.0;

int main() {
    // Usamos vector aplanado de tamaño N * N
    std::vector<double> current(N * N, 0.0);
    std::vector<double> next_grid(N * N, 0.0);

    // Inicialización: bordes fijos a 100.0, interior a 0.0
    for (int j = 0; j < N; ++j) {
        current[0 * N + j] = 100.0;       // Borde superior
        current[(N - 1) * N + j] = 100.0; // Borde inferior
        next_grid[0 * N + j] = 100.0;
        next_grid[(N - 1) * N + j] = 100.0;
    }
    for (int i = 0; i < N; ++i) {
        current[i * N + 0] = 100.0;       // Borde izquierdo
        current[i * N + (N - 1)] = 100.0; // Borde derecho
        next_grid[i * N + 0] = 100.0;
        next_grid[i * N + (N - 1)] = 100.0;
    }

    int iterations = 0;
    bool converged = false;

    while (!converged) {
        iterations++;
        converged = true;

        // 1. Calcular nuevos valores para celdas interiores
        for (int i = 1; i < N - 1; ++i) {
            for (int j = 1; j < N - 1; ++j) {
                double top    = current[(i - 1) * N + j];
                double bottom = current[(i + 1) * N + j];
                double left   = current[i * N + (j - 1)];
                double right  = current[i * N + (j + 1)];
                double center = current[i * N + j];

                next_grid[i * N + j] = (top + bottom + left + right + 4.0 * center) / 8.0;

                // 2. Validar convergencia según la diapositiva:
                // |T(x,y) - (top + bottom + left + right) / 4| > 0.1
                double neighbor_avg = (top + bottom + left + right) / 4.0;
                if (std::abs(center - neighbor_avg) > EPSILON) {
                    converged = false;
                }
            }
        }

        // Intercambiar mallas para la siguiente iteración
        current.swap(next_grid);
    }

    // 3. Contar número de celdas calientes
    int hot_cells = 0;
    for (int i = 0; i < N * N; ++i) {
        if (current[i] > HOT_THRESHOLD) {
            hot_cells++;
        }
    }

    std::cout << "Convergencia alcanzada en " << iterations << " iteraciones." << std::endl;
    std::cout << "Número de celdas calientes (> " << HOT_THRESHOLD << "): " << hot_cells << std::endl;

    return 0;
}