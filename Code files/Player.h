#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Player
{
private:
	const float START_SPEED = 200;
	const float START_HEALTH = 100;

	Vector2f m_Position;							// player position
	Sprite m_Sprite;								// create sprite
	Texture m_Texture;								// and texture
	Vector2f m_Resolution;							// screen resolution
	IntRect m_Arena;								// current arena
	int m_TileSize;									// size of each tile

	bool m_UpPressed;								// move direction
	bool m_DownPressed;
	bool m_LeftPressed;
	bool m_RightPressed;

	

	int m_Health;									// current HP
	int m_MaxHealth;								// max hp
	Time m_LastHit;									// time of death
	float m_Speed;									// current speed

public:
	Player();

	bool m_Reload;

	void spawn(IntRect arena, Vector2f resolution, int tileSize);
	void resetPlayerStats();						// reset character after every game
	bool hit(Time timeHit);							// handle hit by a zombie
	Time getLastHitTime();							// time of last hit
	FloatRect getPosition();						// position of player
	Vector2f getCenter();							// player's center
	float getRotation();							// angle of player's facing
	Sprite getSprite();								// send cope of a sprite to main
	
	void moveLeft();								// move player
	void moveRight();
	void moveUp();
	void moveDown();
	void stopLeft();								// stop moving
	void stopRight();
	void stopUp();
	void stopDown();
													// we will call this once every frame
	void update(float elapsedTime, Vector2i mousePosition);
	void upgradeSpeed();							// speed boost
	void upgradeHealth();							// health boost
	void increaseHealthLevel(int amount);			// max hp boost
	int getHealth();								// current hp
};