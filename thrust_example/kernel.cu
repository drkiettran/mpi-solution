#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/generate.h>
#include <thrust/async/copy.h>
#include <thrust/async/reduce.h>
#include <thrust/functional.h>
#include <thrust/sort.h>
#include <thrust/random.h>
#include <numeric>

void example_2() {
	printf("Running example_2\n");
	// Generate random data serially.
	thrust::default_random_engine rng(1337);
	thrust::uniform_real_distribution<double> dist(-50.0, 50.0);
	thrust::host_vector<double> h_vec(32 << 20);
	thrust::generate(h_vec.begin(), h_vec.end(), [&] { return dist(rng); });

	// Transfer to device and compute the sum.
	thrust::device_vector<double> d_vec = h_vec;
	double x = thrust::reduce(d_vec.begin(), d_vec.end(), 0, thrust::plus<int>());
}

void example_1() {
	printf("Running example_1\n");
	// Generate 32M random numbers serially.
	thrust::default_random_engine rng(1337);
	thrust::uniform_int_distribution<int> dist;
	thrust::host_vector<int> h_vec(32 << 20);
	thrust::generate(h_vec.begin(), h_vec.end(), [&] { return dist(rng); });

	// Transfer data to the device.
	thrust::device_vector<int> d_vec = h_vec;

	// Sort data on the device.
	thrust::sort(d_vec.begin(), d_vec.end());

	// Transfer data back to host.
	thrust::copy(d_vec.begin(), d_vec.end(), h_vec.begin());
}

void example_3() {
	printf("Running example_3\n");
	// Generate 32M random numbers serially.
	thrust::default_random_engine rng(123456);
	thrust::uniform_real_distribution<double> dist(-50.0, 50.0);
	thrust::host_vector<double> h_vec(32 << 20);
	thrust::generate(h_vec.begin(), h_vec.end(), [&] { return dist(rng); });

	// Asynchronously transfer to the device.
	thrust::device_vector<double> d_vec(h_vec.size());
	thrust::device_event e = thrust::async::copy(h_vec.begin(), h_vec.end(),
		d_vec.begin());

	// After the transfer completes, asynchronously compute the sum on the device.
	thrust::device_future<double> f0 = thrust::async::reduce(thrust::device.after(e),
		d_vec.begin(), d_vec.end(),
		0.0, thrust::plus<double>());

	// While the sum is being computed on the device, compute the sum serially on
	// the host.
	double f1 = std::accumulate(h_vec.begin(), h_vec.end(), 0.0, thrust::plus<double>());
}

int main() {
	example_1();
	example_2();
	example_3();
}