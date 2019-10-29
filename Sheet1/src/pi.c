#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

double get_pi(const size_t N) {
	/*
	 *  calculate pi by approximating the integral over 4/(1+x^2)
	 *  from 0 to 1 using a step function
	 *  parameters:
	 *  size_t N - number of steps
	 *  returns:
	 *  double res - approximation for pi
	 */

	double res = 0.;
	double delta_x = 1. / (double)N;
	double x_i = 0. + (.5 * delta_x);  // move x_i to middle of interval

	for (size_t i = 0; i < N; ++i) {
		res += 4. / (1. + x_i * x_i) * delta_x;
		x_i += delta_x;
	}

	return res;
}

int main(int argc, char *argv[]) {
	struct timeval t1;
	struct timeval t2;

	if (argc != 2) {
		fprintf(stderr,
			"Usage: \t %s <precision>\nNote: \t precision is "
			"poroved in powers of 10\n",
			argv[0]);
		return 1;
	}

	long unsigned iterations = (long int)pow(10, atoi(argv[1]));

	const double PI = 3.141592653589793;

	gettimeofday(&t1, NULL);
	double pi = get_pi(iterations);
	gettimeofday(&t2, NULL);

	long compute_time = (t2.tv_sec * 1000 + t2.tv_usec / 1000) -
			    (t1.tv_sec * 1000 + t1.tv_usec / 1000);

	printf("pi calculated \t\t%.17g\n", pi);
	printf("pi defined \t\t%.17g\n", PI);
	printf("absolute error \t\t%g\n", PI - pi);
	printf("Compute time \t\t%ld ms\n", compute_time);
	return 0;
}
