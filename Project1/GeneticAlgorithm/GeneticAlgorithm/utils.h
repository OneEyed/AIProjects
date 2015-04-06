/*====================================================
* Genetic Algorithm Homework 1
* Joel Ruiz II
* CSCI 4350 - Artificial Intelligence
* February 4, 2015
*
* Program Description:
* This Genetic Algorithm will attempt to fill a 32-bit string with all 1s by starting from an initial population of randomized 0s and 1s.
*
* ====================================================*/

#ifndef UTILS_H
#define UTILS_H


#include <time.h>
//#include <random>
using namespace std;


/**
* Utils
* An assortment of helper functions ranging from profiling, debug mode, and random generator wrapper.
* Also included are output header functions for different scenarios as well as printing of the 32-bits for genotypes.
*/
class Utils
{
public:
	
	//-----------------------------------------------------------
	//Debug Mode
	//Used primarily for outputting data to console.  It is great for smaller generation counts.  
	//If you want to try a 1 million or more generations, it is best to set debugging off.
	static bool debugMode;
	static void setDebug(bool debug) { Utils::debugMode = debug; }
	static bool isDebug() { return Utils::debugMode; }

	//-----------------------------------------------------------
	//Random Generator Wrapper
	//I originally had a custom random generation method, but it was reverted back to basic random generation.
	static void seedRandom()
	{
		srand((unsigned int)time(NULL));
	}
	static unsigned int random()
	{
		return rand();
	}
	static unsigned int randomMax()
	{
		return RAND_MAX;
	}

	//-----------------------------------------------------------
	//Profiler
	//Records the elapsed time in seconds for our genetic algorithm to complete all the generations required.
	static unsigned long profileStartTime;
	static unsigned long profileEndTime;
	static void profileStart()
	{
		Utils::profileStartTime = clock();
	}
	static void profileEnd()
	{
		Utils::profileEndTime = clock();
		float endTime = (float)(Utils::profileEndTime - Utils::profileStartTime) / (float)CLOCKS_PER_SEC;
		
		cout << endl << "----------------------------------------------------------------------" << endl;
		cout << endl << "Execution took " << endTime << "s to complete" << endl;
		cout << endl << "----------------------------------------------------------------------" << endl;
	}

	//-----------------------------------------------------------
	//Header Output
	//Prints the tabbed header names for our output that matches the tabbing
	static void outputInitialPopulationHeader()
	{
		if (!isDebug()) return;

		cout << endl << "----------------------------------------------------------------------" << endl;
		cout << "\tInitial Population" << endl << endl;
		cout << "Generation\tBit String\t\t\t\tFitness" << endl;
	}
	static void outputGenerationHeader()
	{
		if (!isDebug()) return;

		cout << endl << "----------------------------------------------------------------------" << endl;
		cout << "\tGeneration Information" << endl << endl;
		cout << "Gen\tBest Gen\tBit String\t\t\t\tFitness" << endl;
	}
	static void outputOptimalHeader()
	{
		cout << endl << "----------------------------------------------------------------------" << endl;
		cout << "\tOptimal Parent" << endl << endl;
		cout << "Generation\tBit String\t\t\t\tFitness" << endl;
	}

	//-----------------------------------------------------------
	//Print Bits
	//Prints each bit to the screen as a 1 or 0.  
	//We have to use bitwise operation to read the bit and check if it is set.
	//The loop starts from maxBits to preserve the ordering of bits in memory.
	static void printBits(unsigned int c, int maxcnt)
	{
		for (int i = maxcnt - 1; i >= 0; i--)
		{
			unsigned int checkbit = c & (1 << i);

			if (checkbit > 0)
				cout << "1";
			else
				cout << "0";
		}
	}
	
	
	//-----------------------------------------------------------
	//Maximum Unsigned Integer
	//The value of 2^32 - 1.  Windows has INT_MAX, but Linux did not.  
	//This is to conform to cross-platform compatibility.
	static unsigned int MAXUINT()
	{
		return 4294967295;
	}

	
};


//default values for our static variables
bool Utils::debugMode = true;
unsigned long Utils::profileStartTime = 0;
unsigned long Utils::profileEndTime = 0;


#endif //UTILS_H