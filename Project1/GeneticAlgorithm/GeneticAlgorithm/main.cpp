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

#include <stdio.h>
#include <stdlib.h>
#include "population.h"


int main()
{
	//Setup default settings
	int showOutput = 1;
	int maxPopulation = 75;
	int maxParentBits = 32;
	int maxPoolCount = 16;
	float parentBitProbability = 0.5f;
	float crossoverProbability = 0.7f;
	float mutationProbability = 0.025f;
	int maxGenerations = 200;


	cout << endl << "Welcome to Max Ones's Testing!" << endl << endl;
	//------------------------------------------------------
	//Undo block comment to manually input variables in terminal
	/*
	cout << "Enter maximum population count: ";
	cin >> maxPopulation;
	cout << endl;
	//cout << "Enter Mating Pool Size: ";
	//cin >> maxPoolCount;
	//cout << endl;
	//cout << "Enter Geneotype Bit Probability [0...1]: ";
	//cin >> parentBitProbability;
	//cout << endl;
	cout << "Enter Crossover Probability [0...1]: ";
	cin >> crossoverProbability;
	cout << endl;
	cout << "Enter Mutation Probability [0...1]: ";
	cin >> mutationProbability;
	cout << endl;
	cout << "Enter Maximum Generations: ";
	cin >> maxGenerations;
	cout << "Ready to begin... Show output? (0=no | 1=yes): ";
	cin >> showOutput;
	*/
	//------------------------------------------------------

	//enable/disable debug mode
	Utils::setDebug(showOutput == 0 ? false : true);


	//Create and setup population with our settings
	Population *population = new Population();
	{
		population->setMaxPopulation(maxPopulation);
		population->setMaxParentBits(maxParentBits);
		population->setMaxPoolCount(maxPoolCount);
		population->setParentBitProbability(parentBitProbability);
		population->setCrossoverProbability(crossoverProbability);
		population->setMutationProbability(mutationProbability);
	}


	//Profile the execution time of our genetic algorithm
	Utils::profileStart();
	{
		//Generate Initial Population and begin running through generations
		population->GenerateParents();
		population->RunGenerations(maxGenerations);
	}
	Utils::profileEnd();
	

	//cleanup allocated population object
	delete population;


	return 0;
}