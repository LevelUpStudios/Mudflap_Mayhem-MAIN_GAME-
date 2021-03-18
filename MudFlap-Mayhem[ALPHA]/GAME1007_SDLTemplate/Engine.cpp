#include "Engine.h"
#include <ctime>
#include <iostream>
#include "States.h"
#include "StateManager.h"
using namespace std;


int Engine::Init(const char* title, int xPos, int yPos, int width, int height, int flags)
{
	srand((unsigned)time(NULL));
	cout << "Initializing engine..." << endl;
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) // If initialization OK
	{
		// Create the SDL window...
		m_pWindow = SDL_CreateWindow(title, xPos, yPos, width, height, flags);
		if (m_pWindow != nullptr)
		{
			// Create the SDL renderer...(back buffer)
			m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, NULL);
			if (m_pRenderer != nullptr)
			{
				// Initialize subsystems...
				if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) != 0)
				{
					
				/*	m_enemyTexture = IMG_LoadTexture(m_pRenderer, "img/enemy_ship.png");
					m_pBGTexture = IMG_LoadTexture(m_pRenderer, "img/mud_background.png ");
					m_pTexture = IMG_LoadTexture(m_pRenderer, "img/semi_truck.png");
					m_pLaserTexture = IMG_LoadTexture(m_pRenderer, "img/blue_laser.png");
					m_eLaserTexture = IMG_LoadTexture(m_pRenderer, "img/red_laser.png");
					m_pHealthTexture = IMG_LoadTexture(m_pRenderer, "img/wall.png");*/
				}
				else return false; // Image init failed.
				if (Mix_Init(MIX_INIT_MP3) != 0)
				{
					// Config mixer.
					Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 2048);
					Mix_AllocateChannels(16);
				}
				else return false; // Mixer init failed.
			}
			else return false; // Renderer creation failed.
		}
		else return false; // Window creation failed.
	}
	else return false; // Init failed.
	m_fps = (Uint32)round(1.0 / (double)FPS * 1000); // Converts FPS into milliseconds, ex (16.67)
	m_keystates = SDL_GetKeyboardState(nullptr);

	STMA::PushState(new TitleState());
		
	cout << "Initialization successful!" << endl;
	m_running = true;
	return true;
}

void Engine::Wake()
{
	m_start = SDL_GetTicks();
}

void Engine::HandleEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			m_running = false;
			break;
		case SDL_KEYUP:
			if (event.key.keysym.sym == ' ')
			{
				m_bCanShoot = true;
			}
		}
	}
}

bool Engine::KeyDown(SDL_Scancode c)
{
	if (m_keystates != nullptr)
	{
		if (m_keystates[c] == 1)
			return true;
	}
	return false;
}

void Engine::Update()
{
	STMA::Update();
}

void Engine::Render()
{
	STMA::Render();
}

void Engine::Sleep()
{
	m_end = SDL_GetTicks();
	m_delta = m_end - m_start; // 1055 - 1050 = 5ms
	if (m_delta < m_fps)
		SDL_Delay(m_fps - m_delta);
}

int Engine::Run()
{
	if (m_running == true)
	{
		return 1;
	}
	// Start and run the "engine"
	if (Init("MUDFLAP MAYHEM", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, NULL) == false)
	{
		return 2;
	}
	// Passes initial checks, start loop
	while (m_running == true)
	{
		Wake();
		HandleEvents(); // Input
		Update();       // Processing
		Render();       // Output
		if (m_running == true)
			Sleep();
	}
	Clean();
	return 0;
}

Engine& Engine::Instance()
{
	static Engine instance; // Magic statics. Creating the object.
	return instance;
}

void Engine::Clean()
{
	//for (unsigned i = 0; i < m_pbullet.size(); i++) // Cleanup leftover player bullets(blue laser)
	//{
	//	delete m_pbullet[i]; // Flag for re-allocation 'for sale'
	//	m_pbullet[i] = nullptr; // Wrangle your dangle
	//}
	//m_pbullet.clear(); // Clears the vector
	//m_pbullet.shrink_to_fit(); // Reduces capacity to size
	//for (unsigned i = 0; i < m_ebullet.size(); i++) // Cleanup levtover enemy bullets(red laser)
	//{
	//	delete m_ebullet[i]; // Flag for re-allocation 'for sale'
	//	m_ebullet[i] = nullptr; // Wrangle your dangle
	//}
	//m_ebullet.clear(); // Clears the vector
	//m_ebullet.shrink_to_fit(); // Reduces capacity to size
	//for (unsigned i = 0; i < m_enemy.size(); i++) // Cleanup leftover enemies
	//{
	//	delete m_enemy[i]; // Flag for re-allocation 'for sale'
	//	m_enemy[i] = nullptr; // Wrangle your dangle
	//}
	//m_enemy.clear(); // Clears the vector
	//m_enemy.shrink_to_fit(); // Reduces capacity to size
	//
	SDL_DestroyRenderer(m_pRenderer);
	SDL_DestroyWindow(m_pWindow);
	//SDL_DestroyTexture(m_pTexture);
	//SDL_DestroyTexture(m_pLaserTexture);
	//SDL_DestroyTexture(m_pBGTexture);
	
	//Mix_FreeChunk(m_redLaser);
	//Mix_FreeChunk(m_blueLaser);
	//Mix_FreeMusic(m_bgMusic);

	Mix_CloseAudio();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}

