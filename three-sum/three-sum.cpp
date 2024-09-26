#include <iostream>

class ThreeSum {
public:
	int count(int* a, int N)
	{ // Count triples that sum to 0.
		int cnt = 0;
		for (int i = 0; i < N; i++)
			for (int j = i + 1; j < N; j++)
				for (int k = j + 1; k < N; k++)
					if (a[i] + a[j] + a[k] == 0)
						cnt++;
		return cnt;
	}
	
};

void main(int argc, char* argv[])
{
	ThreeSum ts = ThreeSum();
	int a[5] = { 1,2,3,4,5 };
	std::cout << ts.count(a, 5);
}