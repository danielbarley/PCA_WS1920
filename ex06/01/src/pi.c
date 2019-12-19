#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

struct result {
	/*
	 *  results for parallel calculation of pi:
	 */
	double approx_pi;
	double exec_time;
};

void get_pi(const size_t N, const unsigned num_threads, struct result* result) {
	/*
	 *  calculate pi by approximating the integral over 4/(1+x^2)
	 *  from 0 to 1 using a step function
	 *  parameters:
	 *  size_t N - number of steps
	 *  returns:
	 *  double res - approximation for pi
	 */

	omp_set_num_threads(num_threads);
	double res = 0.;
	double delta_x = 1. / (double)N;
	double x_i;  // move x_i to middle of interval
	double time = omp_get_wtime();

#pragma omp parallel for private(x_i) reduction(+ : res)
		for (size_t i = 0; i < N; ++i) {
			x_i = ((double)i / (double)N) +
			      (.5 * delta_x);  // move x_i to middle of interval
			res += 4. / (1. + x_i * x_i) * delta_x;
		}
	time = omp_get_wtime() - time;

	result->approx_pi = res;
	result->exec_time = time;
}

int main(int argc, char* argv[]) {

	if (argc != 3) {
		fprintf(stderr,
			"Usage: \t %s <precision> <num_threads>\nNote: \t "
			"precision is "
			"poroved in powers of 10\n",
			argv[0]);
		return 1;
	}

	long unsigned iterations = (long int)pow(10, atoi(argv[1]));

	const double PI = 3.141592653589793;

	struct result res;

	get_pi(iterations, atoi(argv[2]), &res);

	printf("pi calculated \t\t%.17g\n", res.approx_pi);
	printf("pi defined \t\t%.17g\n", PI);
	printf("absolute error \t\t%g\n", fabs(PI - res.approx_pi));
	printf("Compute time \t\t%g ms\n", res.exec_time * 1000);
	return 0;
}
