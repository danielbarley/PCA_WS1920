#include <iostream>
#include <iomanip>
#include <chrono>
#include <valarray>
#include <vector>

#include "argparse/argparse.hpp"
#include "matrix.hpp"

int main(int argc, const char** argv) {
    srand(time(NULL));

    argparse::ArgumentParser ap;

    ap.addArgument("-d", "--dimension", 1, false);
    ap.addArgument("-n", "--threads", 1, true);
    ap.parse(argc, argv);

    int size = ap.retrieve<int>("dimension");
    int nthreads = 1;
    if(ap.gotArgument("threads")) {
        nthreads = ap.retrieve<int>("threads");
    }

    Matrix A = Matrix(size, true, nthreads);
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
    (void)r2;
}
