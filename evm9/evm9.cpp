#include <iostream>
#include <set>
#include <cstdlib>
#include <limits>
#include <algorithm>
#include <intrin.h>
#include <fstream>

#define L1_CACHE_SIZE_BYTE 32768	// 32 KB
#define L2_CACHE_SIZE_BYTE 262144	// 256 KB
#define L3_CACHE_SIZE_BYTE 3215728	// 3 MB
#define NR_OF_COUNTS 30000	// times
#define ELEMENTS_IN_OFFSET 50

using namespace std;

double count(size_t* arr, size_t size)
{
	double curMin = numeric_limits<double>::max();
	size_t index;
	unsigned __int64 startTicks, endTicks;

	for (size_t i = 0; i < NR_OF_COUNTS; ++i) {

		startTicks = __rdtsc();

		for (index = 0, i = 0; i < ELEMENTS_IN_OFFSET * size; ++i)
			index = arr[index];

		endTicks = __rdtsc();

		curMin = min(curMin, (double)(endTicks - startTicks));
	}

	return curMin / ELEMENTS_IN_OFFSET / size;
}

int main(int argc, char* argv[])
{
	ofstream fout;
	fout.open("evm_table.csv");

	for (size_t nrOfOffsets = 1; nrOfOffsets < 40; nrOfOffsets++)
	{
		size_t const size = L1_CACHE_SIZE_BYTE + L2_CACHE_SIZE_BYTE;
		size_t* arr = new size_t[nrOfOffsets * size];

		for (size_t i = 0; i != size; ++i) 
		{
			for (size_t j = 0; j < nrOfOffsets - 1; ++j)
				arr[j * size + i] = (j + 1) * size + i;

			arr[(nrOfOffsets - 1) * size + i] = (i + 1) % size;
		}

		double clocks = count(arr, size * nrOfOffsets);
		cout << nrOfOffsets << ": " << clocks << endl;
		fout << nrOfOffsets << "," << clocks << endl;
		delete[] arr;

	}

	fout.close();

	return 0;
}