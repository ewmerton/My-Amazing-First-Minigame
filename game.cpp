#include "game.h"

Game::Game() { //Constructor
}
Game::~Game() {
}

bool Game::Init() {
	SDL_Init(SDL_INIT_EVERYTHING); //Init
	SDL_CreateWindowAndRenderer(WIN_WIDTH, WIN_HEIGHT, 0, &win, &ren); //create a window with renderer
	SDL_SetWindowTitle(win, "Our First Game!!"); //name the window

	for (int i = 0; i < MAX_KEYS; i++) {
		keys[i] = KEY_IDLE;
	}

	LoadImages();
	LoadMusic();

	Player.Init(WIN_WIDTH/2, WIN_HEIGHT*3/4, 82, 104, 5);
	idxShot = 0;
	lT = 0, cT = 0;
	int h;
	SDL_QueryTexture(img_background, NULL, NULL, NULL, &h);
	Scene.Init(0, 0, WIN_WIDTH, h, 4);
	godMode = false;

	return true;
}

bool Game::LoadImages() {
	IMG_Init(IMG_INIT_PNG);

	img_background = SDL_CreateTextureFromSurface(ren, IMG_Load("backgrounds.png"));
	img_player = SDL_CreateTextureFromSurface(ren, IMG_Load("spaceshipPro.png"));
	img_shot = SDL_CreateTextureFromSurface(ren, IMG_Load("shot.png"));

	return true;
}

bool Game::LoadMusic() {
	Mix_Init(MIX_INIT_OGG);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
	Mix_VolumeMusic(5);

	music = Mix_LoadMUS("music.ogg");
	fx_shot = Mix_LoadWAV("laser.wav");
	Mix_PlayMusic(music, 1);
	Mix_VolumeChunk(fx_shot, 5);

	return true;
}

void Game::Release() {
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_DestroyTexture(img_background);
	SDL_DestroyTexture(img_player);
	SDL_DestroyTexture(img_shot);
	Mix_FreeMusic(music);
	Mix_FreeChunk(fx_shot);
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
	srand(time(NULL));
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
	if (keys[SDL_SCANCODE_KP_4] == KEY_REPEAT)
	{
		cT = SDL_GetTicks();
		if (cT > lT + SHOTDELAY) {
			int x, y;
			int w, h;
			Player.GetRect(&x, &y, &w, &h);
			//size: 56x20
			Shots[idxShot].Init(x + 15, y + 29, 20, 56, SHOTSPEED);
			idxShot++;
			idxShot %= MAX_SHOTS;
			Shots[idxShot].Init(x + 47, y + 29, 20, 56, SHOTSPEED);
			idxShot++;
			idxShot %= MAX_SHOTS;
			Mix_PlayChannel(2, fx_shot, 0);
			lT = cT;
		}
	}

	Scene.Move(0, 1);
	if (Scene.GetY() >= 0) {
		std::cout << Scene.GetHeight()<<endl<< Scene.GetY()<<endl;
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

	SDL_RenderPresent(ren);
	SDL_Delay(10);
}
