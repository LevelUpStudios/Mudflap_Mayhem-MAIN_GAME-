#include "Engine.h"
#include <ctime>
#include <iostream>
using namespace std;

int frameCount = 0;

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
					
					m_enemyTexture = IMG_LoadTexture(m_pRenderer, "img/enemy_ship.png");
					m_pBGTexture = IMG_LoadTexture(m_pRenderer, "img/mud_background.png ");
					m_pTexture = IMG_LoadTexture(m_pRenderer, "img/semi_truck.png");
					m_pLaserTexture = IMG_LoadTexture(m_pRenderer, "img/blue_laser.png");
					m_eLaserTexture = IMG_LoadTexture(m_pRenderer, "img/red_laser.png");
				}
				else return false; // Image init failed.
				if (Mix_Init(MIX_INIT_MP3) != 0)
				{
					// Config mixer.
					Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 2048);
					Mix_AllocateChannels(16);
					
					// Load sounds.
					m_blueLaser = Mix_LoadWAV("aud/blue_laser_sound.wav");
					m_redLaser = Mix_LoadWAV("aud/red_laser_sound.wav");
					m_explode = Mix_LoadWAV("aud/collision_explode_sound.wav");
					// Load music.
					
					m_bgMusic = Mix_LoadMUS("aud/moonlight_sonata.mp3");
			
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
	
	// Player initialization
	m_player.SetRekts (  {0,0,162,130}, {431,638,60,100} ); // Player First {} is source rect, second {} destination rect
	
	// Backgrounds textures initialization
	m_bg1.SetRekts ( { 0,0,WIDTH,HEIGHT}, { 0,0,WIDTH,HEIGHT });

		
	cout << "Initialization successful!" << endl;
	Mix_PlayMusic(m_bgMusic, -1);
	Mix_VolumeMusic(5);
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
				// Spawn a new player bullet
				m_pbullet.push_back (new Bullet({ m_player.GetDst()->x +44, m_player.GetDst()->y-20},m_player.GetPlayerAngle()));
				m_pbullet.shrink_to_fit();
				
				//Mix_PlayChannel(-1, m_blueLaser, 0); 
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
	

	// Player movement
	if (KeyDown(SDL_SCANCODE_W) && m_player.GetDst()->y > 0)
	{
		m_player.GetDst()->y -= m_speed;
		m_player.SetPlayerAngle(0);
		cout << m_player.GetPlayerAngle() << "\n\n\n" << endl;
	}
	else if (KeyDown(SDL_SCANCODE_S) && m_player.GetDst()->y < HEIGHT - m_player.GetDst()->h)
	{
		m_player.GetDst()->y += m_speed;
		m_player.SetPlayerAngle(180);
		cout << m_player.GetPlayerAngle()<< "\n\n\n" << endl;
	}
	if (KeyDown(SDL_SCANCODE_A) && m_player.GetDst()->x > 0)
	{
		m_player.GetDst()->x -= m_speed;
		m_player.SetPlayerAngle(270);
		cout << m_player.GetPlayerAngle() << "\n\n\n" << endl;
	}
	else if (KeyDown(SDL_SCANCODE_D) && m_player.GetDst()->x < WIDTH - m_player.GetDst()->w)
	{
		m_player.SetPlayerAngle(90);
		m_player.GetDst()->x += m_speed;
		cout << m_player.GetPlayerAngle() << "\n\n\n" << endl;
	}
	if (frameCount % (60 * 1) == 0) // Spawining the hoard
	{
		m_enemy.push_back(new Enemy({rand() % 944,-200 }));
		m_enemy.shrink_to_fit();
	}
	if (frameCount % (60 * 2) == 0)
	{	
		for (unsigned i = 0; i < m_enemy.size(); i++)
		{ // Firing zee enemy lasers
			if (m_enemy[i]->GetDst()->y >= -40)
			{
				m_ebullet.push_back(new EnemyBullet({ m_enemy[i]->GetDst()->x + 22, m_enemy[i]->GetDst()->y + 35 }));
				m_enemy.shrink_to_fit();
				//Mix_PlayChannel(-1, m_redLaser, 0);
			}
		}
	}
	frameCount++;
	
	for (unsigned i = 0; i < m_ebullet.size(); i++) // Update the enemy lasers(red)
		m_ebullet[i]->Update();
	for (unsigned i = 0; i < m_enemy.size(); i++) // Update the hoard ships
		m_enemy[i]->Update();
	for (unsigned i = 0; i < m_pbullet.size(); i++) // Update the player lasers(blue)
		m_pbullet[i]->Update();
	
	// Check for colision between player laser and enemy ships
	for (unsigned i = 0; i < m_enemy.size(); i++)
	{
		for (unsigned c = 0; c < m_pbullet.size(); c++)
		{
			if ( SDL_HasIntersection(m_pbullet[c]->GetDst(), m_enemy[i]->GetDst())) 
			{
				cout << "Collision" << endl;
				Mix_PlayChannel(-1, m_explode, 0);
				// Destroys enemy upon collision
				delete m_enemy[i]; // Flag for re-allocation 'for sale'
				m_enemy[i] = nullptr; // Wrangle your dangle
				m_enemy.erase(m_enemy.begin() + i);
				m_enemy.shrink_to_fit();
				// Destroys player bullet upon collision
				delete m_pbullet[c]; // Flag for re-allocation 'for sale'
				m_pbullet[c] = nullptr; // Wrangle your dangle
				m_pbullet.erase(m_pbullet.begin() + c);
				m_pbullet.shrink_to_fit();
				break;
			}
		}
	}
	// Check for colision between player lasers and enemy lasers
	for (unsigned i = 0; i < m_ebullet.size(); i++)
	{
		for (unsigned c = 0; c < m_pbullet.size(); c++)
		{
			if (SDL_HasIntersection(m_pbullet[c]->GetDst(), m_ebullet[i]->GetDst()))
			{
				cout << "Collision" << endl;
				delete m_ebullet[i]; // Flag for re-allocation 'for sale'
				m_ebullet[i] = nullptr; // Wrangle your dangle
				m_ebullet.erase(m_ebullet.begin() + i);
				m_ebullet.shrink_to_fit();


				delete m_pbullet[c]; // Flag for re-allocation 'for sale'
				m_pbullet[c] = nullptr; // Wrangle your dangle
				m_pbullet.erase(m_pbullet.begin() + c);
				m_pbullet.shrink_to_fit();
				break;
			}
		}
	}
	// Check for colision between enemy lasers and player
	for (unsigned i = 0; i < m_ebullet.size(); i++)
	{
		if (SDL_HasIntersection(m_player.GetDst(), m_ebullet[i]->GetDst()))
		{	
			cout << "YOU HAVE DIED" << endl;
			Mix_PlayChannel(-1, m_explode, 0);
			delete m_ebullet[i]; // Flag for re-allocation 'for sale'
			m_ebullet[i] = nullptr; // Wrangle your dangle
			m_ebullet.erase(m_ebullet.begin() + i);
			m_ebullet.shrink_to_fit();
			break;
		}
	}
	// Check for colision between enemy ships and player
	for (unsigned i = 0; i < m_enemy.size(); i++)
	{
		if (SDL_HasIntersection(m_player.GetDst(), m_enemy[i]->GetDst()))
		{
			cout << "YOU HAVE DIED" << endl;
			Mix_PlayChannel(-1, m_explode, 0);
			delete m_enemy[i]; // Flag for re-allocation 'for sale'
			m_enemy[i] = nullptr; // Wrangle your dangle
			m_enemy.erase(m_enemy.begin() + i);
			m_enemy.shrink_to_fit();
			break;
		}
	}
	// Putting memory on the market as bullets and enemy ships go off screen
	// Player bullets going off screen
	for (unsigned i = 0; i < m_pbullet.size(); i++) 
	{
		if (m_pbullet[i]->GetDst()->y <= -(m_pbullet[i]->GetDst()->h)) // Off screen
		{
		 	delete m_pbullet[i]; // Flag for re-allocation 'for sale'
			m_pbullet[i] = nullptr; // Wrangle your dangle
			m_pbullet.erase(m_pbullet.begin() + i);
			m_pbullet.shrink_to_fit();
			break;
		}
	}
	// Enemies going off screen
	for (unsigned i = 0; i < m_enemy.size(); i++)
	{
		if (m_enemy[i]->GetDst()->y >= 768) // Off screen
		{
			delete m_enemy[i]; // Flag for re-allocation 'for sale'
			m_enemy[i] = nullptr; // Wrangle your dangle
			m_enemy.erase(m_enemy.begin() + i);
			m_enemy.shrink_to_fit();
			break;
		}
	}
	// Enemy bullets going off screen
	for (unsigned i = 0; i < m_ebullet.size(); i++)
	{
		if (m_ebullet[i]->GetDst()->y >= 768 + (m_ebullet[i]->GetDst()->h)) // Off screen
		{
			delete m_ebullet[i]; // Flag for re-allocation 'for sale'
			m_ebullet[i] = nullptr; // Wrangle your dangle
			m_ebullet.erase(m_ebullet.begin() + i);
			m_ebullet.shrink_to_fit();
			break;
		}
	}
}


void Engine::Render()
{
	SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255);
	SDL_RenderClear(m_pRenderer);
	//Draw Here

	//Render Background
	SDL_RenderCopy(m_pRenderer, m_pBGTexture, m_bg1.GetSrc(), m_bg1.GetDst());
	SDL_RenderCopy(m_pRenderer, m_pBGTexture, m_bg2.GetSrc(), m_bg2.GetDst());
	
	// Render an enemy ship
	for (unsigned i = 0; i < m_enemy.size(); i ++)
	SDL_RenderCopy(m_pRenderer, m_enemyTexture, nullptr, m_enemy[i]->GetDst());
	// Render a player laser
	for (unsigned i = 0; i < m_pbullet.size(); i++)
		SDL_RenderCopyEx(m_pRenderer, m_pLaserTexture, nullptr, m_pbullet[i]->GetDst(),m_player.GetPlayerAngle(),0,SDL_FLIP_NONE);
	// Render an enemy laser
	for (unsigned i = 0; i < m_ebullet.size(); i++)
		SDL_RenderCopy(m_pRenderer, m_eLaserTexture, nullptr, m_ebullet[i]->GetDst());
	// Render Player Ship
	SDL_RenderCopyEx(m_pRenderer, m_pTexture, m_player.GetSrc(), m_player.GetDst(),m_player.GetPlayerAngle(),0,SDL_FLIP_NONE);  
	
	// Flip buffers - send data to Window.
	SDL_RenderPresent(m_pRenderer);
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
	if (Init("GAME1007 M2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, NULL) == false)
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

void Engine::Clean()
{
	cout << "Cleaning engine..." << endl;
	for (unsigned i = 0; i < m_pbullet.size(); i++) // Cleanup leftover player bullets(blue laser)
	{
		delete m_pbullet[i]; // Flag for re-allocation 'for sale'
		m_pbullet[i] = nullptr; // Wrangle your dangle
	}
	m_pbullet.clear(); // Clears the vector
	m_pbullet.shrink_to_fit(); // Reduces capacity to size
	for (unsigned i = 0; i < m_ebullet.size(); i++) // Cleanup levtover enemy bullets(red laser)
	{
		delete m_ebullet[i]; // Flag for re-allocation 'for sale'
		m_ebullet[i] = nullptr; // Wrangle your dangle
	}
	m_ebullet.clear(); // Clears the vector
	m_ebullet.shrink_to_fit(); // Reduces capacity to size
	for (unsigned i = 0; i < m_enemy.size(); i++) // Cleanup leftover enemies
	{
		delete m_enemy[i]; // Flag for re-allocation 'for sale'
		m_enemy[i] = nullptr; // Wrangle your dangle
	}
	m_enemy.clear(); // Clears the vector
	m_enemy.shrink_to_fit(); // Reduces capacity to size
	
	SDL_DestroyRenderer(m_pRenderer);
	SDL_DestroyWindow(m_pWindow);
	SDL_DestroyTexture(m_pTexture);
	SDL_DestroyTexture(m_pLaserTexture);
	SDL_DestroyTexture(m_pBGTexture);
	
	Mix_FreeChunk(m_redLaser);
	Mix_FreeChunk(m_blueLaser);
	Mix_FreeMusic(m_bgMusic);

	Mix_CloseAudio();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}


