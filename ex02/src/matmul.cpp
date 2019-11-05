#include <iostream>
#include <iomanip>
#include <chrono>
#include <valarray>

#include "matrix.hpp"

int main() {
    srand(time(NULL));
    std::array<int, 6> sizes = {10, 100, 500, 1000, 5000, 10000};
    //std::array<int, 4> sizes = {10, 100, 500, 1000};

    for (auto size: sizes){

        Matrix A = Matrix(size, true);
        Vector x = Vector(size, true);

        auto row_start = std::chrono::high_resolution_clock::now();
        Vector r = A * x;
        auto row_end = std::chrono::high_resolution_clock::now();

        auto col_start = std::chrono::high_resolution_clock::now();
        Vector r2 = A | x;
        auto col_end = std::chrono::high_resolution_clock::now();

        std::printf("%u x %u:\tt_row = %.8gs,\tt_col = %.8gs,\tt_row/t_col = %.8g\n",
            size,
            size,
            std::chrono::duration<double>(row_end - row_start).count(),
            std::chrono::duration<double>(col_end - col_start).count(),
            std::chrono::duration<double>(row_end - row_start).count()/
            std::chrono::duration<double>(col_end - col_start).count()
        );

        (void)r;
    }
}
