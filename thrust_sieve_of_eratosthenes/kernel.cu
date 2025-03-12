#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/sequence.h>

using std::cout;
using std::cin;

#include <iostream>


int main()
{
    int n;
    cout << "Enter the limit: ";
    cin >> n;

    thrust::host_vector<long> tNum(n);
    thrust::sequence(std::begin(tNum), std::end(tNum));
    thrust::transform(std::cbegin(tNum), std::cend(tNum), std::begin(tNum), [](long x)
        {
            bool result = true;
            long stop = (long)std::ceil(std::sqrt((float)x));
            if (x % 2 != 0) {
                for (long i = 3; i < stop; i += 2) {
                    if (x % i == 0) {
                        result = false;
                        break;
                    };
                }
            }
            else {
                result = false;
            }
            if (!result) x = -1;
            return x;
        });
    for (const auto& element : tNum) if (element > 0) std::cout << element << ", ";
    std::cout << std::endl;

    std::cin.ignore();
    return 0;
}