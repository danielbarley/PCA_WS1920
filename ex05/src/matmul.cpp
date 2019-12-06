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

    Matrix A = Matrix(size, size, true, nthreads);
    Vector x = Vector(size, true);

    auto row_start = std::chrono::high_resolution_clock::now();
    Vector r = A * x;
    auto row_end = std::chrono::high_resolution_clock::now();


    std::printf("%u : %.8g s\n",
            size,
            std::chrono::duration<double>(row_end - row_start).count()
            );

    (void)r;
    //(void)r2;
}
