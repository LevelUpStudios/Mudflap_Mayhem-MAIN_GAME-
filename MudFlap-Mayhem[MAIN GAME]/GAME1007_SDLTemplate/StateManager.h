#pragma once
#ifndef _STATEMANAGER_H_
#define _STATEMANAGER_H_

#include "States.h"
#include <vector>
using namespace std;

class StateManager
{
private: // Public properties
	static vector<State*> s_states;
public: //Public methods
	static void Update();
	static void Render();
	static void PushState(State* pState); // Used when going to PauseState
	static void PopState(); // Used leaving PauseState
	static void	ChangeState(State* pState);
	static void	Quit(); // Used for cleanup
	static vector<State*>& GetStates(); // Returns a reference to our State* vector

private: // Private methods
	StateManager(){}
};

typedef StateManager STMA; // Creating a shorter alias

#endif
