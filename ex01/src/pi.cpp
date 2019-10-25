#include <iostream>
#include <iomanip>
#include <cmath>
#include <numeric>

// F(x)
double approx(double x) {
    return 4/(1+x*x);
}

// int{F(x}
double pi_approx(uint64_t N){
    double res = 0;
    double dx = 1/(double)N;
    for (double n = 0; n < N; n+=1){
        res += approx((n+.5)*dx)*dx;
    }
    return res;
}

int main(int argc, char** argv) {
    double res = 0;

    int i = (int)(argv[1][0]-48);
    uint64_t N = std::pow(10, i);
    res = pi_approx(N);
    std::cout << std::setprecision(15) << "PI ~= " << res;
    std::cout << std::setprecision(5);
    std::cout << " (w/ " << N << " iterations, err: " << M_PI - res << ")" << std::endl;
}
