/*====================================================
* A* Algorithm Homework 2
* Joel Ruiz II
* CSCI 4350 - Artificial Intelligence
* February 20, 2015
*
* Program Description:
* This A* algorithm finds the best path using a heuristic function based on distance and custom parameters.
*
* ====================================================*/

#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <time.h>
#include <cmath>

using namespace std;

#define MS_IN_SEC 1000.0f

class Utils
{

//-----------------------------------------------------------
public: //static variables

	//Records the elapsed time in seconds for profiling.
	static unsigned long profileStartTime;
	static unsigned long profileEndTime;

//-----------------------------------------------------------
public: //static methods

	/**
	* Easing Functions
	* The following easing functions are used to transform a linear rate,
	* such as a percentage into an exponential one.
	* This allows pushing a point between [0,1] towards 0 more or towards 1 more.
	* @param float t - current time between interval [0,d]
	* @param float b - starting point
	* @param float c - end point
	* @param float d - end time
	* @return float
	*/
	static float easeOutQuint(float t, float b, float c, float d)
	{
		t /= d;
		t--;
		return c*(t*t*t*t*t + 1) + b;
	}
	static float easeInQuint(float t, float b, float c, float d)
	{
		t /= d;
		return c*t*t*t*t*t + b;
	}
	static float easeInQuad(float t, float b, float c, float d)
	{
		t /= d;
		return c*t*t + b;
	}
	static float easeOutQuad(float t, float b, float c, float d)
	{
		t /= d;
		return -c * t*(t - 2) + b;
	}
	static float easeLinear(float t, float b, float c, float d)
	{
		return t;
	}


	/**
	* Profile Start
	* Begins profiling a function.  Nested profiling is not supported.
	* @return void
	*/
	static void profileStart()
	{
		Utils::profileStartTime = clock();
	}


	/**
	* Profile End
	* Ends profiling a function and displays execution time.
	* @param const char* - name of this profile to display in console
	* @return void
	*/
	static void profileEnd(const char* name)
	{
		Utils::profileEndTime = clock();
		float endTime = (float)(Utils::profileEndTime - Utils::profileStartTime) / (float)MS_IN_SEC;
		cout << ">>>> " << name << " execution took " << endTime << "s to complete" << endl;
	}


	/**
	* Truncate
	* Empties an output file.
	* @param const char* - name of output file to delete text
	* @return void
	*/
	static void Truncate(const char *outputFilename)
	{
		std::ofstream ofs;
		ofs.open(outputFilename, std::ofstream::out | std::ofstream::trunc);
		ofs.close();
	}


	/**
	* Split
	* Splits an std::string into vector elements using a delimeter
	* @param string - string to split
	* @param char - delimeter to split string by
	* @param vector<string> - use your own vector<string>
	* @return vector<string>
	*/
	static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}
	/**
	* Split
	* Splits an std::string into vector elements using a delimeter
	* @param string - string to split
	* @param char - delimeter to split string by
	* @return vector<string>
	*/
	static std::vector<std::string> split(const std::string &s, char delim) {
		std::vector<std::string> elems;
		split(s, delim, elems);
		return elems;
	}


	/**
	* Int Max
	* Maximum size of an unsigned integer
	* @return unsigned int
	*/
	static unsigned int IntMax() { return 4294967295; }
};

//default values
unsigned long Utils::profileStartTime = 0;
unsigned long Utils::profileEndTime = 0;

#endif //UTILS_H
