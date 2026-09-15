#include <iostream>
#include <vector>

int main() {
    // Matriz A (4x3) y Vector v (3x1) del ejemplo
    const int rows = 4;
    const int cols = 3;

    int A[rows][cols] = {
        { 1,  4,  0},
        { 4,  2,  0},
        { 1,  5, -2},
        {-1,  4, -4}
    };

    int v[cols] = {-3, 3, 4};
    int result[rows] = {0};

    // Multiplicación Matriz-Vector secuencial
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            result[i] += A[i][j] * v[j];
        }
    }

    // Mostrar resultado por salida estándar
    std::cout << "Resultado (A * v):" << std::endl;
    for (int i = 0; i < rows; ++i) {
        std::cout << "[ " << result[i] << " ]" << std::endl;
    }

    return 0;
}