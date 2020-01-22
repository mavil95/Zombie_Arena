#include "Pickup.h"
#include "TextureHolder.h"

Pickup::Pickup(int type)
{
	m_Type = type;
	if (m_Type == 1)
	{
		m_Sprite = Sprite(TextureHolder::GetTexture("graphics/health_pickup.png"));
		m_Value = HEALTH_START_VALUE;
	}
	else
	{
		m_Sprite = Sprite(TextureHolder::GetTexture("graphics/ammo_pickup.png"));
		m_Value = AMMO_START_VALUE;
	}
	m_Sprite.setOrigin(getPosition().left - getPosition().width / 2, getPosition().top - getPosition().height / 2);
	m_SecondsToLive = START_SECONDS_TO_LIVE;
	m_SecondsToWait = START_WAIT_TIME;
}

void Pickup::setArena(IntRect arena)
{
	m_Arena.left = arena.left + 75;
	m_Arena.width = arena.width - 75;
	m_Arena.top = arena.top + 75;
	m_Arena.height = arena.height - 75;

	spawn();
}

void Pickup::spawn()
{
	srand((int)time(0) / m_Type);							// random location spawn
	int x = (rand() % m_Arena.width);
	srand((int)time(0) * m_Type);
	int y = (rand() % m_Arena.height);

	m_SecondsSinceSpawn = 0;
	m_Spawned = true;
	m_Sprite.setPosition(x, y);
}

FloatRect Pickup::getPosition()
{
	return m_Sprite.getGlobalBounds();
}

Sprite Pickup::getSprite()
{
	return m_Sprite;
}

bool Pickup::isSpawned()
{
	return m_Spawned;
}

int Pickup::gotIt()
{
	m_Spawned = false;
	m_SecondsSinceDeSpawn = 0;
	return m_Value;
}

void Pickup::update(float elapsedTime)
{
	if (m_Spawned)													// timers
	{
		m_SecondsSinceSpawn += elapsedTime;
	}
	else
	{
		m_SecondsSinceDeSpawn += elapsedTime;
	}

	if (m_SecondsSinceSpawn > m_SecondsToLive&& m_Spawned)			// hide pickup
	{
		m_Spawned = false;
		m_SecondsSinceDeSpawn = 0;
	}

	if (m_SecondsSinceDeSpawn > m_SecondsToWait && !m_Spawned)
	{
		m_SpawnSound.play();
		spawn();
	}

}

void Pickup::upgrade()
{
	if (m_Type == 1)											// stronger
	{
		m_Value += (HEALTH_START_VALUE * 0.5);
	}
	else
	{
		m_Value += (AMMO_START_VALUE * 0.5);
	}

	m_SecondsToLive += (START_SECONDS_TO_LIVE / 10);			// more frequent
	m_SecondsToWait -= (START_WAIT_TIME / 10);
}

Vector2f Pickup::getCenter()
{
	Vector2f result;
	result.x = this->getPosition().left - this->getPosition().width / 2;
	result.y = this->getPosition().top - this->getPosition().height / 2;
	return result;
}