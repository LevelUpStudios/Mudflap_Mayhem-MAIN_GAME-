#pragma once
#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <iostream>
#include <vector>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"


#define FPS 60
#define WIDTH 1024
#define HEIGHT 768
using namespace std;

class Sprite
{
protected:
	SDL_Rect m_src; // Source 
	SDL_Rect m_dst; // Destination rectangle.
	 

public:
	void SetRekts(const SDL_Rect s, const SDL_Rect d) // Non-Default constructor
	{
		m_src = s;
		m_dst = d;
	}
	SDL_Rect* GetSrc() { return &m_src; }
	SDL_Rect* GetDst() { return &m_dst; }

};

class Player : public Sprite
{
private:
	int m_playerAngle;
	int m_playerHealth = 4;
public:
	Player()
	{
		m_playerAngle = 0;
	}
	~Player()
		= default;
	void SetPlayerAngle(int a)
	{
		m_playerAngle = a;
	}
	int GetPlayerAngle()
	{
		return m_playerAngle;
	}
	void SetPlayerHealth(int h)
	{
		m_playerHealth = h;
	}
	int GetPlayerHealth()
	{
		return m_playerHealth;
	}
};

class Wall : public Sprite
{

	
};

class Bullet : public Sprite
{
private:
	SDL_Rect m_rect;
	int m_bulletAngle;
public:
	
	Bullet(SDL_Point spawnLoc= {486, 560},int angle = 0) // Non default constructor
	{
		cout << "Constructing Player Bullet" << endl;
		this->m_rect.x = spawnLoc.x;
		this->m_rect.y = spawnLoc.y;
		this->m_rect.w = 73;
		this->m_rect.h = 126;
		m_bulletAngle = angle;
	}
	~Bullet() // Destructor
	{
		cout << "Deallocating player bullet..." << endl;
	}
	void SetBulletAngle(int angle)
	{
		m_bulletAngle = angle;
	}
	int GetBulletAngle()
	{
		return m_bulletAngle;
	}
	void SetLoc(SDL_Point loc)
	{
		m_rect.x = loc.x;
		m_rect.y = loc.y;
	}
	void Update()
	{
		if (m_bulletAngle == 0) // Bullets going Up
			m_rect.y -= 20;
		if (m_bulletAngle == 90) // Bullets going Right
			m_rect.x += 20;
		if (m_bulletAngle == 180) // Bullets going
			m_rect.y += 20;
		if (m_bulletAngle == 270) // Bullets going
			m_rect.x -= 20;
	}
	SDL_Rect* GetDst() { return &m_rect; }
	  
};

class EnemyBullet : public Sprite
{
private:
	SDL_Rect m_rect;

public:
	EnemyBullet(SDL_Point spawnLoc = { 486, 560 }) // Non default constructor
	{
		cout << "Constructing Enemy Bullet" << endl;
		this->m_rect.x = spawnLoc.x;
		this->m_rect.y = spawnLoc.y;
		this->m_rect.w = 36;
		this->m_rect.h = 64;
	}
	~EnemyBullet() // Destructor
	{
		cout << "De-allocating Enemy Bullet..." << endl;
	}
	void SetLoc(SDL_Point loc)
	{
		m_rect.x = loc.x;
		m_rect.y = loc.y;
	}

	void Update()
	{
		this->m_rect.y += 8; // Moves bullet "down" 10 pixels every frame
	}
	SDL_Rect* GetDst() { return &m_rect; }
};

class Enemy : public Sprite
{
private:
	SDL_Rect m_rect;
public:
	Enemy(SDL_Point spawnLoc = {472,100}) // Non default constructor
	{
		cout << "Spawning Enemy..." << endl;
		this->m_rect.x = spawnLoc.x;
		this->m_rect.y = spawnLoc.y;
		this->m_rect.w = 80;
		this->m_rect.h = 40;
	}
	~Enemy() // Destructor
	{
		cout << "De-allocating Enemy..." << endl;
	}
	void SetLoc(SDL_Point loc)
	{
		m_rect.x = loc.x;
		m_rect.y = loc.y;
	}
	void Update()
	{
		this->m_rect.y += 1; // Moves Enemy Ship "down" 1 pixels every frame
	}
	SDL_Rect* GetDst() { return &m_rect; }
};

class Engine
{
private: // private properties.
	bool m_running = false;
	
	Uint32 m_start, m_end, m_delta, m_fps;
	const Uint8* m_keystates;
	SDL_Window* m_pWindow;
	SDL_Renderer* m_pRenderer;


private: // private method prototypes.
	Engine(){} //Prevents instantiation outside the class
	int Init(const char* title, int xPos, int yPos, int width, int height, int flags);
	void Clean();
	void Wake();
	void HandleEvents();
	// Moved Keydown from here //
	void Update();
	void Render();
	void Sleep();

public: // public method prototypes.
	int Run();
	//static Engine* Instance(); // Pointer way.
	static Engine& Instance(); // Static method for object access.
	SDL_Renderer* GetRenderer() { return m_pRenderer; }
	bool KeyDown(SDL_Scancode c);
	bool m_bCanShoot = false;
};

#endif

