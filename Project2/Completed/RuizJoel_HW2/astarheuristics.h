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

#ifndef ASTARHEURISTICS_H
#define ASTARHEURISTICS_H

#include "astar.h"
#include "parser.h"
#include <cmath>

//view defintion below for details
float compoundInterest(float milesPerDay, float qualityMultiplier, float riskMultiplier, AStarNode *node, float(*easeQuality)(float, float, float, float), float(*easeRisk)(float, float, float, float));


//---------------------------------------------------------------------
/**
* class AStarHeuristicFirst
* No fancy formula, just add the path distance to g(x), providing absolute shortest path.
*/
class AStarHeuristicFirst {
public:
	const char* desc() { return "Searches for cheapest distance only."; }
	
	float g(AStarNode *node)	{
		return (node->prev->g + (float)node->path->distance);
	}

	float h(AStarNode *node)	{
		return ((float)node->city->goaldistance);
	}
};
//---------------------------------------------------------------------
/**
* class AStarHeuristicSecond
* Using the compound interest function, we weight the QUALITY much higher than quality.
* The risk easing gathers percentage closer to 0, and the quality gathers percentage closer to 1.
* Miles per day is exajurated in order for the A* algorithm to process faster.  The results are the same if we reduce.
*/
class AStarHeuristicSecond {
public:
	const char* desc() { return "Searches for safest route, with road quality as priority."; }
	
	float g(AStarNode *node)	{

		const float milesPerDay = 500;
		const float qualityMultiplier = 1.5f;
		const float riskMultiplier = 0.001f;

		return node->prev->g + compoundInterest(milesPerDay, qualityMultiplier, riskMultiplier, node, &Utils::easeOutQuad, &Utils::easeInQuad);
	}

	float h(AStarNode *node)	{
		return (node->city->goaldistance);
	}
};
//---------------------------------------------------------------------
/**
* class AStarHueristicThird
* Using the compound interest function, we weight the RISK much higher than quality.  
* The quality easing gathers percentage closer to 0, and the risk gathers percentage closer to 1.
* Miles per day is exajurated in order for the A* algorithm to process faster.  The results are the same if we reduce.
*/
class AStarHeuristicThird {
public:
	const char* desc() { return "Searches for safest route, with risk level as priority."; }
	
	float g(AStarNode *node)	{
		
		const float milesPerDay = 500;
		const float qualityMultiplier = 0.001f;
		const float riskMultiplier = 1.5f;

		return node->prev->g + compoundInterest(milesPerDay, qualityMultiplier, riskMultiplier, node, &Utils::easeInQuad, &Utils::easeOutQuad);
	}

	float h(AStarNode *node)	{
		return (node->city->goaldistance);
	}
};
//---------------------------------------------------------------------



/**
* Compound Interest
* This formula attempts to compute the dangers of traveling across X miles, in intervals of Y miles per day.
* Road Quality and Risk Levels are converted to percentages between [0,1].  
* The percentages are linear, so we allow transformation using an easing function to make them exponential.  
* The percentage is then used as an interest rate and compounded based on the amount of days the trip will take.
* The worse a quality or the worse a risk level, the closer it approaches a 100% interest rate.
* @param float milesPerDay - used to compute travel time in days
* @param float qualityMultiplier - this allows us to scale quality % after easing
* @param float riskMulitiplier - this allows us to scale risk % after easing
* @param AStarNode *node - the node we will be compounding
* @param float(*easing)(float,float,float,float) - the quality easing function, look at utils.h for samples.
* @param float(*easing)(float,float,float,float) - the risk easing function, look at utils.h for samples.
* @return float - the total distance after being compounded by quality and risk
*/
float compoundInterest(
	float milesPerDay,
	float qualityMultiplier, 
	float riskMultiplier, 
	AStarNode *node, 
	float(*easeQuality)(float, float, float, float), 
	float(*easeRisk)(float, float, float, float)
	)
{
	const float maxRoadQuality = 10;
	const float maxRiskLevel = 10;

	float total = (float)node->path->distance;
	float distance = total;
	float roadQuality = (float)node->path->roadQuality / maxRoadQuality;
	float riskLevel = (float)node->path->riskLevel / maxRiskLevel;
	float travelTimeInDays = distance / milesPerDay;

	//quality interest rate
	//inverse so that we can weight bad quality much higher
	//easeing function will transform our linear road quality into an exponential form
	roadQuality = 1.0f - roadQuality;
	float qualityPct = easeQuality(roadQuality, 0.0f, 1.0f, 1.0f) * qualityMultiplier;

	//risk interest rate 
	//easeing function will transform linear risk level into an exponential form
	float riskPct = easeRisk(riskLevel, 0.0f, 1.0f, 1.0f) * riskMultiplier;

	//compound our interest rates
	float compoundQuality = (1.0f + (qualityPct / travelTimeInDays));
	compoundQuality = pow(compoundQuality, travelTimeInDays);
	float compoundRisk = (1.0f + (riskPct / travelTimeInDays));
	compoundRisk = pow(compoundRisk, travelTimeInDays);

	//multiply the quality and risk interests to total
	total = (distance * compoundQuality * compoundRisk);

	return total;
}


#endif //ASTARHEURISTICS_H