#include <iostream>
#include <random>
#include <intrin.h>

#pragma intrinsic(__rdtsc)

using namespace std;

enum WALK_TYPE {UP = 0, DOWN, RANDOM};

void walk(int* arr, int arrSize)
{
	for (int index = 0, i = 0; i < arrSize; i++)
		index = arr[index];
}

void initRandom(int* arr, int arrSize)
{
	default_random_engine generator;
	uniform_int_distribution<int> distribution(0, arrSize - 1);

	bool success;
	int oldRandNum = 0;
	int newRandNum;

	int* randArr = (int*)malloc(sizeof(int) * arrSize);

	for (int i = 0; i < arrSize; i++)
		randArr[i] = -1;

	randArr[oldRandNum] = 0;

	for (int i = 0; i < arrSize; i++)
	{
		
		if (i == arrSize - 1)
			randArr[0] = -1;

		success = false;

		while (!success)
		{
			// generating some random number in [0, arrSize)
			newRandNum = distribution(generator);

			success = true;

			// checking if randNum is already used somewhere
			if (randArr[newRandNum] != -1)
				success = false;

			if (success)
			{
				arr[oldRandNum] = newRandNum;
				randArr[newRandNum] = 0;
				oldRandNum = newRandNum;
			}
		}
	}
}

void initDown(int* arr, int arrSize)
{
	//	<-
	// [7][0][1][2][3][4][5][6]

	for (int i = arrSize - 1; i >= 0; i--)
	{
		if (i > 0)
			arr[i] = i - 1;
		else
			arr[i] = arrSize - 1;
	}
}

void initUp(int* arr, int arrSize)
{
	//	->
	// [1][2][3][4][5][6][7][0]

	for (int i = 0; i < arrSize; i++)
	{
		if (i < arrSize - 1)
			arr[i] = i + 1;
		else
			arr[i] = 0;
	}
}

int getClocks(const int arrSizeKB, WALK_TYPE type)
{
	int arrSize = arrSizeKB * 1024 / (sizeof(int));
	int* arr = (int*)malloc(sizeof(int) * arrSize);

	unsigned __int64 startTicks, endTicks;

	switch (type)
	{
	case UP:
		initUp(arr, arrSize);
		break;
	case DOWN:
		initDown(arr, arrSize);
		break;
	case RANDOM:
		initRandom(arr, arrSize);
		break;
	default:
		break;
	};
	startTicks = __rdtsc();
	walk(arr, arrSize);
	endTicks = __rdtsc();

	free(arr);

	return (endTicks - startTicks);	// sum of clocks
}

int getClocksPerElement(const int arrSizeKB, WALK_TYPE type)
{
	return getClocks(arrSizeKB, type) / arrSizeKB / 1024;
}

void printClocks(const int arrSizeKB)
{
	cout << "ARRAY SIZE: " << arrSizeKB << " KB"
		<< " (= " << arrSizeKB / 1024 << " MB + " << arrSizeKB % 1024 << " KB)" << endl;
	cout << "UP: " << getClocksPerElement(arrSizeKB, UP) << " clocks per element" << endl;
	cout << "DOWN: " << getClocksPerElement(arrSizeKB, DOWN) << " clocks per element" << endl;
	cout << "RANDOM: " << getClocksPerElement(arrSizeKB, RANDOM) << " clocks per element" << endl;
	cout << endl;
}

void printStatistic()
{
	int arrSizeKB = 1;
	int half = 1;

	while (arrSizeKB < 64 * 1024)
	{
		if (arrSizeKB > 1024)
		{
			printClocks(arrSizeKB);
			arrSizeKB += 1024;
		}
		else
		{
			if (arrSizeKB == 1)
			{
				printClocks(arrSizeKB);
				arrSizeKB++;
				printClocks(arrSizeKB);
				arrSizeKB += 2;
			}

			half = arrSizeKB / 2;

			for (int i = 1; i <= 2; i++)
			{
				printClocks(arrSizeKB);
				arrSizeKB += half;
			}
		}
	}
}

int main()
{
	printStatistic();
	return 0;
}