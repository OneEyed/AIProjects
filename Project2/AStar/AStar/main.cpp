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

#include <iostream>
#include <stdio.h>

#include "parser.h"
#include "astar.h"

using namespace std;

//-------------------------------------------------------------
//open this file to manage HEURISTIC functions
#include "astarheuristics.h"
//-------------------------------------------------------------


int main()
{
	//variables
	Parser *parser;
	AStar<AStarHeuristicFirst> astar1;
	AStar<AStarHeuristicSecond> astar2;
	AStar<AStarHeuristicThird> astar3;

	//parse city and path data
	parser = new Parser("table1.txt", "table2.txt");

	//process A* with 3 different heuristics
	City *start = parser->themap("Blue Mountains");
	City *end = parser->themap("Iron Hills");
	astar1.run(start, end);
	astar2.run(start, end);
	astar3.run(start, end);
	
	//output
	const char *outputFilename = "output.txt";
	Utils::Truncate(outputFilename);
	astar1.print(outputFilename);
	astar2.print(outputFilename);
	astar3.print(outputFilename);

	//cleanup
	astar1.clean();
	astar2.clean();
	astar3.clean();
	delete parser;

	//system("pause");
	return 0;
}
