/*******************************************************************************
* File Name   :	Mudflap Mayhem [ALPHA]	 
* Description :	ALPHA for Mudflap Mayhem GAME PRODUCTION W2021
* Author      :	LevelUP Studios
* Created     :	
* Modified    :	17/03/2021
*******************************************************************************/

// SDL includes pasted for convenience. Move/copy to relevant files.
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"
#include "Engine.h"

int main(int argc, char* argv[])
{
	return Engine::Instance().Run();
}