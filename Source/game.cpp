#include "game.h"

Game::Game() { //Constructor
}
Game::~Game() {
}

bool Game::Init() {
	SDL_Init(SDL_INIT_EVERYTHING); //Init
	SDL_CreateWindowAndRenderer(WIN_WIDTH, WIN_HEIGHT, 0, &win, &ren); //create a window with renderer
	SDL_SetWindowTitle(win, "A, S, W and D to move, SPACE to shoot, F to kill sound"); //name the window

	for (int i = 0; i < MAX_KEYS; i++) {
		keys[i] = KEY_IDLE;
	}

	LoadImages();
	LoadMusic();

	Player.Init(WIN_WIDTH/2, WIN_HEIGHT*3/4, 82, 104, 5);
	idxShot = 0, idxEnemy = 0;
	lTs = 0, cTs = 0, lTe = 0, cTe = 0, dist = 0, xEnemy = 0;
	int h;
	SDL_QueryTexture(img_background, NULL, NULL, NULL, &h);
	Scene.Init(0, 0, WIN_WIDTH, h, 4);
	godMode = false;

	return true;
}

bool Game::LoadImages() {
	IMG_Init(IMG_INIT_PNG);

	img_background = SDL_CreateTextureFromSurface(ren, IMG_Load("Assets/backgrounds.png"));
	img_player = SDL_CreateTextureFromSurface(ren, IMG_Load("Assets/spaceshipPro.png"));
	img_shot = SDL_CreateTextureFromSurface(ren, IMG_Load("Assets/shot.png"));
	img_enemy1 = SDL_CreateTextureFromSurface(ren, IMG_Load("Assets/enemy1.png"));
	img_enemy2 = SDL_CreateTextureFromSurface(ren, IMG_Load("Assets/enemy2.png"));
	img_enemy3 = SDL_CreateTextureFromSurface(ren, IMG_Load("Assets/enemy3.png"));

	return true;
}

bool Game::LoadMusic() {
	Mix_Init(MIX_INIT_OGG);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
	Mix_VolumeMusic(5);

	music = Mix_LoadMUS("Assets/bgMusic.ogg");
	fx_shot = Mix_LoadWAV("Assets/shotSound.wav");
	fx_dead = Mix_LoadWAV("Assets/deathSound.wav");
	Mix_PlayMusic(music, 1);
	Mix_VolumeChunk(fx_shot, 2);
	Mix_VolumeChunk(fx_dead, 4);

	return true;
}

void Game::Release() {
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_DestroyTexture(img_background);
	SDL_DestroyTexture(img_player);
	SDL_DestroyTexture(img_shot);
	SDL_DestroyTexture(img_enemy1);
	SDL_DestroyTexture(img_enemy2);
	SDL_DestroyTexture(img_enemy3);
	Mix_FreeMusic(music);
	Mix_FreeChunk(fx_shot);
	Mix_FreeChunk(fx_dead);
	Mix_CloseAudio();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}

bool Game::Input() {
	SDL_Event event;
	SDL_PollEvent(&event);
	SDL_PumpEvents();
	const Uint8* keyboard = SDL_GetKeyboardState(NULL);

	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keyboard[i])
			keys[i] = (keys[i] == KEY_IDLE) ? KEY_DOWN : KEY_REPEAT;
		else
			keys[i] = (keys[i] == KEY_REPEAT || keys[i] == KEY_DOWN) ? KEY_UP : KEY_IDLE;
	}

	return true;
}

bool Game::Update() {
	if (!Input())
	{
		return true;
	}

	float fx = 0, fy = 0;
	if (keys[SDL_SCANCODE_ESCAPE] == KEY_DOWN) {
		return true;
	}
	if (keys[SDL_SCANCODE_F1] == KEY_DOWN) {
		godMode = !godMode; //toggle God Mode
	}
	if (keys[SDL_SCANCODE_W] == KEY_REPEAT) {
		fy = -SHIPSPEED;
	}
	if (keys[SDL_SCANCODE_S] == KEY_REPEAT) {
		fy = SHIPSPEED;
	}
	if (keys[SDL_SCANCODE_A] == KEY_REPEAT) {
		fx = -SHIPSPEED;
	}
	if (keys[SDL_SCANCODE_D] == KEY_REPEAT) {
		fx = SHIPSPEED;
	}
	if ((keys[SDL_SCANCODE_KP_4] == KEY_REPEAT) || (keys[SDL_SCANCODE_SPACE] == KEY_REPEAT))
	{
		cTs = SDL_GetTicks();
		if (cTs > lTs + SHOTDELAY) {
			int x, y, w, h;
			Player.GetRect(&x, &y, &w, &h);
			//size: 56x20
			Shots[idxShot].Init(x + 15, y + 29, 20, 56, SHOTSPEED);
			idxShot++;
			idxShot %= MAX_SHOTS;
			Shots[idxShot].Init(x + 47, y + 29, 20, 56, SHOTSPEED);
			idxShot++;
			idxShot %= MAX_SHOTS;
			Mix_PlayChannel(2, fx_shot, 0);
			lTs = cTs;
		}
	}

	//Kill
	if (keys[SDL_SCANCODE_F] == KEY_DOWN) {
		Kill();
	}

	Scene.Move(0, 1);
	if (Scene.GetY() >= 0) {
		Scene.SetY(-(Scene.GetHeight() / 2));
	}

	//Player update
	Player.Move(fx, fy);

	//Shots update
	for (int i = 0; i < MAX_SHOTS; i++)
	{
		if (Shots[i].IsAlive())
		{
			Shots[i].Move(0, -1);
			if (Shots[i].GetX() > WIN_WIDTH) {
				Shots[i].ShutDown();
			}
		}
	}

	//Enemies
	cTe = SDL_GetTicks();
	if (cTe > lTe + 1000) {
		//size: 32x32
		for (int i = 0; i < 6; i++)
		{
			if (!Enemy[idxEnemy].IsAlive())
			{
				Enemy[idxEnemy].Init(xEnemy + dist, 0, 64, 64, 96);
			}
			else {
				Enemy[idxEnemy].Move(0, 1);
			}
			dist += 96;
			idxEnemy++;
		}
		dist = 0;
		idxEnemy %= MAX_ENEMIES;
		std::cout << idxEnemy << endl;
		lTe = cTe;
	}

	return false;
}

void Game::Draw() {
	SDL_Rect rect;
	SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	SDL_RenderClear(ren);

	if (godMode) {
		SDL_SetRenderDrawColor(ren, 192, 0, 0, 255);
	}

	Scene.GetRect(&rect.x, &rect.y, &rect.w, &rect.h);
	SDL_RenderCopy(ren, img_background, NULL, &rect);
	rect.x += rect.w;
	SDL_RenderCopy(ren, img_background, NULL, &rect);

	Player.GetRect(&rect.x, &rect.y, &rect.w, &rect.h);
	SDL_RenderCopy(ren, img_player, NULL, &rect);
	if (godMode) {
		SDL_RenderDrawRect(ren, &rect);
	}

	for (int i = 0; i < MAX_SHOTS; i++)
	{
		if (Shots[i].IsAlive())
		{
			Shots[i].GetRect(&rect.x, &rect.y, &rect.w, &rect.h);
			SDL_RenderCopy(ren, img_shot, NULL, &rect);
			if (godMode) {
				SDL_RenderDrawRect(ren, &rect);
			}
		}
	}

	for (int i = 0; i < MAX_ENEMIES; i++)
	{
		if (Enemy[i].IsAlive())
		{
			Enemy[i].GetRect(&rect.x, &rect.y, &rect.w, &rect.h);
			SDL_RenderCopy(ren, img_enemy1, NULL, &rect);
			if (godMode) {
				SDL_RenderDrawRect(ren, &rect);
			}
		}
	}

	SDL_RenderPresent(ren);
	SDL_Delay(10);
}

void Game::Kill() {
	Mix_PlayChannel(2, fx_dead, 0);
	//Enemy[].ShutDown();
}
