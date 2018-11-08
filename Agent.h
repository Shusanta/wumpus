// Agent.h

#ifndef AGENT_H
#define AGENT_H
using namespace std;
#include "Action.h"
#include "Percept.h"

#include <unordered_set>

#include "Location.h"
#include "Orientation.h"
#include "Search.h"
#include <list>
#include <set>

class Agent
{
public:
	Agent ();
	~Agent ();
	void Initialize ();
	Action Process (Percept& percept);
	void GameOver (int score);
	void UpdateState (Percept& percept);
	int currentCounter; // to see if the pit killed the agent or not
	Action lastAction;
	Percept lastPercept;
	void FindPathToLocation (Location& goalLocation);
	WorldState currentState;
	bool agentHasGold;
	void ExecuteTurnLeft();
	void ExecuteTurnRight();
	void ExecuteForward();
	void addSafeLocations(int x);
	vector<Location> GenerateFrontier(Location location);
	list<Action> actionList;
	SearchEngine searchEngine;
	Location query;
	double CalculateProbabilityTrue(Location location, vector<Location> relativeFrontier);
	double CalculateProbabilityFalse(Location location, vector<Location> relativeFrontier);
	vector<Location> Known;
	vector<Location> adjBreeze;
	vector<Location> breeze;
	vector<Location> Frontier;
    double CalculateProbability(Location location);
	bool isBreeze(int x, int y);
	bool ProbabilityOverThreshold();
	bool SameBreezeAdjancent(Location location1, Location location2);
	bool SameBreezeAdjacent(Location location1, Location location2);
	
};

#endif // AGENT_H
