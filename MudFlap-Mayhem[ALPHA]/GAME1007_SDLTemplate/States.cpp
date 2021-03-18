#include "States.h"
#include "StateManager.h"
#include "Engine.h"
#include <iostream>
using namespace std;

void State::Render() 
{
	SDL_RenderPresent(Engine::Instance().GetRenderer());
}

void State::Resume() {}

// Begin TitleState
TitleState::TitleState(){}

void TitleState::Enter()
{
	m_pBGTexture = IMG_LoadTexture(Engine::Instance().GetRenderer(), "img/mudflap_title.png ");
	m_bg1.SetRekts({ 0,0,WIDTH,HEIGHT }, { 0,0,WIDTH,HEIGHT });
	m_pPlayerTexture = IMG_LoadTexture(Engine::Instance().GetRenderer(), "img/semi_truck.png");
	//m_player.SetRekts({ 0,0,162,130 }, { 431,300,162,130 }); // Player First {} is source rect, second {} destination rect
	
	

	cout << "Entering TitleState..." << endl;
}

void TitleState::Update()
{
	if (Engine::Instance().KeyDown(SDL_SCANCODE_N))
		STMA::ChangeState(new GameState()); // Change to new GamesState 
}

void TitleState::Render()
{
	cout << "Rendering TitleState..." << endl;
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 255, 0, 255, 255);
	SDL_RenderClear(Engine::Instance().GetRenderer());
	SDL_RenderCopy(Engine::Instance().GetRenderer(), m_pBGTexture, m_bg1.GetSrc(), m_bg1.GetDst());
	State::Render();
}

void TitleState::Exit()
{
	cout << "Exiting TitleState..." << endl;
	
}
// End TitleState

//Begin GameState
GameState::GameState(){}

void GameState::Enter() // Used for initialization
{
	cout << "Entering GameState..." << endl;
	// Initialize Textures
	m_pBGTexture = IMG_LoadTexture(Engine::Instance().GetRenderer(), "img/mud_background.png ");
	
	m_pLaserTexture = IMG_LoadTexture(Engine::Instance().GetRenderer(), "img/blue_laser.png");
	m_eLaserTexture = IMG_LoadTexture(Engine::Instance().GetRenderer(), "img/red_laser.png");
	m_pPlayerTexture = IMG_LoadTexture(Engine::Instance().GetRenderer(), "img/semi_truck.png");
	m_pEnemyTexture = IMG_LoadTexture(Engine::Instance().GetRenderer(), "img/enemy_ship.png");

	m_player.SetRekts({ 0,0,162,130 }, { 431,600,60,100 }); // Player First {} is source rect, second {} destination rect

	m_bg1.SetRekts({ 0,0,WIDTH,HEIGHT }, { 0,0,WIDTH,HEIGHT });

	m_wallTexture = IMG_LoadTexture(Engine::Instance().GetRenderer(), "img/tileset.png");
	m_wall1.SetRekts({ 80,0,16,16 }, { 64,0,896,64 });
	m_wall2.SetRekts({ 80,0,16,16 }, { 64,704,896,64 });
	m_wall3.SetRekts({ 96,0,16,16 }, { 0,64,64,HEIGHT - 64 });
	m_wall4.SetRekts({ 96,0,16,16 }, { WIDTH - 64, 64 ,64,HEIGHT - 64 });
	m_corner1.SetRekts({ 64,0,16,16 }, { 0,0,64,64 });
	m_corner2.SetRekts({ 64,0,16,16 }, { 960,0,64,64 });
	m_corner3.SetRekts({ 64,0,16,16 }, { 0,704,64,64 });
	m_corner4.SetRekts({ 64,0,16,16 }, { 960,704,64,64 });

	m_healthBarTexture = IMG_LoadTexture(Engine::Instance().GetRenderer(), "img/HealthBarSheet.png");
	

	// Load sounds.
	m_blueLaser = Mix_LoadWAV("aud/blue_laser_sound.wav");
	m_redLaser = Mix_LoadWAV("aud/red_laser_sound.wav");
	m_explode = Mix_LoadWAV("aud/collision_explode_sound.wav");
	Mix_VolumeChunk(m_redLaser, 30);
	Mix_VolumeChunk(m_explode,40);

	// Load music.
	m_bgMusic = Mix_LoadMUS("Aud/moonlight_sonata.mp3");

	//Play BG music
	//Mix_PlayMusic(m_bgMusic, -1);
	//Mix_VolumeMusic(20);
	
}

int frameCount = 0;
void GameState::Update()
{
	
	if (Engine::Instance().KeyDown(SDL_SCANCODE_W) && m_player.GetDst()->y > 0)
	{
		m_player.GetDst()->y -= m_speed;
		m_player.SetPlayerAngle(0);
	}
	else if (Engine::Instance().KeyDown(SDL_SCANCODE_S) && m_player.GetDst()->y < HEIGHT - m_player.GetDst()->h)
	{
		m_player.GetDst()->y += m_speed;
		m_player.SetPlayerAngle(180);
	}
	if (Engine::Instance().KeyDown(SDL_SCANCODE_A) && m_player.GetDst()->x > 0)
	{
		m_player.GetDst()->x -= m_speed;
		m_player.SetPlayerAngle(270);
	}
	else if (Engine::Instance().KeyDown(SDL_SCANCODE_D) && m_player.GetDst()->x < WIDTH - m_player.GetDst()->w)
	{
		m_player.SetPlayerAngle(90);
		m_player.GetDst()->x += m_speed;
	}
	
	if (Engine:: Instance().KeyDown(SDL_SCANCODE_SPACE) && Engine::Instance().m_bCanShoot)
	{
		//Spawn a new player bullet
		Engine::Instance().m_bCanShoot = false;
		m_pBullet.push_back (new Bullet({ m_player.GetDst()->x +44, m_player.GetDst()->y-20},m_player.GetPlayerAngle()));
		m_pBullet.shrink_to_fit();
		//Mix_PlayChannel(-1, m_blueLaser, 0);
	}

	// Spawning Enemies
	if (frameCount % (60 * 2) == 0) // Spawining the hoard
	{
		m_enemy.push_back(new Enemy({ rand() % 944,-200 }));
		m_enemy.shrink_to_fit();
	}
	if (frameCount % (60 * 2) == 0)
	{
		for (unsigned i = 0; i < m_enemy.size(); i++)
		{ // Firing zee enemy lasers
			if (m_enemy[i]->GetDst()->y >= -100)
			{
				m_pEnemyBullet.push_back(new EnemyBullet({ m_enemy[i]->GetDst()->x + 22, m_enemy[i]->GetDst()->y + 35 }));
				m_enemy.shrink_to_fit();
				//Mix_PlayChannel(-1, m_redLaser, 0);
			}
		}
	}
	++frameCount;

	for (unsigned i = 0; i < m_enemy.size(); i++) // Update the hoard ships
		m_enemy[i]->Update();
	for (unsigned i = 0; i < m_pBullet.size(); i++) // Update the player lasers(blue)
		m_pBullet[i]->Update();
	for (unsigned i = 0; i < m_pEnemyBullet.size(); i++) // Update the player lasers(blue)
		m_pEnemyBullet[i]->Update();
	
	// Check for colision between player laser and enemy ships
	for (unsigned i = 0; i < m_enemy.size(); i++)
	{
		for (unsigned c = 0; c < m_pBullet.size(); c++)
		{
			if (SDL_HasIntersection(m_pBullet[c]->GetDst(), m_enemy[i]->GetDst()))
			{
				cout << "Collision" << endl;
				//Mix_PlayChannel(-1, m_explode, 0);
				// Destroys enemy upon collision
				delete m_enemy[i]; // Flag for re-allocation 'for sale'
				m_enemy[i] = nullptr; // Wrangle your dangle
				m_enemy.erase(m_enemy.begin() + i);
				m_enemy.shrink_to_fit();
				// Destroys player bullet upon collision
				delete m_pBullet[c]; // Flag for re-allocation 'for sale'
				m_pBullet[c] = nullptr; // Wrangle your dangle
				m_pBullet.erase(m_pBullet.begin() + c);
				m_pBullet.shrink_to_fit();
				break;
			}
		}
	}
	// Check for colision between player lasers and enemy lasers
	for (unsigned i = 0; i < m_pEnemyBullet.size(); i++)
	{
		for (unsigned c = 0; c < m_pBullet.size(); c++)
		{
			if (SDL_HasIntersection(m_pBullet[c]->GetDst(), m_pEnemyBullet[i]->GetDst()))
			{
				cout << "Collision" << endl;
				delete m_pEnemyBullet[i]; // Flag for re-allocation 'for sale'
				m_pEnemyBullet[i] = nullptr; // Wrangle your dangle
				m_pEnemyBullet.erase(m_pEnemyBullet.begin() + i);
				m_pEnemyBullet.shrink_to_fit();

				delete m_pBullet[c]; // Flag for re-allocation 'for sale'
				m_pBullet[c] = nullptr; // Wrangle your dangle
				m_pBullet.erase(m_pBullet.begin() + c);
				m_pBullet.shrink_to_fit();
				break;
			}
		}
	}
	// Check for colision between enemy lasers and player
	for (unsigned i = 0; i < m_pEnemyBullet.size(); i++)
	{
		if (SDL_HasIntersection(m_player.GetDst(), m_pEnemyBullet[i]->GetDst()))
		{
			//Mix_PlayChannel(-1, m_explode, 0);
			delete m_pEnemyBullet[i]; // Flag for re-allocation 'for sale'
			m_pEnemyBullet[i] = nullptr; // Wrangle your dangle
			m_pEnemyBullet.erase(m_pEnemyBullet.begin() + i);
			m_pEnemyBullet.shrink_to_fit();
			HealthCheck();
			break;
		}
	}
	// Check for colision between enemy ships and player
	for (unsigned i = 0; i < m_enemy.size(); i++)
	{
		if (SDL_HasIntersection(m_player.GetDst(), m_enemy[i]->GetDst()))
		{
			//Mix_PlayChannel(-1, m_explode, 0);
			delete m_enemy[i]; // Flag for re-allocation 'for sale'
			m_enemy[i] = nullptr; // Wrangle your dangle
			m_enemy.erase(m_enemy.begin() + i);
			m_enemy.shrink_to_fit();
			HealthCheck();
			break;
		}
	}

	//A little cleanup of dangling pointers and freeing up memory
	// Player bullets going off screen
	for (unsigned i = 0; i < m_pBullet.size(); i++)
	{
		if (m_pBullet[i]->GetDst()->y <= -(m_pBullet[i]->GetDst()->h)) // Off screen
		{
			delete m_pBullet[i]; // Flag for re-allocation 'for sale'
			m_pBullet[i] = nullptr; // Wrangle your dangle
			m_pBullet.erase(m_pBullet.begin() + i);
			m_pBullet.shrink_to_fit();
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
	for (unsigned i = 0; i < m_pEnemyBullet.size(); i++)
	{
		if (m_pEnemyBullet[i]->GetDst()->y >= 768 + (m_pEnemyBullet[i]->GetDst()->h)) // Off screen
		{
			delete m_pEnemyBullet[i]; // Flag for re-allocation 'for sale'
			m_pEnemyBullet[i] = nullptr; // Wrangle your dangle
			m_pEnemyBullet.erase(m_pEnemyBullet.begin() + i);
			m_pEnemyBullet.shrink_to_fit();
			break;
		}
	}

	if (Engine::Instance().KeyDown(SDL_SCANCODE_X))
		STMA::ChangeState(new TitleState()); // Change to new TitleState
	else if(Engine::Instance().KeyDown(SDL_SCANCODE_P))
		STMA::PushState(new PauseState()); // Add new PauseState
	if (Engine::Instance().KeyDown(SDL_SCANCODE_M))
		STMA::ChangeState(new WinState()); // Change to new WinState (Using to force win state for now)
}

void GameState::Render()
{
	//cout << "Rendering GameState..." << endl;
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 0, 0, 0, 255);
	SDL_RenderClear(Engine::Instance().GetRenderer());
	SDL_RenderCopy(Engine::Instance().GetRenderer(), m_pBGTexture, m_bg1.GetSrc(), m_bg1.GetDst());
	// Render Border Walls -- There is a better way to do this, this was just my band-aid solution
	SDL_RenderCopy(Engine::Instance().GetRenderer(), m_wallTexture, m_wall1.GetSrc(), m_wall1.GetDst());
	SDL_RenderCopy(Engine::Instance().GetRenderer(), m_wallTexture, m_wall2.GetSrc(), m_wall2.GetDst());
	SDL_RenderCopy(Engine::Instance().GetRenderer(), m_wallTexture, m_wall3.GetSrc(), m_wall3.GetDst());
	SDL_RenderCopy(Engine::Instance().GetRenderer(), m_wallTexture, m_wall4.GetSrc(), m_wall4.GetDst());
	SDL_RenderCopyEx(Engine::Instance().GetRenderer(), m_wallTexture, m_corner1.GetSrc(), m_corner1.GetDst(), NULL, NULL, SDL_FLIP_HORIZONTAL);
	SDL_RenderCopyEx(Engine::Instance().GetRenderer(), m_wallTexture, m_corner2.GetSrc(), m_corner2.GetDst(), NULL, NULL, SDL_FLIP_NONE);
	SDL_RenderCopyEx(Engine::Instance().GetRenderer(), m_wallTexture, m_corner3.GetSrc(), m_corner3.GetDst(), 180, NULL, SDL_FLIP_NONE);
	SDL_RenderCopyEx(Engine::Instance().GetRenderer(), m_wallTexture, m_corner4.GetSrc(), m_corner4.GetDst(), NULL, NULL, SDL_FLIP_VERTICAL);

	//Render Health bar
	SDL_RenderCopy(Engine::Instance().GetRenderer(), m_healthBarTexture, m_healthFull.GetSrc(), m_healthFull.GetSrc());
	SDL_RenderCopy(Engine::Instance().GetRenderer(), m_healthBarTexture, m_health3.GetSrc(), m_health3.GetSrc());
	SDL_RenderCopy(Engine::Instance().GetRenderer(), m_healthBarTexture, m_health4.GetSrc(), m_health4.GetSrc());
	SDL_RenderCopy(Engine::Instance().GetRenderer(), m_healthBarTexture, m_healthDead.GetSrc(), m_healthDead.GetSrc());
	// Render Player Ship
	SDL_RenderCopyEx(Engine::Instance().GetRenderer(), m_pPlayerTexture, m_player.GetSrc(), m_player.GetDst(),m_player.GetPlayerAngle(),0,SDL_FLIP_NONE);
	for (unsigned i = 0; i < m_pBullet.size(); i++)
		SDL_RenderCopyEx(Engine::Instance().GetRenderer(), m_pLaserTexture, nullptr, m_pBullet[i]->GetDst(),m_pBullet[i]->GetBulletAngle(),0,SDL_FLIP_NONE);
		//SDL_RenderCopy(Engine::Instance().GetRenderer(), m_pLaserTexture, nullptr, m_pBullet[i]->GetDst());

	// Render an enemy ship
	for (unsigned i = 0; i < m_enemy.size(); i++)
		SDL_RenderCopy(Engine::Instance().GetRenderer(), m_pEnemyTexture, nullptr, m_enemy[i]->GetDst());
	// Render an enemy laser
	for (unsigned i = 0; i < m_pEnemyBullet.size(); i++)
		SDL_RenderCopy(Engine::Instance().GetRenderer(), m_eLaserTexture, nullptr, m_pEnemyBullet[i]->GetDst());


	if (dynamic_cast<GameState*>(STMA::GetStates().back()) ) // Check to see if current state is of type GameState
		State::Render();
}

void GameState::Exit()
{
	cout << "Exiting Gamestate" << endl;
}

void GameState::Resume()
{
	cout << "Resuming GameState..." << endl;
	Mix_ResumeMusic();
}

//Checking health status and displaying the proper health bar sprite.
void GameState::HealthCheck()
{
	m_player.SetPlayerHealth(m_player.GetPlayerHealth() - 1);
	cout << m_player.GetPlayerHealth() << endl;
	if (m_player.GetPlayerHealth() == 3)
	{
		SDL_DestroyTexture(m_healthBarTexture);
		SDL_RenderCopy(Engine::Instance().GetRenderer(), m_healthBarTexture, m_health2.GetSrc(), m_health2.GetSrc()); //Set its rects 
	}
	else if (m_player.GetPlayerHealth() == 2)
	{
		SDL_DestroyTexture(m_healthBarTexture);
		SDL_RenderCopy(Engine::Instance().GetRenderer(), m_healthBarTexture, m_health3.GetSrc(), m_health3.GetSrc()); //Set its rects 
	}
	else if (m_player.GetPlayerHealth() == 1)
	{
		SDL_DestroyTexture(m_healthBarTexture);
		SDL_RenderCopy(Engine::Instance().GetRenderer(), m_healthBarTexture, m_health4.GetSrc(), m_health4.GetSrc()); //Set its rects 
	}
	else if (m_player.GetPlayerHealth() == 0)
		STMA::PushState(new LoseState()); // Add new LoseState
}
// End GameState

// Begin PauseState
PauseState::PauseState(){}

void PauseState::Enter()
{
	// Create buttons in here
	cout << "Entering PauseState..." << endl;
	Mix_PauseMusic();
}

void PauseState::Update()
{
	
	if (Engine::Instance().KeyDown(SDL_SCANCODE_R))
		STMA::PopState();
}

void PauseState::Render()
{
	cout << "Rendering PauseState..." << endl;
	// First render GameState
	STMA::GetStates().front()->Render();
	
	// Now render rest of PauseState
	SDL_SetRenderDrawBlendMode(Engine::Instance().GetRenderer(), SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 100, 0, 100, 128);
	SDL_Rect rect = { 256,128,512,512 };
	SDL_RenderFillRect(Engine::Instance().GetRenderer(), &rect);

	State::Render();
}

void PauseState::Exit()
{
	cout << "Exiting PauseState..." << endl;
}
// Ending PauseState

//Beginning LoseState
LoseState::LoseState(){}

void LoseState::Enter()
{
	cout << "Entering LoseState" << endl;
	Mix_PauseMusic();
}

void LoseState::Update()
{
	if (Engine::Instance().KeyDown(SDL_SCANCODE_X))
		STMA::ChangeState(new TitleState()); // Change to new TitleState
}

void LoseState::Render()
{
	cout << "Rendering LoseState" << endl;
	STMA::GetStates().front()->Render();

	SDL_SetRenderDrawBlendMode(Engine::Instance().GetRenderer(), SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 100, 0, 0, 128);
	SDL_Rect rect = { 256,128,512,512 };
	SDL_RenderFillRect(Engine::Instance().GetRenderer(), &rect);
	State::Render();
}

void LoseState::Exit()
{
	
}

WinState::WinState() {}

void WinState::Enter()
{
	
}

void WinState::Update()
{
	if (Engine::Instance().KeyDown(SDL_SCANCODE_X))
		STMA::ChangeState(new TitleState()); // Change to new TitleState
}

void WinState::Render()
{
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 0, 0, 0, 255);
	SDL_Rect rect = { 0,0,1024,768 };
	SDL_RenderFillRect(Engine::Instance().GetRenderer(), &rect);
	State::Render();
}

void WinState::Exit()
{
}
