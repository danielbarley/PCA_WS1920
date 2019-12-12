#include <iostream>
#include <iomanip>
//#include <chrono>
#include <omp.h>
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

    auto row_start = omp_get_wtime();
    Vector r = A * x;
    auto row_end = omp_get_wtime();


    std::printf("%u : %.8g s (OMPS: %s)\n",
            size,
            row_end-row_start,
#ifdef OMP_SCHEDULING
#define str(x) #x
#define STRING(x) str(x)
            STRING( OMP_SCHEDULING )
#else
            "static"
#endif
            );

    (void)r;
    //(void)r2;
}
