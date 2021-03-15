#pragma once

#include <iostream>
#include "entity.h"

#include "SDL/include/SDL.h"
#include "SDL_image/include/SDL_image.h"
#include "SDL_mixer/include/SDL_mixer.h"
#pragma comment( lib, "SDL/libx86/SDL2.lib" )
#pragma comment( lib, "SDL/libx86/SDL2main.lib" )
#pragma comment( lib, "SDL_image/libx86/SDL2_image.lib" )
#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

#define WIN_WIDTH 1024
#define WIN_HEIGHT 768
#define MAX_KEYS 256
#define MAX_SHOTS 32
#define SHOTSPEED 30
#define SHOTDELAY 200
#define SHIPSPEED 1.5f

using namespace std;

class Game {
public:
	Game();
	~Game();
	bool Init();
	bool LoadImages();
	bool LoadMusic();
	void Release();
	bool Input();
	bool Update();
	void Draw();
private:
	SDL_Renderer* ren;
	SDL_Window* win;
	SDL_Texture* img_background, * img_player, * img_shot;
	Mix_Music* music;
	Mix_Chunk* fx_shot;

	Entity Player, Shots[MAX_SHOTS], Scene;
	bool godMode;

	int idxShot, cT, lT;

	enum KEY_STATE { KEY_IDLE, KEY_DOWN, KEY_REPEAT, KEY_UP };
	KEY_STATE keys[MAX_KEYS];
};
