#pragma once

class Entity {
public:
	Entity();
	~Entity();

	void Init(int posX, int posY, int w, int h, int s);
	void GetRect(int* posX, int* posY, int* w, int* h);
	void SetX(int posX);
	void SetY(int posY);
	int GetX();
	int GetY();
	int GetWidth();
	int GetHeight();
	void ShutDown();
	bool IsAlive();
	void Move(float dx, float dy);
private:
	int x, y, width, height, speed;
	bool isAlive;
};
