#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

struct targs {
	/*
	 *  argument struct:
	 *  holds function call parameters for get_pi_parallel_impl
	 */
	const size_t id;     // current thread id
	const size_t N;	     // number of iterations in total
	const size_t lower;  // sum starts here
	const size_t upper;  // sum ends here
	double* res;	     // pointer to write result to
};

struct result {
	/*
	 *  results for parallel calculation of pi:
	 */
	double approx_pi;
	double exec_time;
};

void* memdup(const void* src, size_t size) {
	/*
	 *  generic memory allocation function similar to strdup
	 */
	void* mem = malloc(size);
	return mem ? memcpy(mem, src, size) : NULL;
}

// macro for memory allocation and direct assignment
#define ALLOC_INIT(type, ...) (type*)memdup((type[]){__VA_ARGS__}, sizeof(type))
#define MIN(a, b) (a < b ? a : b)

void* get_pi_parallel_impl(void* args) {
	/*
	 *  calculate partial sum from lower to upper bound
	 *  and use reduction afterwards to get final result
	 *  parameters:
	 *  struct args -> contains:
	 *  size_t lower - start sum from here
	 *  size_t upper - calculate sum until here
	 *  res          - pointer to write result to
	 *  modifies:
	 *  double res   - partial sum of elements within bounds
	 */

	struct targs* t = (struct targs*)args;
	double delta_x = 1. / (double)(t->N);
	double x_i = (double) t->lower / t->N + (.5 * delta_x);

	for (size_t i = t->lower; i < t->upper; ++i) {
		*t->res += 4. / (1. + x_i * x_i) * delta_x;
		x_i += delta_x;
	}
	pthread_exit((void*)args);
}

void get_pi_parallel(const size_t N, const unsigned num_threads,
		     struct result* res) {
	/*
	 *  wrapper function
	 *  divides problem according to num_threads and produces
	 *  partial sums appropriately, these sums can then be reduced
	 *  to single result.
	 *  Since we only expect up to 32 different results at max
	 *  we simply reduce serially
	 *  parameters:
	 *  size_t N             - number of total iterations
	 *  unsigned num_threads - number of threads
	 *  modifies:
	 *  sruct result         - conains approximation for pi
	 *                         and execution time
	 */

	struct timeval tv1, tv2;  // measure time
	pthread_t* threads;	  // keep threads in array
	pthread_attr_t attr;	  // all threads share the same attributes
	double* partial_sums;	  // buffer for intermediate results
	struct targs** ts;	  // keeps arguments for all threads
	void* status;		  // collect status from pthread functions

	pthread_attr_init(&attr);  // make threads joinable
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	threads = malloc(sizeof(pthread_t) * num_threads);
	partial_sums = calloc(num_threads, sizeof(double));
	ts = malloc(sizeof(struct targs*) * num_threads);

	// divide problem into appropriate chunks
	size_t its_per_thread =
	    (N + num_threads - 1) / num_threads;  // ceil(N / numthreads)

	for (size_t i = 0; i < num_threads; ++i) {
		*(ts + i) = ALLOC_INIT(
		    struct targs, {.id = i,
				   .N = N,
				   .lower = i * its_per_thread,
				   // avoid overshooting by checking for min()
				   .upper = MIN((i + 1) * its_per_thread, N),
				   .res = &partial_sums[i]});
	}

	// START COMPUTE SECTION
	gettimeofday(&tv1, NULL);

	int rc;	 // fetch return code of phtread_create
	for (unsigned i = 0; i < num_threads; ++i) {
		rc = pthread_create(&threads[i], &attr, get_pi_parallel_impl,
				    (void*)*(ts + i));
		if (rc) {
			fprintf(
			    stderr,
			    "ERROR; return code from pthread_create() is %d\n",
			    rc);
			exit(-1);
		}
	}
	pthread_attr_destroy(&attr);
	for (unsigned i = 0; i < num_threads; ++i) {
		rc = pthread_join(threads[i], &status);
		if (rc) {
			fprintf(
			    stderr,
			    "ERROR; return code from pthread_join() is %d\n",
			    rc);
			exit(-1);
		}
	}

	// serial reduction -> since we only expect up to 32 values
	double pi = 0.;
	for (unsigned i = 0; i < num_threads; ++i) {
		pi += partial_sums[i];
	}

	gettimeofday(&tv2, NULL);
	// END COMPUTE SECTION

	res->exec_time = (double)(tv2.tv_usec - tv1.tv_usec) / 1000000 +
			 (double)(tv2.tv_sec - tv1.tv_sec);
	res->approx_pi = pi;
	// free allocated resources
	free(threads);
	free(partial_sums);
	for (size_t i = 0; i < num_threads; ++i) {
		free(ts[i]);
	}
	free(ts);
}

int main(int argc, char* argv[]) {
	struct result res;

	if (argc != 3) {
		fprintf(stderr,
			"Usage: \t %s <precision> <num_threads>\nNote: \t "
			"precision is "
			"provided in powers of 10\n",
			argv[0]);
		return 1;
	}

	long unsigned iterations = (long unsigned)pow(10, atoi(argv[1]));
	unsigned num_threads = (unsigned)atoi(argv[2]);
	assert(num_threads < iterations);

	const double PI = 3.141592653589793;

	get_pi_parallel(iterations, num_threads, &res);

	printf("pi calculated \t\t%.17g\n", res.approx_pi);
	printf("pi defined \t\t%.17g\n", PI);
	printf("absolute error \t\t%g\n", fabs(PI - res.approx_pi));
	printf("Compute time \t\t%g ms\n", res.exec_time * 1000);
	return 0;
}
