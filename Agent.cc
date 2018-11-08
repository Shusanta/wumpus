// Agent.cc

#include <iostream>
#include <list>
#include "Agent.h"

using namespace std;

double PIT_PROBABILITY = .2;

Agent::Agent ()
{
	for (int i = 1; i <= 4; i++) {
		for (int j = 1; j <= 4; j++) {
			searchEngine.AddSafeLocation(i,j);
		}
	} 
	 //assuming every place is safe. there can only be so many pits/wumpuses on a 4x4 board

	currentState.wumpusLocation = Location(0,0); //initial set
	currentState.goldLocation = Location(0,0);
	currentCounter = 0;
}

Agent::~Agent ()
{

}

void Agent::Initialize ()
{
	if (lastAction == GOFORWARD) {
		ExecuteForward();
		int x = currentState.agentLocation.X; 
		int y = currentState.agentLocation.Y;
		
		if(lastPercept.Stench && currentState.wumpusLocation == Location(0,0)) {
			currentState.wumpusLocation = currentState.agentLocation;
			cout << "Wumpus at (" << currentState.wumpusLocation.X << "," << currentState.wumpusLocation.Y << ")" << endl;
			searchEngine.RemoveSafeLocation(x, y); // remove the wumpus
		}
		else if(lastPercept.Breeze && currentCounter<= 999) { //make sure a pit killed it and remove it
			currentState.pitLocations.push_back(currentState.agentLocation);
			cout << "Pit at (" << currentState.agentLocation.X << "," << currentState.agentLocation.Y << ")" << endl;
			searchEngine.RemoveSafeLocation(x, y);
		}
		
		cout << "CURRENT COUNTER" << currentCounter <<endl;

		currentCounter =0;
	}

	if(!(currentState.wumpusLocation == Location(0,0))){
		int xw = currentState.wumpusLocation.X; // removing wumpus if not removed. // to make sure.
		int yw = currentState.wumpusLocation.Y;
		searchEngine.RemoveSafeLocation(xw,yw);
		cout<< "REMOVING" << xw << " AND " << yw <<endl;
	}
	
	currentState.agentLocation = Location(1,1);
	currentState.agentOrientation = RIGHT;
	currentState.agentAlive = true;
	currentState.agentHasArrow = true;
	currentState.agentHasGold = false;
	currentState.agentInCave = true;
	currentState.wumpusAlive = true;
	lastAction = CLIMB; 
	
}

Action Agent::Process (Percept& percept)
{
	Action action;
	list<Action> actionList2;

	UpdateState(percept);

	

	if (percept.Glitter) { // make sure to reset action list if glitter foind
			currentState.goldLocation = currentState.agentLocation;
			cout << "Gold at Location (" << currentState.goldLocation.X << "," << currentState.goldLocation.Y << ")" << endl;
			actionList.push_back(GRAB);
			action = actionList.front();
			actionList.pop_front();
			lastAction = action;
			lastPercept = percept;
			actionList.clear();
			currentCounter++;
		return action;
			
		}

	if(actionList.empty()){

		if(!currentState.agentHasGold && !(currentState.goldLocation == Location(0,0))) {			
			// grab the gold if known
			cout << "Gold AT (" << currentState.goldLocation.X << "," << currentState.goldLocation.Y << ")" << endl;
			actionList2 = searchEngine.FindPath(currentState.agentLocation, currentState.agentOrientation, currentState.goldLocation, RIGHT);
			actionList.splice(actionList.end(), actionList2);
		}
		else if(currentState.agentHasGold && !(currentState.agentLocation == Location(1,1))) {
			// =(1,1) and climb
			cout << "I have achieved the gold, I am going  to (1,1)" << endl;
			actionList2 = searchEngine.FindPath(currentState.agentLocation, currentState.agentOrientation, Location(1,1), LEFT);
			actionList.splice(actionList.end(), actionList2);
		} else if (currentState.agentHasGold && (currentState.agentLocation == Location(1,1))) { // Rule 4b
			actionList.push_back(CLIMB);
		} else if (percept.Stench && currentState.agentHasArrow) { // shoot
			actionList.push_back(SHOOT);
		} else if (percept.Bump) { // change direction
			actionList.push_back(TURNLEFT);
			actionList.push_back(GOFORWARD);
		} 
	}
	
	srand(time(NULL));
	while(actionList.empty()) { //choose a random spot on the map. usually works but on rare occasion it picks the spot where the gold isn't multiple times. 
		int xa = rand()% 4 +1;
		int ya = rand()% 4 +1;

		actionList2 = searchEngine.FindPath(currentState.agentLocation, currentState.agentOrientation, Location(xa,ya), RIGHT);
		actionList.splice(actionList.end(), actionList2);
			
	}
	    action = actionList.front();
		actionList.pop_front();
		if((action == GOFORWARD) && ProbabilityOverThreshold()){
			action = TURNLEFT;
			actionList.empty();
		}
		lastAction = action;
		lastPercept = percept;
		currentCounter++;

		return action;
}


void Agent::UpdateState(Percept& percept){ // update state and act accordingly
	if(percept.Breeze){
		breeze.push_back(currentState.agentLocation);
	}
	switch( lastAction){
		case GOFORWARD:
			if (! percept.Bump){
				ExecuteForward();
			}
			break;
		case TURNLEFT:
			ExecuteTurnLeft();
			break;
		case TURNRIGHT:
			ExecuteTurnRight();
			break;
		case GRAB:
			currentState.agentHasGold = true;
			break;
		case SHOOT:
			currentState.agentHasArrow = false;
			if(percept.Scream) {
				ExecuteForward();
				currentState.wumpusLocation = currentState.agentLocation; // known that the wumpus was killed
			}
			break;
		case CLIMB:
			break;
	}
}

void Agent::GameOver (int score)
{

}

void Agent::ExecuteTurnLeft ()
{
	if (currentState.agentOrientation == UP)
		currentState.agentOrientation = LEFT;
	else if (currentState.agentOrientation == DOWN)
		currentState.agentOrientation = RIGHT;
	else if (currentState.agentOrientation == LEFT)
		currentState.agentOrientation = DOWN;
	else currentState.agentOrientation = UP;
}

void Agent::ExecuteTurnRight ()
{
	if (currentState.agentOrientation == UP)
		currentState.agentOrientation = RIGHT;
	else if (currentState.agentOrientation == DOWN)
		currentState.agentOrientation = LEFT;
	else if (currentState.agentOrientation == LEFT)
		currentState.agentOrientation = UP;
	else currentState.agentOrientation = DOWN;
}

void Agent::ExecuteForward () 
{
	if (currentState.agentOrientation == UP)
			currentState.agentLocation.Y++;
	if (currentState.agentOrientation == DOWN)
			currentState.agentLocation.Y--;
	if (currentState.agentOrientation == RIGHT)
			currentState.agentLocation.X++;
	if (currentState.agentOrientation == LEFT)
			currentState.agentLocation.X--;
}
		
vector<Location> Agent::GenerateFrontier(Location location)
{
	int x = location.X;
	int y = location.Y;

	adjBreeze.clear();
	
	vector<Location> frontier;

	vector<Location>::iterator itr;

	if ((x+1 <= 4) && isBreeze(x+1,y)){
		adjBreeze.push_back(Location(x+1,y));
	}
	if ((x-1 <= 0) && isBreeze(x-1,y)){
		adjBreeze.push_back(Location(x-1,y));
	}
	if ((y+1 <= 4) && isBreeze(x,y+1)){
		adjBreeze.push_back(Location(x,y+1));
	}
	if ((y-1 <= 4) && isBreeze(x,y-1)){
		adjBreeze.push_back(Location(x,y-1));
	}

	for (itr = adjBreeze.begin(); itr != adjBreeze.end(); itr++) {

		x = itr->X;
		y = itr->Y;


		if ((x+1<=4) && !searchEngine.SafeLocation(x+1, y) && !(Location(x+1, y)==query)) 
		{
			frontier.push_back(Location(x+1,y));
		}
		if ((x-1>0) && !searchEngine.SafeLocation(x-1, y) && !(Location(x-1, y)==query)) 
		{
			frontier.push_back(Location(x-1,y));
		}
		if ((y+1<=4) && !searchEngine.SafeLocation(x, y+1) && !(Location(x, y+1)==query)) 
		{
			frontier.push_back(Location(x,y+1));
		}
		if ((y-1>0) && !searchEngine.SafeLocation(x, y-1)&& !(Location(x, y-1)==query)) 
		{
			frontier.push_back(Location(x,y-1));
		}
	}

	return frontier;
}

double Agent::CalculateProbability(Location location)
{
	vector<Location> relativeFrontier = GenerateFrontier(location);

	if(relativeFrontier.size() == 0)
		return 0;
	
	double alpha;

	double isTrue = PIT_PROBABILITY * (CalculateProbabilityTrue(location, relativeFrontier));
	double isFalse = (1 - PIT_PROBABILITY) * (CalculateProbabilityFalse(location, relativeFrontier));
	double total = isTrue + isFalse;
	alpha = (double) 1.0 / (double)total;
	cout << " FOR LOCATION " << location.X << " " << location.Y << "  it is "<< alpha * isTrue << "\n" << " ALPHA " << alpha << "\n" << isTrue << isFalse << "\n";

	return double(alpha * isTrue);
}
		
double Agent::CalculateProbabilityTrue(Location location, vector<Location> relativeFrontier)
{

	double prob = 0;
	//if only one relative frontier location, return 1
	//b/c there are only two possible combinations w/ destination being true 
	// (1-PIT_PROBABILITY) + PIT_PROBABILITY = 1
	if(relativeFrontier.size()==1)
	{
		return 1;
	}

	if(relativeFrontier.size()==2)
	{
		//three possible combinations if the two relative frontier locations
		
		prob = (PIT_PROBABILITY * PIT_PROBABILITY);
		prob += PIT_PROBABILITY * (1-PIT_PROBABILITY);
		prob += (1-PIT_PROBABILITY) * PIT_PROBABILITY;		
	}

	//three relative frontire locations, two locations must share breeze
	if(relativeFrontier.size()==3)
	{

		//solo location has pit, w/ permutations of shared breeze locations
		prob = (PIT_PROBABILITY)*(PIT_PROBABILITY)*(PIT_PROBABILITY);
		prob += (PIT_PROBABILITY)*(PIT_PROBABILITY)*(1-PIT_PROBABILITY);
		prob += (PIT_PROBABILITY)*(1-PIT_PROBABILITY)*(PIT_PROBABILITY);
		prob += (PIT_PROBABILITY)*(1-PIT_PROBABILITY)*(1-PIT_PROBABILITY);

		//solo location doesn't have pit, w/ permutations of shared breeze locations
		prob += (1-PIT_PROBABILITY)*(PIT_PROBABILITY)*(PIT_PROBABILITY);
		prob += (1-PIT_PROBABILITY)*(PIT_PROBABILITY)*(1-PIT_PROBABILITY);
		prob += (1-PIT_PROBABILITY)*(1-PIT_PROBABILITY)*(PIT_PROBABILITY);

		//None have pits
		prob += (1-PIT_PROBABILITY)*(1-PIT_PROBABILITY)*(1-PIT_PROBABILITY);
	}
	return prob;
}

double Agent::CalculateProbabilityFalse(Location location, vector<Location> relativeFrontier)
{
	double prob = 0;


	//if there's only one relative frontier location, only one combination possible where destination isn't pit
	if(relativeFrontier.size()==1)
	{
		return (1 - PIT_PROBABILITY);
	}

	if(relativeFrontier.size()==2)
	{
		//three possible combinations if the two relative frontier locations
		if(SameBreezeAdjancent(relativeFrontier[0], relativeFrontier[1]))
		{
			prob = ((1-PIT_PROBABILITY )* PIT_PROBABILITY);
		}

		else
		{
			prob = PIT_PROBABILITY * PIT_PROBABILITY;
			prob += (1-PIT_PROBABILITY) * PIT_PROBABILITY;
		}

		
	}

	if(relativeFrontier.size() == 3)
	{
		prob = PIT_PROBABILITY * PIT_PROBABILITY * PIT_PROBABILITY;
		prob += PIT_PROBABILITY * (1-PIT_PROBABILITY) * PIT_PROBABILITY;
		prob += (1-PIT_PROBABILITY) * PIT_PROBABILITY * PIT_PROBABILITY;
	}

	return .8 *prob;

}

bool Agent::isBreeze(int x, int y)
{
	Location location = Location(x,y);
	vector<Location>::iterator itr;

	for (itr = breeze.begin(); itr != breeze.end(); itr++) {
		if (*itr == location) {
			return true;
		}
	}

	return false;
	
}


bool Agent::ProbabilityOverThreshold()
{
	int x = currentState.agentLocation.X;
	int y = currentState.agentLocation.Y;
	double prob = 0.0;
	Orientation orientation = currentState.agentOrientation;

	if (orientation == RIGHT) {
		x++;
	}
	if (orientation == UP) {
		y++;
	}
	if (orientation == LEFT) {
		x--;
	}
	if (orientation == DOWN) {
		y--;
	}

	if(y<1 || x < 1 || y>=4 ||x >= 4){
		return true;
	}
	query = Location(x,y);

	prob = CalculateProbability(Location(x,y));

	
	if(prob > .5){
		return true;
	}

	return false;

}

bool Agent::SameBreezeAdjancent(Location location1, Location location2){
	vector<Location>::iterator itr;
	for (itr = breeze.begin(); itr != breeze.end(); itr++) 
	{
		if((*itr  == Location(location1.X+1,location1.Y) || *itr  == Location(location1.X,location1.Y+1)) && (*itr  == Location(location1.X,location1.Y+1) ||  *itr  == Location(location1.X,location1.Y-1)))
		{
			return true;
		}

	}
	return false;
}