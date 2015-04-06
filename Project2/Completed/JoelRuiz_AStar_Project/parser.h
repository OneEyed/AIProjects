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

#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <iterator>
#include <stdlib.h>

#include "utils.h"

using namespace std;
class Path;

//=====================================================================
//=====================================================================
/**
* City
* Object with city information, acts as a "vertex"
*/
class City
{
public:
	unsigned int id;
	string name;
	float goaldistance;
	vector<Path*> paths;

	void addpath(Path *path) { paths.push_back(path); }
};
//=====================================================================
//=====================================================================
/**
* Path
* Object with path information, acts as an "edge"
*/
class Path
{
public:
	unsigned int distance;
	unsigned int roadQuality;
	float riskLevel;
	City* start;
	City* end;
};
//=====================================================================
//=====================================================================
/**
* class Parser
* This class 
*/
class Parser
{

private: //member variables

	vector<City*> cities;
	map<string, City*> mapOfMiddleEarth;


public: //constructor and destructor

	/**
	* Constructor
	* Creates the cities and paths from two different data files.  
	* @param const char* - table1 with the city name and distance to goal
	* @param const char* - table2 with the path info between cities
	* @return this
	*/
	Parser(const char *filename1, const char *filename2)
	{
		cout << endl << endl;
		Utils::profileStart();
		{
			parseCities(filename1);
			parsePaths(filename2);
		}
		Utils::profileEnd("Parser");
	}

	/**
	* Destructor
	* Delete all city and path objects in memory, and clear our "map"
	* @return void
	*/
	~Parser()
	{
		mapOfMiddleEarth.clear();
		for (int i = 0; i < cities.size(); i++)
		{
			for (int j = 0; j < cities[i]->paths.size(); j++)
				delete cities[i]->paths[j];
			delete cities[i];
		}
	}


public: //member accessors

	/**
	* The Map
	* Retrieves a city by name
	* @param string - the city name
	* @return City*
	*/
	City *themap(const string &key) 
	{
		return mapOfMiddleEarth.find(key)->second;
	}

	/**
	* Add City
	* Places the city in two lists:
	* cities - vector array for indexing
	* mapOfMiddleEarth - map to search by city name
	* @param City* - new city to add to the list
	* @return void
	*/
	void addcity(City *city) 
	{ 
		cities.push_back(city);
		mapOfMiddleEarth.insert(pair<string, City*>(city->name,city)); 
	}

//---------------------------------------------------------------------
public: //methods

	/**
	* Parse Cities
	* Line Format: CityName,GoalDistance
	* @param filename1 - Table with city names and goal distance
	* @return void
	*/
	void parseCities(const char *filename1)
	{
		ifstream file(filename1);
		string line;
		vector<string> elems;
		City *city = NULL;
		string cityName;

		while (getline(file, line))
		{
			//split string into a vector<string> by delimiter
			elems = Utils::split(line, ',');
			cityName = elems[0];

			//create city 
			city = new City();
			city->id = (unsigned int)cities.size();
			city->name = cityName;
			city->goaldistance = (float)atof(elems[1].c_str());
			
			addcity(city);
		}
	}

	/**
	* Parse Paths
	* The start city will be enclosed in brackets [].  Lines that follow define the paths from this city.
	* Paths are two way, so a duplicate path must be created for start and end cities.
	*
	* Block Format:
	*	[CityName]
	*	NeighborName,Distance,Quality,Risk
	*
	* @param filename1 - Table with city names and goal distance
	* @return void
	*/
	void parsePaths(const char *filename2)
	{
		ifstream file(filename2);
		string line;
		vector<string> elems;
		
		City *cityA = NULL;
		City *cityB = NULL;
		Path *pathAB = NULL;
		Path *pathBA = NULL;

		string pathDestination;
		unsigned int pathDistance;
		unsigned int pathQuality;
		float pathRisk;

		while (getline(file, line))
		{
			//skip bad lines
			if (line.size() <= 2)
				continue;

			if (line[0] == '[')
			{
				//remove [ and ] from string
				size_t start = line.find("[") + 1;
				size_t end = line.find("]");
				string cityName = line.substr(start, end-start);
				
				//find city A
				cityA = themap(cityName);
				continue;
			}

			//split string into a vector<string> by delimiter and convert string to numbers
			elems = Utils::split(line, ',');
			pathDestination = elems[0];
			pathDistance = atoi(elems[1].c_str());
			pathQuality = atoi(elems[2].c_str());
			pathRisk = (float)atof(elems[3].c_str());

			//find city B
			cityB = themap(pathDestination);

			//create path from B to A
			pathBA = new Path();
			pathBA->start = cityB;
			pathBA->end = cityA;
			pathBA->distance = pathDistance;
			pathBA->roadQuality = pathQuality; 
			pathBA->riskLevel = pathRisk;
			cityB->addpath(pathBA);

			//create path from A to B
			pathAB = new Path();
			pathAB->start = cityA;
			pathAB->end = cityB;
			pathAB->distance = pathDistance;
			pathAB->roadQuality = pathQuality;
			pathAB->riskLevel = pathRisk;
			cityA->addpath(pathAB);
		}
	}
};

#endif //PARSER_H
