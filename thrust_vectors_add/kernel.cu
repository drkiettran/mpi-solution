#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/transform.h>
#include <thrust/functional.h>

#define size 1024*1024

int main() {
    // Initialize host vectors
    thrust::host_vector<int> h_vec1(size);
    thrust::host_vector<int> h_vec2(size);

    // Fill the vectors with data
    for (int i = 0; i < size; i++) {
        h_vec1[i] = i;
        h_vec2[i] = i * 2;
    }

    // Transfer host vectors to device vectors
    thrust::device_vector<int> d_vec1 = h_vec1;
    thrust::device_vector<int> d_vec2 = h_vec2;
    thrust::device_vector<int> d_result(size);

    // Add vectors element-wise
    thrust::transform(d_vec1.begin(), d_vec1.end(), d_vec2.begin(), d_result.begin(), thrust::plus<int>());

    // Transfer result back to host
    thrust::host_vector<int> h_result = d_result;

    // Display results
    //for (int i = 0; i < size; i++) {
    //    std::cout << h_result[i] << std::endl;
    //}

    return 0;
}
