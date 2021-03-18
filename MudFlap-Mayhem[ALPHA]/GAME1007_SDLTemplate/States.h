#pragma once
#ifndef _STATES_H_
#define _STATES_H_
#include <vector>

#include "Engine.h"

class State // This is the abstract base class for all states
{
public:
	virtual void Enter() = 0; // = 0 means pure Virtual - must be defined in the subclass (forcing an overide)
	virtual void Update() = 0;
	virtual void Render();
	virtual void Exit() = 0;
	virtual void Resume();
	virtual ~State() = default; // Modern alternative to {}

	SDL_Texture* m_pBGTexture, * m_pPlayerTexture, * m_pEnemyTexture, *m_pLaserTexture, *m_eLaserTexture, * m_wallTexture,*m_healthBarTexture;
	Sprite m_bg1, m_wall1, m_wall2, m_wall3, m_wall4, m_corner1, m_corner2, m_corner3, m_corner4,m_healthFull,m_health2,m_health3,m_health4,m_healthDead;
	Player m_player;
	int m_speed = 7;

	vector<Enemy*> m_enemy;
	vector<Bullet*> m_pBullet;
	vector<EnemyBullet*>m_pEnemyBullet;

	// Adding the sound variables
	Mix_Chunk* m_blueLaser;
	Mix_Chunk* m_redLaser;
	Mix_Chunk* m_explode;
	// Adding background music variables
	Mix_Music* m_bgMusic;

protected: // Private but inherited
	State() {} // Prevents instantiation from outside the class
};

class PauseState : public State
{
public:
	PauseState();
	virtual void Enter();
	virtual void Update();
	virtual void Render();
	virtual void Exit();
	
};

class TitleState : public State
{
public:
	TitleState();
	virtual void Enter();
	virtual void Update();
	virtual void Render();
	virtual void Exit();

};

class GameState : public State
{
public:
	GameState();
	virtual void Enter();
	virtual void Update();
	virtual void Render();
	virtual void Exit();
	virtual void Resume();
	void HealthCheck();
};

class LoseState : public State
{
public:
	LoseState();
	virtual void Enter();
	virtual void Update();
	virtual void Render();
	virtual void Exit();


};

class WinState :public State
{
public:
	WinState();
	virtual void Enter();
	virtual void Update();
	virtual void Render();
	virtual void Exit();
};

#endif
