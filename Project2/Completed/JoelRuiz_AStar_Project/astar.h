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

#ifndef ASTAR_H
#define ASTAR_H


#include <typeinfo>
#include <cmath>
#include <string>
#include <set>
#include <iomanip>
#include <algorithm>

#include "parser.h"


//=====================================================================
//=====================================================================
/**
* class AStarNode
* This class defines a route of paths between cities, with heuristic information stored
*/
class AStarNode
{
public:
	AStarNode *prev;
	AStarNode *next;
	City *city;
	Path *path;
	unsigned int depth;
	float g_prev;
	float g;
	float h;
	float f;
};

//=====================================================================
//=====================================================================
/**
* class AStarNodeMinComparer
* This class is used for our std::set, to keep an ordered list.
* This specific comparer sorts based on this priority:
* 1) f(x)
* 2) path depth
* 3) distance to goal
*/
class AStarNodeMinComparer{
public:
	bool operator()(AStarNode* lhs, AStarNode* rhs) const {
		
		if (lhs->f == rhs->f)
		{
			if (lhs->depth == rhs->depth)
				return lhs->h > rhs->h;
			return (lhs->depth < rhs->depth); //depth is sorted with higher is better
		}
		return (lhs->f > rhs->f); //function is sorted with smaller is better
	}
};

//=====================================================================
//=====================================================================
/**
* class AStar
* The core A* algorithm class.  
* You must pass a custom Heuristic class to the function.  View "astarheuristics.h" for examples.
* The three important functions of this class are:
*
* void run(City *start, City *end)
*	//Process the A* algorithm from start to end
*
* AStarNode *TestSuccessor(Path *path)
*	//Tests a new path from the current city
*
* void print(const char *outputFilename)
*	//Outputs the results to console and output file
* 
*/
template <class Heuristic>
class AStar
{

//---------------------------------------------------------------------
private: //member variables


	AStarNode *root;
	AStarNode *current;

	set<AStarNode*, AStarNodeMinComparer> m_Opened;
	set<AStarNode*, AStarNodeMinComparer> m_Closed;

	City *m_StartCity;
	City *m_EndCity;

	Heuristic heuristic;

//---------------------------------------------------------------------
public: //destructor

	/**
	* Clean
	* Deletes all the nodes from Opened and Closed lists
	* @return void
	*/
	void clean()
	{
		set<AStarNode*, AStarNodeMinComparer>::iterator it;
		set<AStarNode*, AStarNodeMinComparer>::iterator temp;
		AStarNode *node;

		for (it = m_Opened.begin(); it != m_Opened.end();) 
		{
			node = (*it);
			m_Opened.erase(it++);
			delete node;
		}

		for (it = m_Opened.begin(); it != m_Opened.end();) 
		{
			node = (*it);
			m_Closed.erase(it++);
			delete node;
		}
	}

//---------------------------------------------------------------------
public: //methods

	/**
	* Run
	* Performs the A* algorithm on our graph of cities and paths
	* @param City* - starting city
	* @param City* - goal city
	* @return AStarNode* - the newly created successor
	*/
	void run(City *start, City *end)
	{
		Utils::profileStart();

		//Setup the root node
		root = new AStarNode();
		root->depth = 0;
		root->city = start;
		root->prev = NULL;
		root->g = 0;
		root->h = start->goaldistance;
		root->f = root->g + root->h;

		//add root node to opened list, this node will be first of the A* loop
		m_Opened.insert(root);

		//iterators for city paths, opened, and closed lists
		vector<Path* >::iterator it;
		set<AStarNode*, AStarNodeMinComparer>::iterator findClosed;
		set<AStarNode*, AStarNodeMinComparer>::iterator findOpened;

		//reusable AStarNodes
		AStarNode *successor;

		//loop until we reach our destination or exhaust all routes
		while (m_Opened.size() > 0)
		{
			//set current to the best city in our route
			current = (*m_Opened.rbegin());
			findClosed = m_Closed.find(current);
			if (findClosed != m_Closed.end())
				continue;
			
			m_Closed.insert(current);

			//exit loop if we reach destination
			if (current->city == end)
				break;

			//printNode(current);

			//remove current from being tested again
			m_Opened.erase(current);

			//iterate through each path from current city
			for (it = current->city->paths.begin(); it != current->city->paths.end(); it++)
			{
				//create a test node and check against the A* heuristic function
				successor = TestSuccessor((*it));
			
				//if IN closed list, skip and remove from opened list
				findClosed = m_Closed.find(successor);
				if (findClosed != m_Closed.end())
				{
					delete successor;
					continue;
				}
					
				//if NOT in opened list OR test's heuristic performed better than current, add as possible successor
				findOpened = m_Opened.find(successor);
				if (findOpened == m_Opened.end() || successor->f < current->f)
				{
					if (findOpened != m_Opened.end())
						continue; //skip

					m_Opened.insert(successor);
					continue; //success
				}

				//cleanup unused
				delete successor; 
			}
		}

		Utils::profileEnd(typeid(Heuristic).name());
	}


	/**
	* Test Successor
	* Creates a new successor and processes it through the heuristic to determine it's function values
	* @param Path* - the path we must test against
	* @return AStarNode* - the newly created successor
	*/
	AStarNode *TestSuccessor(Path *path)
	{
		AStarNode *result = new AStarNode();

		result->depth = current->depth + 1;
		result->city = path->end;
		result->path = path;
		result->prev = current;

		result->g = heuristic.g(result);
		result->h = heuristic.h(result);
		result->f = result->g + result->h;
		
		return result;
	}
	

	/**
	* Print
	* Prints the route the A* algorithm determined was best
	* @param ostream& - the stream to output to
	* @return void
	*/
	void print(ostream &out)
	{
		out << "==========================================================================" << endl;
		out << "Results -- " << typeid(Heuristic).name() << endl;
		out << heuristic.desc() << endl;
		out << "==========================================================================" << endl;
		out << setw(4) << left << "#" << setw(8) << "Dist" << setw(8) << "g(x)" << setw(8) << "h(x)" << setw(8) << "f(x)" << setw(20) << "City" << endl;
		out << "--------------------------------------------------------------------------" << endl;
		
		//loop up to the root node and set the next of each node
		while (current != NULL)
		{
			if ( current->prev != NULL )
				current->prev->next = current;
			current = current->prev;
		}
		
		//reset to root
		current = root;

		//loop down to the tail node and output data
		unsigned int totalDistance = 0;
		int count = 1;
		while ( current != NULL )
		{
			//output step #
			out << setw(4) << left << count++;

			//accumilate total distance
			if (current != root && current->prev != NULL)
			{
				totalDistance += current->path->distance;
				out << setw(8) << current->path->distance;
			}
			else
			{
				out << setw(8) << "0";
			}
				
			//output functions
			out << setw(8) << round(current->g);
			out << setw(8) << round(current->h);
			out << setw(8) << round(current->f);
			
			//output city name
			out << setw(8) << current->city->name << endl;

			current = current->next;
		}
		out << "Total distance traveled: " << totalDistance << endl;
	}


	/**
	* Print
	* Prints to the console and file
	* @param const char* - the filename to output to
	* @return void
	*/
	void print(const char *outputFilename)
	{
		ofstream realfile(outputFilename, ios::out | ios::app);

		cout << endl << endl;
		print(cout);
		print(realfile);
		
	}
};



#endif //ASTAR_H