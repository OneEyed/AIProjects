#include <iostream>

using namespace std;

int main()
{
	int *MemArray;

	MemArray = new int[100];
	for (int i = 0; i < 100; i++)
	{
		MemArray[i] = 1;
	}

	int result = 0;
	int x = 0;

	for (int i = 0; i < 100; i++)
	{
		result = result + *(MemArray);
		MemArray++;
	}

	cout << "RESULT = " << result << endl;

	system("pause");

	return 0;
}