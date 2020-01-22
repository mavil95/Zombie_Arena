#include <sstream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Player.h"
#include "ZombieArena.h"
#include "TextureHolder.h"
#include "Bullet.h"
#include "Pickup.h"

using namespace sf;
using namespace std;

int main()
{
	// first preparation
	TextureHolder holder;
	enum class State									// four states of game
	{
		PAUSE, LEVELING_UP, GAME_OVER, PLAYING, CONTROLS, STAGE_CLEAR
	};
	State state = State::GAME_OVER;

	Vector2f resolution;								// get screen resolution and make window
	resolution.x = VideoMode::getDesktopMode().width;
	resolution.y = VideoMode::getDesktopMode().height;

	RenderWindow window(VideoMode(resolution.x, resolution.y), 
		"Zombie Arena", Style::Fullscreen);

	View mainView(sf::FloatRect(0, 0, resolution.x,			// create view for main action
		resolution.y));

	Clock clock;										// main clock
	Time gameTimeTotal;									// playing timer

	Vector2f mouseWorldPosition;						// mouse on world coordinates
	Vector2i mouseScreenPosition;						// mouse on screen coordinates

	Player player;
	IntRect arena;

	VertexArray background;								// create background
	Texture textureBackground = TextureHolder::GetTexture
	("graphics/background_sheet.png");

	int numZombies;
	int numZombiesAlive;
	Zombie* zombies = nullptr;

	Bullet bullets[100];								// create bullets
	int currentBullet = 0;
	int bulletsSpare = 24;
	int bulletsInClip = 6;
	int clipSize = 6;
	float fireRate = 1;
	Time lastPressed;
	Time lastReload;

	window.setMouseCursorVisible(false);				// set crosshair
	Sprite spriteCrosshair;
	Texture textureCrosshair = TextureHolder::GetTexture("graphics/crosshair.png");
	spriteCrosshair.setTexture(textureCrosshair);
	spriteCrosshair.setOrigin(25, 25);

	Pickup healthPickup(1);								// create two type of pickups
	Pickup ammoPickup(2);

	// About the game
	int score = 0;
	int hiScore = 0;

	Sprite spriteGameOver;												// game over background
	Texture textureGameOver = TextureHolder::GetTexture("graphics/background.png");
	spriteGameOver.setTexture(textureGameOver);
	spriteGameOver.setPosition(0, 0);

	View hudView(FloatRect(0, 0, resolution.x, resolution.y));			// create view for HUD
	
	Sprite spriteAmmoIcon;												// ammo icon
	Texture textureAmmoIcon = TextureHolder::GetTexture("graphics/ammo_icon.png");
	spriteAmmoIcon.setTexture(textureAmmoIcon);
	spriteAmmoIcon.setPosition(40, resolution.y - 100);

	Sprite spriteRedArrow;												// red arrow
	Texture textureRedArrow = TextureHolder::GetTexture("graphics/red_arrow2.png");
	spriteRedArrow.setTexture(textureRedArrow);
	spriteRedArrow.setOrigin(0, 25);

	Sprite spriteYellowArrow;											// Yellow arrow
	Texture textureYellowArrow = TextureHolder::GetTexture("graphics/yellow_arrow2.png");
	spriteYellowArrow.setTexture(textureYellowArrow);
	spriteYellowArrow.setOrigin(0, 25);

	// Text works
	Font font;															// load the font
	font.loadFromFile("fonts/ZOMBIE.ttf");

	Text pausedText;													// paused text
	pausedText.setFont(font);
	pausedText.setCharacterSize(85);
	pausedText.setFillColor(Color::White);
	pausedText.setString("  Press Esc\nto continue");
	FloatRect pausedTextRect = pausedText.getLocalBounds();
	pausedText.setOrigin(pausedTextRect.width / 2, pausedTextRect.height / 2);
	pausedText.setPosition(resolution.x / 2, resolution.y / 2);
	

	Text gameOverText;													// game over text
	gameOverText.setFont(font);
	gameOverText.setCharacterSize(80);
	gameOverText.setFillColor(Color::White);
	gameOverText.setString("Press Enter to play");
	FloatRect gameOverTextRect = gameOverText.getLocalBounds();
	gameOverText.setOrigin(gameOverTextRect.left + gameOverTextRect.width / 2, gameOverTextRect.top + gameOverTextRect.height / 2);
	gameOverText.setPosition(resolution.x / 2, resolution.y / 2);
	

	Text levelUpText;													// level up text
	levelUpText.setFont(font);
	levelUpText.setCharacterSize(60);
	levelUpText.setFillColor(Color::White);
	stringstream levelUpStream;
	levelUpStream << "Choose your Level Up bonus: " << "\n1 - Rate of fire" << "\n2 - Clip size" <<
		"\n3 - Max health" << "\n4 - Run speed" <<
		"\n5 - Health pickups upgrade" << "\n6 - Ammo pickups upgrade";
	levelUpText.setString(levelUpStream.str());
	FloatRect LevelUpTextRect = levelUpText.getLocalBounds();
	levelUpText.setOrigin(LevelUpTextRect.left + LevelUpTextRect.width / 2, LevelUpTextRect.top + LevelUpTextRect.height / 2);
	levelUpText.setPosition(resolution.x / 2, resolution.y / 2);

	Text stageClearText;												// stage clear text
	stageClearText.setFont(font);
	stageClearText.setCharacterSize(100);
	stageClearText.setFillColor(Color::White);
	stageClearText.setString("Stage clear");
	FloatRect stageClearTextRect = stageClearText.getLocalBounds();
	stageClearText.setOrigin(stageClearTextRect.left + stageClearTextRect.width / 2, stageClearTextRect.top + stageClearTextRect.height / 2);
	stageClearText.setPosition(resolution.x / 2, resolution.y / 2);

	Text stageClear2Text;												// stage clear text
	stageClear2Text.setFont(font);
	stageClear2Text.setCharacterSize(70);
	stageClear2Text.setFillColor(Color::White);
	stageClear2Text.setString("Press Enter to continue");
	FloatRect stageClear2TextRect = stageClear2Text.getLocalBounds();
	stageClear2Text.setOrigin(stageClear2TextRect.left + stageClear2TextRect.width / 2, stageClear2TextRect.top + stageClear2TextRect.height / 2);
	stageClear2Text.setPosition(resolution.x / 2, resolution.y / 2 - 150);

	Text seeControlsText;												// see controls text
	seeControlsText.setFont(font);
	seeControlsText.setCharacterSize(70);
	seeControlsText.setFillColor(Color::White);
	seeControlsText.setString("Press Enter to see controls");
	FloatRect seeControlsTextRect = seeControlsText.getLocalBounds();
	seeControlsText.setOrigin(seeControlsTextRect.left + seeControlsTextRect.width / 2, seeControlsTextRect.top + seeControlsTextRect.height / 2);
	seeControlsText.setPosition(resolution.x / 2, 70);

	Text unseeControlsText;												// unsee controls text
	unseeControlsText.setFont(font);
	unseeControlsText.setCharacterSize(70);
	unseeControlsText.setFillColor(Color::White);
	unseeControlsText.setString("Press Enter to return in menu");
	FloatRect unseeControlsTextRect = unseeControlsText.getLocalBounds();
	unseeControlsText.setOrigin(unseeControlsTextRect.left + unseeControlsTextRect.width / 2, unseeControlsTextRect.top + unseeControlsTextRect.height / 2);
	unseeControlsText.setPosition(resolution.x / 2, 70);

	Text controlsText;													// controls text
	controlsText.setFont(font);
	controlsText.setCharacterSize(55);
	controlsText.setFillColor(Color::White);
	stringstream controlsTextStream;
	controlsTextStream << "W - Move up" << "\nA - Move left" << "\nS - Move down" << "\nD - Move right" << 
		"\nR - Reload weapon" << "\nM - Mute music" << "\n'-' - Reduce music volume" << "\n'+' - Increase music volume" << 
		"\n9 - Reduce SFX volume" << "\n0 - Increase SFX volume" << "\nF12 - Exit from game";
	controlsText.setString(controlsTextStream.str());
	FloatRect controlsTextRect = controlsText.getLocalBounds();
	controlsText.setOrigin(controlsTextRect.left + controlsTextRect.width / 2, controlsTextRect.top + controlsTextRect.height / 2);
	controlsText.setPosition(resolution.x / 2, resolution.y / 2);

	Text ammoText;														// ammo text
	ammoText.setFont(font);
	ammoText.setCharacterSize(50);
	ammoText.setFillColor(Color::White);
	ammoText.setPosition(116, resolution.y - 100);

	Text scoreText;														// score text
	scoreText.setFont(font);
	scoreText.setCharacterSize(50);
	scoreText.setFillColor(Color::White);
	scoreText.setPosition(20, 20);

	ifstream inputFile("gamedata/scores.txt");							// load hi score from txt file
	if (inputFile.is_open())
	{
		inputFile >> hiScore;
		inputFile.close();
	}

	Text hiScoreText;													// hi score text
	hiScoreText.setFont(font);
	hiScoreText.setCharacterSize(50);
	hiScoreText.setFillColor(Color::White);
	stringstream s;
	s << "Hi score: " << hiScore;
	hiScoreText.setString(s.str());
	FloatRect hiScoreTextRect = hiScoreText.getLocalBounds();
	hiScoreText.setOrigin(hiScoreTextRect.width, hiScoreTextRect.top);
	hiScoreText.setPosition(resolution.x - 20, 20);
	

	Text zombiesRemainingText;											// zombies remaining text
	zombiesRemainingText.setFont(font);
	zombiesRemainingText.setCharacterSize(50);
	zombiesRemainingText.setFillColor(Color::White);
	zombiesRemainingText.setPosition(resolution.x - 250, resolution.y - 100);
	zombiesRemainingText.setString("Zombies: 100");

	int wave = 0;														// wave number
	Text waveNumberText;
	waveNumberText.setFont(font);
	waveNumberText.setCharacterSize(50);
	waveNumberText.setFillColor(Color::White);
	waveNumberText.setPosition(resolution.x - 430, resolution.y - 100);
	waveNumberText.setString("Wave: 0");

	RectangleShape healthBar;											// health bar
	healthBar.setFillColor(Color::Red);
	healthBar.setPosition(resolution.x / 2 - 150, resolution.y - 120);

	int framesSinceLastHUDUpdate = 0;									// updating the HUD timers
	int fpsMeasurmentFrameInterval = 1000;

	// Sound works
	SoundBuffer hitBuffer;												// hit sound
	hitBuffer.loadFromFile("sound/hit.wav");
	Sound hit;
	hit.setBuffer(hitBuffer);

	SoundBuffer splatBuffer;											// splat sound
	splatBuffer.loadFromFile("sound/splat.wav");
	Sound splat;
	splat.setBuffer(splatBuffer);

	SoundBuffer shootBuffer;											// shoot sound
	shootBuffer.loadFromFile("sound/shoot.wav");
	Sound shoot;
	shoot.setBuffer(shootBuffer);

	SoundBuffer shootFailedBuffer;										// shoot failed sound
	shootFailedBuffer.loadFromFile("sound/shoot_failed.wav");
	Sound shootFailed;
	shootFailed.setBuffer(shootFailedBuffer);

	SoundBuffer reloadBuffer;											// reload sound
	reloadBuffer.loadFromFile("sound/reload.wav");
	Sound reload;
	reload.setBuffer(reloadBuffer);

	SoundBuffer reloadFailedBuffer;										// failed reload sound
	reloadFailedBuffer.loadFromFile("sound/reload_failed.wav");
	Sound reloadFailed;
	reloadFailed.setBuffer(reloadFailedBuffer);

	SoundBuffer levelupBuffer;											// level up sound
	levelupBuffer.loadFromFile("sound/levelup1.wav");
	Sound levelup;
	levelup.setBuffer(levelupBuffer);

	SoundBuffer pickupSpawnBuffer;										// pickup spawn sound
	pickupSpawnBuffer.loadFromFile("sound/pickup_spawn.wav");
	ammoPickup.m_SpawnSound.setBuffer(pickupSpawnBuffer);
	healthPickup.m_SpawnSound.setBuffer(pickupSpawnBuffer);

	SoundBuffer pickupBuffer;											// hp pickup sound
	pickupBuffer.loadFromFile("sound/pickup.wav");
	Sound pickup;
	pickup.setBuffer(pickupBuffer);

	SoundBuffer pickup2Buffer;											// ammo pickup sound
	pickup2Buffer.loadFromFile("sound/pickup2.wav");
	Sound pickup2;
	pickup2.setBuffer(pickup2Buffer);

	SoundBuffer deathBuffer;											// death sound
	deathBuffer.loadFromFile("sound/death.wav");
	Sound death;
	death.setBuffer(deathBuffer);

	SoundBuffer roarBuffer1;											// roar sounds
	roarBuffer1.loadFromFile("sound/roar1.wav");
	SoundBuffer roarBuffer2;
	roarBuffer2.loadFromFile("sound/roar2.wav");
	SoundBuffer roarBuffer3;
	roarBuffer3.loadFromFile("sound/roar3.wav");
	Sound roar1;
	roar1.setBuffer(roarBuffer1);
	Sound roar2;
	roar2.setBuffer(roarBuffer2);
	Sound roar3;
	roar3.setBuffer(roarBuffer3);

	SoundBuffer menuMusicBuffer;										// menu music
	menuMusicBuffer.loadFromFile("sound/Approaching_Zombies.wav");
	Sound menuMusic;
	menuMusic.setBuffer(menuMusicBuffer);
	menuMusic.setVolume(70);
	menuMusic.setLoop(true);
	bool menuMusicPlayed = false;

	SoundBuffer gameMusicBuffer;										// game music
	gameMusicBuffer.loadFromFile("sound/Bad_Ways.wav");
	Sound gameMusic;
	gameMusic.setBuffer(gameMusicBuffer);
	gameMusic.setVolume(70);
	gameMusic.setLoop(true);
	bool gameMusicPlayed = false;

	Sound* currentMusic = nullptr;
	bool* currentMusicPlayed = nullptr;
	int volumeSFX;
	int volumeMusic;
	int muteBuffer;

	// Main game loop
	while (window.isOpen())
	{
		// *******************
		// Handle input
		// *******************

		Event event;													// handle events
		while (window.pollEvent(event))
		{
			if (event.type == Event::KeyPressed)
			{
				if (event.key.code == Keyboard::Escape && state == State::PLAYING)
				{
					menuMusic.play();
					menuMusicPlayed = true;
					currentMusic = &menuMusic;
					currentMusicPlayed = &menuMusicPlayed;
					gameMusic.pause();
					gameMusicPlayed = false;

					state = State::PAUSE;								// pause while playing
				}
				else if (event.key.code == Keyboard::Escape && state == State::PAUSE)
				{
					gameMusic.play();
					gameMusicPlayed = true;
					currentMusic = &gameMusic;
					currentMusicPlayed = &gameMusicPlayed;
					menuMusic.pause();
					menuMusicPlayed = false;

					state = State::PLAYING;								// play while paused
					clock.restart();
				}
				else if (event.key.code == Keyboard::Return && state == State::PAUSE)
				{
					state = State::CONTROLS;							// controls from pause
				}
				else if (event.key.code == Keyboard::Return && state == State::CONTROLS)
				{
					state = State::PAUSE;
				}
				else if (event.key.code == Keyboard::Return && state == State::GAME_OVER)
				{
					state = State::LEVELING_UP;							// restart while game over
					wave = 0;											// reset stats
					score = 0;
					currentBullet = 0;
					bulletsSpare = 30;
					bulletsInClip = 6;
					clipSize = 6;
					fireRate = 1;

					player.resetPlayerStats();
				}
				if (state == State::PLAYING)
				{

					// RELOADING
					if (event.key.code == Keyboard::R && !player.m_Reload)
					{
						if (bulletsInClip == clipSize)
						{
							reloadFailed.play();						// reloading
						}
						else
						{
							lastReload = gameTimeTotal;
							reload.play();
							player.m_Reload = true;
						}
					}
				}

				// HOTKEYS HANDLE
				if (event.key.code == Keyboard::M)					// mute music
				{
					if (*currentMusicPlayed)
					{
						muteBuffer = currentMusic->getVolume();
						currentMusic->setVolume(0);
						*currentMusicPlayed = false;
					}
					else
					{
						currentMusic->setVolume(muteBuffer);
						*currentMusicPlayed = true;
						muteBuffer = 0;
					}
				}
				if (event.key.code == Keyboard::Add && gameMusic.getVolume() != 100)	// + music volume
				{
					gameMusic.setVolume(gameMusic.getVolume() + 10);
					menuMusic.setVolume(menuMusic.getVolume() + 10);
				}
				if (event.key.code == Keyboard::Subtract && gameMusic.getVolume() != 0)	// - music volume
				{
					gameMusic.setVolume(gameMusic.getVolume() - 10);
					menuMusic.setVolume(menuMusic.getVolume() - 10);
				}
				if (event.key.code == Keyboard::Num0 && roar1.getVolume() != 100)		// + sfx volume
				{
					roar2.setVolume(roar1.getVolume() + 10);
					roar3.setVolume(roar1.getVolume() + 10);
					pickup2.setVolume(roar1.getVolume() + 10);
					pickup.setVolume(roar1.getVolume() + 10);
					death.setVolume(roar1.getVolume() + 10);
					ammoPickup.m_SpawnSound.setVolume(roar1.getVolume() + 10);
					healthPickup.m_SpawnSound.setVolume(roar1.getVolume() + 10);
					levelup.setVolume(roar1.getVolume() + 10);
					reloadFailed.setVolume(roar1.getVolume() + 10);
					reload.setVolume(roar1.getVolume() + 10);
					shootFailed.setVolume(roar1.getVolume() + 10);
					shoot.setVolume(roar1.getVolume() + 10);
					splat.setVolume(roar1.getVolume() + 10);
					hit.setVolume(roar1.getVolume() + 10);
					roar1.setVolume(roar1.getVolume() + 10);
				}
				if (event.key.code == Keyboard::Num9 && roar1.getVolume() != 0)			// - sfx volume
				{
					roar2.setVolume(roar1.getVolume() - 10);
					roar3.setVolume(roar1.getVolume() - 10);
					pickup2.setVolume(roar1.getVolume() - 10);
					pickup.setVolume(roar1.getVolume() - 10);
					death.setVolume(roar1.getVolume() - 10);
					ammoPickup.m_SpawnSound.setVolume(roar1.getVolume() - 10);
					healthPickup.m_SpawnSound.setVolume(roar1.getVolume() - 10);
					levelup.setVolume(roar1.getVolume() - 10);
					reloadFailed.setVolume(roar1.getVolume() - 10);
					reload.setVolume(roar1.getVolume() - 10);
					shootFailed.setVolume(roar1.getVolume() - 10);
					shoot.setVolume(roar1.getVolume() - 10);
					splat.setVolume(roar1.getVolume() - 10);
					hit.setVolume(roar1.getVolume() - 10);
					roar1.setVolume(roar1.getVolume() - 10);
				}
			}
		}// end of handle events

		if (Keyboard::isKeyPressed(Keyboard::F12))
		{
			window.close();
		}

		// control handle
		if (state == State::PLAYING)				
		{
			if (Keyboard::isKeyPressed(Keyboard::W))
			{
				player.moveUp();
			}
			else
			{
				player.stopUp();
			}

			if (Keyboard::isKeyPressed(Keyboard::S))
			{
				player.moveDown();
			}
			else
			{
				player.stopDown();
			}

			if (Keyboard::isKeyPressed(Keyboard::A))
			{
				player.moveLeft();
			}
			else
			{
				player.stopLeft();
			}

			if (Keyboard::isKeyPressed(Keyboard::D))
			{
				player.moveRight();
			}
			else
			{
				player.stopRight();
			}

			if (Mouse::isButtonPressed(Mouse::Left)) //(Keyboard::isKeyPressed(Keyboard::F))
			{
				if (gameTimeTotal.asMilliseconds() - lastPressed.asMilliseconds() > 1000 / fireRate && bulletsInClip > 0 && !player.m_Reload)
				{
					bullets[currentBullet].shoot(player.getCenter().x, player.getCenter().y, mouseWorldPosition.x, mouseWorldPosition.y);
					currentBullet++;
					if (currentBullet > 99)
					{
						currentBullet = 0;
					}
					lastPressed = gameTimeTotal;
					bulletsInClip--;
					shoot.play();
				}
				else if (gameTimeTotal.asMilliseconds() - lastPressed.asMilliseconds() > 1000 / fireRate && bulletsInClip == 0 && !player.m_Reload)
				{
					lastPressed = gameTimeTotal;
					shootFailed.play();
				}
			}
		}//end control handle 

		if (state == State::GAME_OVER)						// handle game over state
		{
			if (!menuMusicPlayed)							// menu music
			{
				menuMusic.play();
				menuMusicPlayed = true;
				currentMusic = &menuMusic;
				currentMusicPlayed = &menuMusicPlayed;
			}
		}//end of handle game over state

		if (state == State::STAGE_CLEAR)					// handle stage slear state
		{
			currentMusic = nullptr;
			currentMusic = nullptr;
			if (menuMusicPlayed)
			{
				menuMusic.stop();
				menuMusicPlayed = false;
			}
			if (gameMusicPlayed)
			{
				gameMusic.stop();
				gameMusicPlayed = false;
			}
			if (Keyboard::isKeyPressed(Keyboard::Enter))
			{
				state = State::LEVELING_UP;
			}
		}//end of handle stage clear state

		// handle leveling up state
		if (state == State::LEVELING_UP)
		{
			if (event.key.code == Keyboard::Num1)			// increase fire rate
			{
				fireRate++;
				state = State::PLAYING;
			}
			if (event.key.code == Keyboard::Num2)			// increase clip size
			{
				clipSize += clipSize;
				state = State::PLAYING;
			}
			if (event.key.code == Keyboard::Num3)			// increase hp
			{
				player.upgradeHealth();
				state = State::PLAYING;
			}
			if (event.key.code == Keyboard::Num4)			// increase speed
			{
				player.upgradeSpeed();
				state = State::PLAYING;
			}
			if (event.key.code == Keyboard::Num5)			// upgrade hp pickup
			{
				healthPickup.upgrade();
				state = State::PLAYING;
			}
			if (event.key.code == Keyboard::Num6)			// upgrade ammo pickup
			{
				ammoPickup.upgrade();
				state = State::PLAYING;
			}

			if (state == State::PLAYING)
			{
				if (!gameMusicPlayed)
				{
					gameMusic.play();
					gameMusicPlayed = true;
					currentMusic = &gameMusic;
					currentMusicPlayed = &gameMusicPlayed;
					menuMusic.stop();
					menuMusicPlayed = false;
				}
				wave++;
				arena.width = 600 + (wave * 100);										// prepare the arena
				arena.height = 600 + (wave * 100);
				arena.left = 0;
				arena.top = 0;
				int tileSize = createBackground(background, arena);
				player.spawn(arena, resolution, tileSize);				// spawn player
				healthPickup.setArena(arena);							// configure pickups
				ammoPickup.setArena(arena);
				numZombies = 10 + wave * wave * 2;
				delete[] zombies;
				zombies = createHorde(numZombies, arena);
				numZombiesAlive = numZombies;
				clock.restart();
			}
			
		}// end leveling up


		// *******************
		// Update the frame
		// *******************

		if (state == State::PLAYING)
		{
			Time dt = clock.restart();									// update delta time
			gameTimeTotal += dt;										// update total game time
			float dtAsSeconds = dt.asSeconds();							// make a decimal fraction of 1 from dt

			mouseScreenPosition = Mouse::getPosition();					// mouse pointer
			mouseWorldPosition = window.mapPixelToCoords(				// mouse position to world coordinates
				Mouse::getPosition(), mainView);
			spriteCrosshair.setPosition(mouseWorldPosition);			// set image of crosshair to mouse position

			player.update(dtAsSeconds, Mouse::getPosition());			// update player
			Vector2f playerPosition(player.getCenter());				// note of players new position
			mainView.setCenter(player.getCenter());						// make view aroud the player

			for (int i = 0; i < numZombies; i++)						// zombies update
			{
				if (zombies[i].isAlive())
				{
					zombies[i].update(dt.asSeconds(), playerPosition);
				}
			}

			for (int i = 0; i < 100; i++)								// bullets update
			{
				if (bullets[i].isInFlight())
				{
					bullets[i].update(dtAsSeconds);
				}
			}

			healthPickup.update(dtAsSeconds);							// pickup update
			ammoPickup.update(dtAsSeconds);

			for (int i = 0; i < 100; i++)								// zombie-bullet collision detect
			{
				for (int j = 0; j < numZombies; j++)
				{
					if (bullets[i].isInFlight() && zombies[j].isAlive())
					{
						if (bullets[i].getPosition().intersects(zombies[j].getPosition()))
						{
							bullets[i].stop();
							if (zombies[j].hit())
							{
								score += 10;
								if (score >= hiScore)
								{
									hiScore = score;
								}
								numZombiesAlive--;
								if (numZombiesAlive == 0)
								{
									levelup.play();
									state = State::STAGE_CLEAR;
								}
							}
							splat.play();
						}
					}
				}
			}//end of zombie shot

			for (int i = 0; i < numZombies; i++)						// player-zombie collision detect
			{
				if (zombies[i].isAlive() && player.getPosition().intersects(zombies[i].getPosition()))
				{
					if (player.hit(gameTimeTotal))
					{
						hit.play();
					}

					if (player.getHealth() <= 0)
					{
						death.play();
						menuMusic.stop();
						menuMusicPlayed = false;
						gameMusic.stop();
						gameMusicPlayed = false;
						state = State::GAME_OVER;
						ofstream outputFile("gamedata/scores.txt");
						outputFile << hiScore;
						outputFile.close();
					}
				}
			}//end of player touched

			if (healthPickup.isSpawned() &&								// player-pickup collision
				player.getPosition().intersects(healthPickup.getPosition()))
			{
				player.increaseHealthLevel(healthPickup.gotIt());
				pickup.play();
			}

			if (ammoPickup.isSpawned() && player.getPosition().intersects(ammoPickup.getPosition()))
			{
				bulletsSpare += ammoPickup.gotIt();
				pickup2.play();
			}// end of pickup touched

			healthBar.setSize(Vector2f(player.getHealth() * 3, 70));	// health bar update
			hiScoreText.setPosition(resolution.x - 20, 20);

			framesSinceLastHUDUpdate++;
			if (framesSinceLastHUDUpdate > fpsMeasurmentFrameInterval)
			{
				stringstream ssAmmo;									// create streams for update
				stringstream ssScore;
				stringstream ssHiScore;
				stringstream ssWave;
				stringstream ssZombiesAlive;
				
				ssAmmo << bulletsInClip << "/" << bulletsSpare;			// update ammo
				ammoText.setString(ssAmmo.str());

				ssScore << "Score: " << score;							// update score
				scoreText.setString(ssScore.str());

				ssHiScore << "Hi Score: " << hiScore;					// update hi score
				hiScoreText.setString(ssHiScore.str());

				ssWave << "Wave: " << wave;								// update wave
				waveNumberText.setString(ssWave.str());

				ssZombiesAlive << "Zombies: " << numZombiesAlive;		// update alive zombies
				zombiesRemainingText.setString(ssZombiesAlive.str());

				framesSinceLastHUDUpdate = 0;
			}//end of HUD update

			for (int i = 0; i < numZombies; i++)						// zombie roar
			{
				srand((int)time(0)* i);
				int chance = rand() % 30;
				if (zombies[i].isAlive() && chance == 1)
				{
					switch (rand() % 3)
					{
					case 0:
						roar1.play();
						break;
					case 1:
						roar2.play();
						break;
					case 2:
						roar3.play();
						break;
					}
				}
			}//end of zombie roar

			if (player.m_Reload && gameTimeTotal.asMilliseconds() - lastReload.asMilliseconds() > 1500)
			{
				if (bulletsSpare >= bulletsInClip)
				{
					bulletsInClip = clipSize;
					bulletsSpare -= clipSize;
				}
				else if (bulletsSpare > 0)
				{
					bulletsInClip = bulletsSpare;
					bulletsSpare = 0;
				}
				player.m_Reload = false;
			}

			if (healthPickup.isSpawned())
			{
				spriteRedArrow.setPosition(player.getCenter());
				float angleRedArrow = (atan2(healthPickup.getCenter().y + 50 - player.getCenter().y, healthPickup.getCenter().x + 50 - player.getCenter().x) * 180) / 3.141;
				spriteRedArrow.setRotation(angleRedArrow);
			}
			if (ammoPickup.isSpawned())
			{
				spriteYellowArrow.setPosition(player.getCenter());
				float angleYellowArrow = (atan2(ammoPickup.getCenter().y + 50 - player.getCenter().y, ammoPickup.getCenter().x + 50 - player.getCenter().x) * 180) / 3.141;
				spriteYellowArrow.setRotation(angleYellowArrow);
			}
		}//end of scene update

		// *********************
		// Draw the scene
		// *********************

		if (state == State::PLAYING)
		{
			window.clear();
			window.setView(mainView);									// set the mainView to be displayed in the window and draw it
			window.draw(background, &textureBackground);
			
			for (int i = 0; i < numZombies; i++)						// draw zombies
			{
				window.draw(zombies[i].getSprite());
			}

			window.draw(player.getSprite());

			if (ammoPickup.isSpawned())									// draw pickups
			{
				window.draw(ammoPickup.getSprite());
				window.draw(spriteYellowArrow);
			}
			if (healthPickup.isSpawned())
			{
				window.draw(healthPickup.getSprite());
				window.draw(spriteRedArrow);
			}
			for (int i = 0; i < 100; i++)
			{
				if (bullets[i].isInFlight())
				{
					window.draw(bullets[i].getShape());
				}
			}
			window.draw(spriteCrosshair);

			window.setView(hudView);
			window.draw(spriteAmmoIcon);
			window.draw(ammoText);
			window.draw(scoreText);
			window.draw(hiScoreText);
			window.draw(healthBar);
			window.draw(waveNumberText);
			window.draw(zombiesRemainingText);
		}
		if (state == State::LEVELING_UP)
		{
			window.draw(spriteGameOver);
			window.draw(levelUpText);
		}
		if (state == State::PAUSE)
		{
			window.clear();
			window.setView(mainView);									// set the mainView to be displayed in the window and draw it
			window.draw(background, &textureBackground);

			for (int i = 0; i < numZombies; i++)						// draw zombies
			{
				window.draw(zombies[i].getSprite());
			}

			window.draw(player.getSprite());

			if (ammoPickup.isSpawned())									// draw pickups
			{
				window.draw(ammoPickup.getSprite());
			}
			if (healthPickup.isSpawned())
			{
				window.draw(healthPickup.getSprite());
			}
			for (int i = 0; i < 100; i++)
			{
				if (bullets[i].isInFlight())
				{
					window.draw(bullets[i].getShape());
				}
			}

			window.setView(hudView);
			window.draw(hiScoreText);

			window.draw(pausedText);
			window.draw(seeControlsText);
		}
		if (state == State::GAME_OVER)
		{
			window.draw(spriteGameOver);
			window.draw(gameOverText);
			window.draw(scoreText);
			window.draw(hiScoreText);
		}
		if (state == State::STAGE_CLEAR)
		{
			window.draw(spriteGameOver);
			window.draw(stageClearText);
			window.draw(stageClear2Text);
		}
		if (state == State::CONTROLS)
		{
			window.draw(spriteGameOver);
			window.draw(unseeControlsText);
			window.draw(controlsText);
		}

		window.display();

	}//end of game loop

	delete[] zombies;
	return 0;
}