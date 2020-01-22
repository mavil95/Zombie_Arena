#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace sf;

class Pickup
{
private:
	const int HEALTH_START_VALUE = 50;
	const int AMMO_START_VALUE = 12;
	const int START_WAIT_TIME = 10;
	const int START_SECONDS_TO_LIVE = 10;

	Sprite m_Sprite;
	IntRect m_Arena;
	int m_Value;
	int m_Type;
	bool m_Spawned;
	float m_SecondsSinceSpawn;
	float m_SecondsSinceDeSpawn;
	float m_SecondsToLive;
	float m_SecondsToWait;
	

public:
	Sound m_SpawnSound;

	Pickup(int type);
	void setArena(IntRect arena);							// prepare new pickup
	void spawn();
	FloatRect getPosition();
	Sprite getSprite();
	void update(float elapsedTime);							// update each frame
	bool isSpawned();
	int gotIt();											// collision detect with player
	void upgrade();											// upgrade value of each pickup
	Vector2f getCenter();
};