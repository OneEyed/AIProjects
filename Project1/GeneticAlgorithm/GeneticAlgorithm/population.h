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

#ifndef POPULATION_H
#define POPULATION_H



#include <stdint.h>
#include <stdlib.h>
#include <iostream>     // std::cout
#include <fstream>		// output to file
#include <algorithm>    // std::make_heap, std::pop_heap, std::push_heap, std::sort_heap
#include <vector>       // std::vector
#include <queue>		// std::priority_queue
#include <stack>		// std::stack
#include "utils.h"		// helper class for misc
#include "parent.h"		// parent class that holds an individuals data
//using namespace std;



//Parent class exposed
class Parent;	

//Comparator used for our Mating Pool maximum fitness sorting of a priority queue
class FitnessMaxComparer{public: bool operator()(Parent *lhs,  Parent *rhs) const {	return (lhs->fitness() < rhs->fitness()); }};

//Comparator used for our Population's minimum fitness sorting of a priority queue
class FitnessMinComparer{public: bool operator()(Parent* lhs,  Parent* rhs) const { return (lhs->fitness() > rhs->fitness()); }};



/**
* Population
* The genetic algorithm is processed through this class.  
* It handles creation of initial parents and processing N number of generations which consists of:
* 1) Select random parents for mating
* 2) Pair the parents in tournament format
* 3) Crossover the genotypes of the parents to new children
* 4) Add children to population
* 5) Survivor Selection
* Also handled is the debug output of each generation and data file output of optimal solution for run.
*/
class Population
{

//-----------------------------------------------
private: //Member Variables
//-----------------------------------------------

	Parent *m_OptimalParent;	//optimal fit parent
	Parent **m_Parents;			//master array of parents for indexing
	Parent **m_MatesList;		//mating pool array list for indexing

	//min fitness sorted list of parents, not indexable
	priority_queue<Parent*, vector<Parent*>, FitnessMinComparer> m_ParentsMinSorted;

	//max fitness sorted list of mating pool parents, not indexable
	priority_queue<Parent*, vector<Parent*>, FitnessMaxComparer> m_MatingPool;

	//stack list of new children that need to be moved to master array of parents
	stack<Parent*> m_NewChildren;

	//stack list of killed parents to be reincarnated as children
	stack<Parent*> m_ReusableChildren;

	//population settings
	int m_iMaxPopulation;
	int m_iMaxPoolCount;
	int m_iMaxParentBits;
	int m_iCurrentGeneration;
	float m_flParentBitProbability;
	float m_flCrossoverProbability;
	float m_flMutationProbability;


//-----------------------------------------------
public: //Constructor/Destructor
//-----------------------------------------------

	Population()
	{
		Utils::seedRandom();	//Seed the Random Generator
		
		m_iCurrentGeneration = 0;
		m_Parents = NULL;
		m_OptimalParent = NULL;
	}
	
	~Population()
	{
		//cleanup allocated memory
		delete[] m_Parents;
		delete[] m_MatesList;
		while (m_ReusableChildren.size() > 0) {
			delete m_ReusableChildren.top();
			m_ReusableChildren.pop();
		}
	}


//-----------------------------------------------
public: //Variable Accessors
//-----------------------------------------------

	void setMaxPopulation(int maxpop) { m_iMaxPopulation = maxpop; }
	int maxPopulation() { return m_iMaxPopulation; }
	
	void setMaxParentBits(int maxbits) { m_iMaxParentBits = maxbits; }
	int maxParentBits() { return m_iMaxParentBits; }

	float parentBitProbability() { return m_flParentBitProbability; }
	void setParentBitProbability(float bitProbability) { m_flParentBitProbability = bitProbability; }

	void setMaxPoolCount(int maxPoolCount) { m_iMaxPoolCount = maxPoolCount; }
	int maxPoolCount() { return m_iMaxPoolCount; }

	void setCrossoverProbability(float p) { m_flCrossoverProbability = p; }
	float crossoverProbability() { return m_flCrossoverProbability; };

	void setMutationProbability(float p) { m_flMutationProbability = p; }
	float mutationProbability() { return m_flMutationProbability; }

	int incrementGeneration() { return m_iCurrentGeneration++; }
	int currentGeneration() { return m_iCurrentGeneration; }

	Parent *optimalFitParent() { return m_OptimalParent; }


//-----------------------------------------------
public: //Class Methods
//-----------------------------------------------

	/**
	* Run Generations
	* Process the max generations of the genetic algorithm and exit if an optimal fitness was found.
	* Outputs debug information if activated in main.cpp
	* Saves solution result to CSV file for scatterplot chart
	* @param maxGenerations - The amount of times to perform the genetic algorithm on an initial sample population
	* @return void
	*/
	void RunGenerations(int maxGenerations)
	{
		//output debugging generation data header 
		Utils::outputGenerationHeader();
		
		for (int i = 0; i < maxGenerations; i++)
		{
			//process exactly one generation
			RunOneGeneration();

			//output debugging info of optimal parent each generation
			if (Utils::isDebug())
			{
				cout << i << "\t";
				optimalFitParent()->print();
			}

			//if we found optimal, we can stop processing generations
			if (checkIfHasOptimal())
				break;
		}

		//output final optimal parent
		Utils::outputOptimalHeader();
		optimalFitParent()->print();

		//output run to csv file
		saveRun();
	}


	/**
	* Run One Generation
	* Process a single generation by following the genetic algorithm formula of:
	* 1) Select random parents for mating
	* 2) Pair the parents in tournament format
	* 3) Crossover the genotypes of the parents to new children
	* 4) Add children to population
	* 5) Survivor Selection
	* @return int - The current generation number
	*/
	int RunOneGeneration()
	{
		//start by generating our mating pool of random parents using with replacement
		processMatingPool();

		//children creation, two-point crossover, and mutation performed here
		processTournament();	

		//we have all the children added to population, now lets truncate our total population back to the max allowed
		processSurvivorSelection();

		return incrementGeneration();
	}


	/**
	* Generate Parents
	* Allocate memory for our Parent objects and generate their genotypes
	* m_Parents = master array for indexing parents
	* m_ParentsMinSorted = min heap of parents sorted by fitness, may grow larger than master array
	* m_MatesList = array used to index parents from the sorted mating pool
	* m_ReusableChildren = stack to reuse parents killed off from the population
	* m_OptimalParent = parent with the best fitness
	* @return void
	*/
	void GenerateParents()
	{
		//output debugging info initial population
		Utils::outputInitialPopulationHeader();

		//allocate memory for master list of parent population
		m_Parents = new Parent*[maxPopulation()];

		for (int i = 0; i < maxPopulation(); i++)
		{
			//create parent and generate genotypes
			m_Parents[i] = new Parent(i);
			m_Parents[i]->setMaxBits(maxParentBits());
			m_Parents[i]->setBitProbability(parentBitProbability());
			m_Parents[i]->Generate();
			if (Utils::isDebug())
				m_Parents[i]->print();
			

			//maintain a min heap of parents sorted by fitness 
			m_ParentsMinSorted.push(m_Parents[i]);

			//maintain who is the optimal parent
			if (i == 0)
				m_OptimalParent = m_Parents[i];
			else
				checkIfOptimal(m_Parents[i]);
		}

		//allocate array for mating pool list of parents
		m_MatesList = new Parent*[maxPoolCount()];

		//allocate extra parents for use as children
		for (int i = 0; i < maxPoolCount(); i++)
			m_ReusableChildren.push(new Parent(-1, maxParentBits(), parentBitProbability()));
	}
	

//-----------------------------------------------
private: //Class Methods
//-----------------------------------------------

	/**
	* Process Mating Pool
	* For each slot in our mating pool, randomly select 3 parents from the population master array using with replacement.
	* Then, determine the best fitness of the 3 parents, and add the best parent to the mating pool slot
	* @return void
	*/
	void processMatingPool()
	{
		//clear out the mating pool
		while (m_MatingPool.size() > 0)
			m_MatingPool.pop();
		
		//temp variables
		Parent *p1, *p2, *p3;
		Parent *best;
		int maxPop = maxPopulation();
		int current = 0;
		
		//only add max allowed to pool
		for (int i = 0; i < maxPoolCount(); i++)
		{
			//3 randomly selected parents using with replacement
			p1 = m_Parents[Utils::random() % maxPop];
			p2 = m_Parents[Utils::random() % maxPop];
			p3 = m_Parents[Utils::random() % maxPop];

			//select best fit of the first 2 parents
			if (p1->fitness() > p2->fitness())
				best = p1;
			else
				best = p2;

			//compare the best of first 2 parents with the last (3rd) parent
			if (p3->fitness() > best->fitness())
				best = p3;

			//add best parent to mating pool
			m_MatingPool.push(best);
		}
	}


	/**
	* Process Tournament
	* First, move the mating pool sorted list (by max fitness) to an array to allow indexing.
	* Then, combine parents by highest vs lowest until they meet at the middle fitness values.
	* This process is similar to single elimination bracket seeding.
	* @return void
	*/
	void processTournament()
	{
		//move mating pool sorted list to array
		int pos = 0;
		int maxcnt = m_MatingPool.size() - 1;
		while (m_MatingPool.size() > 0)
		{
			m_MatesList[pos++] = m_MatingPool.top();
			m_MatingPool.pop();
		}

		//find half point
		int halfcnt = (maxcnt+1) / 2;
		Parent *left, *right;

		//only loop through half the mates
		for (int i = 0; i < halfcnt; i++)
		{
			//left = high fitness, right = low fitness
			left = m_MatesList[i];
			right = m_MatesList[maxcnt - i];

			//perform the genotype processing
			processChildren(left, right);
		}
	}


	/**
	* Process Children
	* Setup two new children from the reusable children stack to process two-point crossover
	* @param left - Left Parent for crossover
	* @param right - Right Parent for crossover
	* @return void
	*/
	void processChildren(Parent *left, Parent *right)
	{
		Parent *child1 = m_ReusableChildren.top();
		m_ReusableChildren.pop();
		child1->setGene(0);
		child1->setFitness(0);

		Parent *child2 = m_ReusableChildren.top();
		m_ReusableChildren.pop();
		child2->setGene(0);
		child2->setFitness(0);

		processCrossover(left, right, child1, child2);
	}



	/**
	* Process Crossover
	* First check against crossover probability.  
	*
	* If we can't use crossover: 
	* Copy left and right parents to child 1 and 2, respectively.
	*
	* If we can use crossover:
	* Create a Two-point Crossover 
	* 1) Calculate crossover points A and B by randomizing a number between [0, maxbits/2 ]
	* 2) Create a bitmask with ones in the middle between crossover points and zeros on the outside 
	*	a) Start with a bitmask with all bits set to 1
	*	b) Shift left and then right by crossover point A amount to set zeros on the left side of bitmask 
	*	c) Shift right and then left by crossover point B amount to set zeros on the right side of bitmask
	* 3) Using the bitmask we can then copy Left Parent and Right Parent by bitwise & the parent gene against the bitmask.
	*    To flip the bitmask we use the bitwise NOT on the bitmask, allowing to copy only what is set to 1 on our bitmask
	* 4) Process the mutation of the new gene
	* 5) Set Child 1 and Child 2's gene, generation and fitness information
	* 6) add to population
	* 
	* @param left - Left parent
	* @param right - Right parent
	* @param child1 - child 1 to process crossover on
	* @param child2 - child 2 to process crossover on
	* @return void
	*/
	void processCrossover(Parent *left, Parent *right, Parent *child1, Parent *child2)
	{
		bool shouldCross = shouldCrossover();

		if (shouldCross)
		{
			int maxbits = maxParentBits();
			int halfmaxbits = maxbits / 2;

			int crossPointA = (Utils::random() % halfmaxbits);
			int crossPointB = (Utils::random() % halfmaxbits);

			unsigned int crossPointBitMask = Utils::MAXUINT();
			
			crossPointBitMask = (crossPointBitMask << crossPointA);
			crossPointBitMask = (crossPointBitMask >> crossPointA);
			crossPointBitMask = (crossPointBitMask >> crossPointB);
			crossPointBitMask = (crossPointBitMask << crossPointB);

			unsigned int gene1 = (left->gene() & ~crossPointBitMask) | (right->gene() & crossPointBitMask);
			unsigned int gene2 = (left->gene() & crossPointBitMask) | (right->gene() & ~crossPointBitMask);
	
			unsigned int geneFitness1 = processMutation(gene1);
			unsigned int geneFitness2 = processMutation(gene2);

			child1->setGene(gene1);
			child1->setGeneration(currentGeneration());
			child1->setFitness(geneFitness1);

			child2->setGene(gene2);
			child2->setGeneration(currentGeneration());
			child2->setFitness(geneFitness2);
		}
		else
		{
			child1->Copy(left);
			child2->Copy(right);
		}

		addToPopulation(child1, child2);
	}


	/**
	* Process Survivor Selection
	* All the children were recently added to the population in the m_ParentsMinSorted list. 
	* We need to shrink the sorted list back to the max population size, but we also need to maintain the master array of parents.
	* This means we need to check if the parents we remove are on the master array, and swap in one of the new children that was just created.
	* Special Case:
	* If all the population becomes indentical in fitness, it may be possible to remove the optimal fitness parent, this prevents that from happening.
	* @return void
	*/
	void processSurvivorSelection()
	{
		Parent *weakParent = NULL;	//the weak parent to remove from master array 
		Parent *newParent = NULL;	//the new parent to add to master array
		Parent *reAddParent = NULL;	//to prevent removing the optimal parent
		int id = 0;

		//loop until total population is at our maxPopulation
		while (m_ParentsMinSorted.size() > (unsigned int)maxPopulation())
		{
			//select the lowest fitness parent
			weakParent = m_ParentsMinSorted.top();

			//special case: 
			//avoid using the optimalFitParent as weakest parent when all our population becomes identical in fitness
			//we will add back to our min heap before loop restarts
			if (weakParent == optimalFitParent())
			{
				reAddParent = weakParent;
				m_ParentsMinSorted.pop();
				weakParent = m_ParentsMinSorted.top();
			}

			//pop the lowest fitness parent out of our min heap
			m_ParentsMinSorted.pop();

			//we need to transfer the weakParent's ID to the child that is replacing them in the population master array
			if (!weakParent->needsID())
			{
				id = weakParent->id();

				newParent = m_NewChildren.top();
				newParent->setID(id);

				//swap the new parent into master array
				m_Parents[id] = newParent;
			}

			//the newest child was processed, remove so we don't process the same child again
			m_NewChildren.pop();

			//special case: 
			//re-add the optimal parent if pulled from min heap when population has indentical fitness
			if (reAddParent != NULL)
			{
				m_ParentsMinSorted.push(reAddParent);
				reAddParent = NULL;
			}
			
			//instead of deleting our weakest parent, we can reuse them as a new child for next generation
			m_ReusableChildren.push(weakParent);
		}
	}


	/**
	* Process Mutation
	* Loop through each bit and mutate (flip) the bit if it is less than the mutation probability
	* @param gene - The gene of the parent
	* @return unsigned int - The final fitness of the mutated gene
	*/
	unsigned int processMutation(unsigned int &gene)
	{
		int bitCnt = 0;
		double randomNumber = 0;

		//loop for each bit
		for (int i = 0; i < maxParentBits(); i++)
		{
			//flip bit if less than mutation probability
			randomNumber = (double)Utils::random() / Utils::randomMax();
			if (randomNumber <= mutationProbability())
				gene ^= (1 << i);

			//check if bit is set, if it is, count it
			if ((gene & (1 << i)) > 0)
				bitCnt++;
		}
		return bitCnt;
	}


//-----------------------------------------------
private: //Class Helper Methods
//-----------------------------------------------

	/**
	* Add To Population
	* Simultaneously adds to min heap of parents, checks for optimal fitness, and adds to stack of new children
	* for both Child1 and Child2
	* @param child1 - The first child created
	* @param child2 - The second child created
	* @return void
	*/
	void addToPopulation(Parent *child1, Parent *child2)
	{
		m_ParentsMinSorted.push(child1);
		m_ParentsMinSorted.push(child2);

		checkIfOptimal(child1);
		checkIfOptimal(child2);

		m_NewChildren.push(child1);
		m_NewChildren.push(child2);
	}


	/**
	* Check If Optimal
	* Check parent against current optimal fitness and set as optimal parent if fitness is better
	* @param parent - The parent to compare against current optimal parent
	* @return boolean - If parent was optimal
	*/
	bool checkIfOptimal(Parent *parent)
	{
		if (parent->fitness() > m_OptimalParent->fitness())
		{
			m_OptimalParent = parent;
			return true;
		}
		return false;
	}


	/**
	* Check If Has Optimal
	* Checks if population has at least one parent at max ones
	* @return boolean - If optimal solution has been found
	*/
	bool checkIfHasOptimal()
	{
		if (m_OptimalParent == NULL)
			return false;
		if (m_OptimalParent->fitness() != maxParentBits())
			return false;
		return true;
	}


	/**
	* Should Crossover
	* Compare a random number against crossover probability
	* @return boolean - If there should be crossover
	*/
	bool shouldCrossover()
	{
		double randomNumber = (double)Utils::random() / Utils::randomMax();
		return randomNumber <= crossoverProbability();
	}


	/**
	* Get Run Count
	* Read the output data for line count to use as a run count
	* @return int - The number of runs saved in output file
	*/
	int getRunCount()
	{
		//open file
		ifstream inFile("scatterdata.csv");
		//read lines using '\n' as seperator
		int lineCount = count(istreambuf_iterator<char>(inFile), istreambuf_iterator<char>(), '\n');
		return lineCount;
	}


	/**
	* Save Run
	* Append the run # and optimal parent's generation # to our output data in a Comma-Seperated Values (.csv) format 
	* Output to console that we saved the run data
	* @return void
	*/
	void saveRun()
	{
		int lineCount = getRunCount();

		//open file and output line: lineCount,genotype,fitness
		ofstream myfile;
		myfile.open("scatterdata.csv", ios::out | ios::app);
		myfile << lineCount << "," << optimalFitParent()->generation() << "," << optimalFitParent()->fitness() << endl;

		//close file
		myfile.close();

		//output to console that we saved the runs
		cout << endl << "Saved Run #" << lineCount << " to scatterdata.csv." << endl;
	}
};


#endif //POPULATION_H