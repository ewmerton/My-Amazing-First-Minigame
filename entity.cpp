#include "entity.h"

Entity::Entity() {
	isAlive = false;
}

Entity::~Entity() {}

void Entity::Init(int posX, int posY, int w, int h, int s) {
	x = posX;
	y = posY;
	width = w;
	height = h;
	speed = s;
	isAlive = true;
}

void Entity::GetRect(int* posX, int* posY, int* w, int* h)
{
	*posX = x;
	*posY = y;
	*w = width;
	*h = height;
}

int Entity::GetX() {
	return x;
}
int Entity::GetY() {
	return y;
}

int Entity::GetWidth() {
	return width;
}

int Entity::GetHeight() {
	return height;
}

void Entity::SetX(int posX)
{
	x = posX;
}

void Entity::SetY(int posY)
{
	y = posY;
}

void Entity::ShutDown()
{
	isAlive = false;
}
bool Entity::IsAlive()
{
	return isAlive;
}

void Entity::Move(float dx, float dy)
{
	x += dx * speed;
	y += dy * speed;
}
