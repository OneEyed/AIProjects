#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <math.h>
#include <queue>

using namespace std;

struct MinHeapCompartor { bool operator()(string left, string right){ return left.compare(right) > 0; } };

vector<string> combinateString(string str);
void permute(char *a, int i, int n, priority_queue<string, std::vector<string>, MinHeapCompartor> *list);
void swap(char *x, char *y);



int main(int argc, char *argv[]) {
	//ifstream stream(argv[1]);
	//string line;
	//while (getline(stream, line)) {
		// Do something with the line
	//}
	//combinateString("abcd");
	priority_queue<string, std::vector<string>, MinHeapCompartor> sortedList;
	char a[] = "aabc";
	permute(a, 0, 3, &sortedList);
	
	while (sortedList.size() > 0)
	{
		cout << sortedList.top();
		if (sortedList.size() > 1)
			cout << ",";

		sortedList.pop();
	}
	cout << endl;

	system("pause");
	return 0;
}





/* Function to swap values at two pointers */
void swap(char *x, char *y)
{
	char temp;
	temp = *x;
	*x = *y;
	*y = temp;
}

/* Function to print permutations of string
This function takes three parameters:
1. String
2. Starting index of the string
3. Ending index of the string. */
void permute(char *a, int i, int n, priority_queue<string, std::vector<string>, MinHeapCompartor> *list)
{
	int j;
	if (i == n)
	{
		string copy = a;
		list->push(copy);
	}
	else
	{
		for (j = i; j <= n; j++)
		{
			swap((a + i), (a + j));
			permute(a, i + 1, n, list);
			swap((a + i), (a + j)); //backtrack
		}
	}
}

/*
Input:	
	hat
	abc
	Zu6
Output:
	aht, ath, hat, hta, tah, tha
	abc, acb, bac, bca, cab, cba
	6Zu, 6uZ, Z6u, Zu6, u6Z, uZ6

Algorithm:
1) Loop through each character in string and add to minheap (so it sorts a lower than z)
2) Pop all characters from minheap to create new character sorted string
3) Create a new vector list of strings
4) Properties of string combination:
		a) There are n! combinations, where n = character count.
		b) We already have the first string
		c) 1st characters are repeated m! times, where m = (n-1) 
		
			
Ex. hat:  0 => t, 1 => a, 2 => h
*/
vector<string> combinateString(string str)
{
	vector<string> sortedList;
	unsigned int n = (unsigned int)str.length();
	unsigned int m = n-1;
	unsigned int nFactorial = 1;
	unsigned int mFactorial = 1;
	string sortedStr(str);
	priority_queue<char, std::vector<char>, MinHeapCompartor> minHeap;

	//calculate factorial and sort string
	for (unsigned int i = n; i > 0; i--)
	{
		nFactorial = nFactorial * i;
		minHeap.push(str[i-1]);
	}

			
	//dump sorted heap into string
	while (minHeap.size() > 0)
	{
		sortedStr[n - minHeap.size()] = minHeap.top();
		minHeap.pop();
	}

	cout << "n = " << n << endl;
	cout << "nFactorial = " << nFactorial << endl;
	cout << "m = " << m << endl;
	cout << "mFactorial = " << mFactorial << endl;
	cout << "sortedStr = " << sortedStr << endl;

	//cout << sortedStr;

	//calculate first character repeat amount (n-1)!
	mFactorial = nFactorial / n;

	//we have the first character
	sortedList.push_back(sortedStr);

	string test(str);
	

	unsigned int start;
	unsigned int cnt;
	unsigned int cur;
	unsigned int z;
	//loop through each character
	for (unsigned int x = 0; x < n; x++)
	{
		test[0] = sortedStr[x];
		start = 0;
		cur = 0;
		//loop through each first character repeated amount; m times
		for (unsigned int y = 0; y < mFactorial; y++)
		{
			string copy(test);
			cnt = 0;
			z = 1;
			
			cur = start++ % n;
			if (x == 0) cur++;

			//loop through 2nd to nth character
			while (z < n)
			{
				cur = cur % n;
				if (cur == x) cur++;
				copy[z++] = sortedStr[(cur++ % n)];
			}
				
			sortedList.push_back(copy);
			//cout << "," << copy;
			cout << copy << endl;
		}
	}

	cout << endl;
		
	return sortedList;
}