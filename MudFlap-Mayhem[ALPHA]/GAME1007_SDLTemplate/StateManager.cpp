#include "StateManager.h"

void StateManager::Update() // Invokes Update() of the current state
{
	if (!s_states.empty())
		s_states.back()->Update(); // s_states.back() reps the current state by pointer
}

void StateManager::Render() // Invokes the Render() of the current state
{
	if (!s_states.empty())
		s_states.back()->Render();
}

void StateManager::PushState(State* pState) // Invoked going to PauseState from GameState
{
	s_states.push_back(pState);
	s_states.back()->Enter();
}

void StateManager::PopState() // Invoked going back to GamesState from PauseState
{
	
	if (s_states.size() <= 1) return; // If only one state in vector, return
	if(!s_states.empty())
	{
		s_states.back()->Exit();
		delete s_states.back();
		s_states.back() = nullptr;
		s_states.pop_back();
	}
	s_states.back()->Resume();
}

void StateManager::ChangeState(State* pState)
{
	if (!s_states.empty())
	{
		s_states.back()->Exit();
		delete s_states.back();
		s_states.back() = nullptr;
		s_states.pop_back();
	}
	pState->Enter();
	s_states.push_back(pState);
	/*or, this may be a little more elegant:
	s_states.push_back(pState);
	s_states.back()->Enter();*/
}

void StateManager::Quit()
{
	while (!s_states.empty())
	{
		s_states.back()->Exit();
		delete s_states.back();
		s_states.back() = nullptr;
		s_states.pop_back();
	}
}

vector<State*>& StateManager::GetStates() { return s_states; }
vector<State*> StateManager::s_states;