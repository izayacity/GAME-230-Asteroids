#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <string>
#include "../include/Gameplay.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <assert.h>

int Gameplay::init () {
	std::srand (static_cast<unsigned int>(std::time (NULL)));
	window.setFramerateLimit (60); // call it once, after creating the window
	
	assert (thrust_sound_buffer.loadFromFile ("resources/thrust.wav"));
	thrust_sound.setBuffer (thrust_sound_buffer);

	// Load the text font
	if (!fontHNMed.loadFromFile ("resources/HelveticaNeueMed.ttf"))
		return EXIT_FAILURE;
	if (!fontHNM.loadFromFile ("resources/HelveticaNeue Medium.ttf"))
		return EXIT_FAILURE;
	if (!fontHNL.loadFromFile ("resources/HelveticaNeue Light.ttf"))
		return EXIT_FAILURE;

	// Life text
	lifeText.setFont (fontHNMed);
	lifeText.setCharacterSize (30);
	lifeText.setOutlineThickness (1);
	lifeText.setOutlineColor (sf::Color::White);
	lifeText.setFillColor (sf::Color::Red);
	lifeText.setString ("Life: 3");
	lifeText.setPosition (10.f, gameHeight - 40.f);

	// Score text
	score.setFont (fontHNL);
	score.setCharacterSize (30);
	score.setFillColor (sf::Color (239, 187, 56));

	// Welcome background
	bgTex.loadFromFile ("resources/bg.jpg");
	sf::Vector2f sz ((float)window.getSize ().x, (float)window.getSize ().y);
	shape.setSize (sz);
	shape.setTexture (&bgTex);
	assert (largeTexture.loadFromFile ("resources/large.png"));
	largeTexture.setSmooth (true);
	assert (mediumTexture.loadFromFile ("resources/medium.png"));
	mediumTexture.setSmooth (true);
	assert (smallTexture.loadFromFile ("resources/small.png"));
	smallTexture.setSmooth (true);

	// Select game mode
	gameState = selectMode (window);
	//restart ();

	// Game loop window
	while (window.isOpen ()) {
		while (window.pollEvent (event)) {
			if ((event.type == sf::Event::Closed) ||
				((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))) {
				gameState = selectMode (window);  //To be made as resumeWindow ()
				//restart ();
				if (gameState == -1)
					return EXIT_SUCCESS;
				break;
			}

			if (sf::Keyboard::isKeyPressed (sf::Keyboard::Space)) {				
				lazers.push_back (Lazer ());
				if (!lazers.empty () && shotClock.getElapsedTime().asSeconds() >= 0.25f) {
					lazers.back ().rect.setRotation (ship.sprite.getRotation ());
					lazers.back ().rect.setPosition (ship.sprite.getPosition ());
				}
				shotClock.restart ();
			}
			/*if (gameState == MODE1 && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
				gameState = RESUME1;
			}*/
		}

		deltaTime = clock.restart ().asSeconds ();

		if (gameState == MODE1) {
			gameMode1 ();
		}

		renderFrame ();
		window.display ();
	}

	return EXIT_SUCCESS;
}

void Gameplay::renderFrame () {
	// Win text
	sf::Text wonText ("You won!\nPress esc to menu.", fontHNM, 40);
	wonText.setPosition (gameWidth / 2 - wonText.getGlobalBounds ().width / 2, gameHeight / 2 - wonText.getGlobalBounds ().height / 2);
	wonText.setFillColor (sf::Color::White);

	// Lost text
	sf::Text lostText ("You lost!\nPress esc to menu.", fontHNM, 40);
	lostText.setPosition (gameWidth / 2 - lostText.getGlobalBounds ().width / 2, gameHeight / 2 - lostText.getGlobalBounds ().height / 2);
	lostText.setFillColor (sf::Color::White);

	window.clear ();
	window.draw (shape);
	window.draw (lifeText);
	window.draw (ship.sprite);

	if (gameState == MODE1 || gameState == RESUME1) {
		int i = 0;
		for (std::vector<Lazer>::iterator it = lazers.begin (); it != lazers.end (); ++it) {
			it->rect.move (sin (it->rect.getRotation () / 360 * 2 * pi) * it->velocity, -cos (it->rect.getRotation () / 360 * 2 * pi) * it->velocity);
			window.draw (it->rect);
		}
		for (std::vector<Asteroid>::iterator it = asteroids.begin (); it != asteroids.end (); ++it) {
			it->sprite.move (sin (it->sprite.getRotation () / 360 * 2 * pi) * it->velocity, -cos (it->sprite.getRotation () / 360 * 2 * pi) * it->velocity);
			window.draw (it->sprite);
		}
	} else if (gameState == P1LOST) {
		window.draw (lostText);
	} else if (gameState == P1WIN) {
		window.draw (wonText);
	}
}

int Gameplay::selectMode (sf::RenderWindow& window) {
	life = 3;

	// Welcome text
	sf::Text welcome[5];
	std::string msg[5] = { "Press 1 - You VS Ai", "2 - You + Friend VS 2 Ai" , "Player1 uses WSAD", "Player2 uses Arrow Keys.", "Press Esc to exit." };
	for (int i = 0; i < 5; i++) {
		welcome[i].setPosition (50.0f, 50.0f * (i + 1));
		welcome[i].setFillColor (sf::Color (239, 187, 56));
		welcome[i].setCharacterSize (30);
		welcome[i].setString (msg[i]);
		welcome[i].setFont (fontHNMed);
	}

	// Main loop
	while (true) {
		sf::Event event;
		while (window.pollEvent (event)) {
			if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
				window.close ();
				return -1;
			}

			if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Num1) {
				gameState = MODE1;
				//level1 ();
				window.clear ();
				shotClock.restart ();
				return gameState;
			} else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Num2) {
				gameState = MODE2;
				window.clear ();
				shotClock.restart ();
				return gameState;
			}
		}
		window.clear ();
		// Put text and wait for select
		window.draw (shape);
		for (int i = 0; i < 5; i++)
			window.draw (welcome[i]);

		window.display ();
	}
}

int Gameplay::gameMode1 () {
	astTime = astClock.getElapsedTime().asSeconds();
	if (astTime >= 1 && asteroids.size() <= 10) {
		astSpawn ();
		astClock.restart ();
	}

	if (ship.velocity > 0.f) {
		ship.velocity -= deltaTime;
		if (isUp == 1) {
			ship.sprite.move (sin (ship.sprite.getRotation () / 360 * 2 * pi) * ship.velocity, -cos (ship.sprite.getRotation () / 360 * 2 * pi) * ship.velocity);
		} else if (isUp == -1) {
			ship.sprite.move (-sin (ship.sprite.getRotation () / 360 * 2 * pi) * ship.velocity, cos (ship.sprite.getRotation () / 360 * 2 * pi) * ship.velocity);
		}
	} else if (ship.velocity < 0.f) {
		ship.velocity = 0.f;
		isUp = 0;
	}

	// Move the player's ship
	if (sf::Keyboard::isKeyPressed (sf::Keyboard::Up)) {
		isUp = 1;
		ship.velocity = ship.acceleration * deltaTime;
		ship.sprite.move (sin (ship.sprite.getRotation () / 360 * 2 * pi) * ship.velocity, -cos (ship.sprite.getRotation () / 360 * 2 * pi) * ship.velocity);
	}

	if (sf::Keyboard::isKeyPressed (sf::Keyboard::Down)) {
		thrust_sound.play ();
		isUp = -1;
		ship.velocity = ship.acceleration * deltaTime;
		ship.sprite.move (-sin (ship.sprite.getRotation () / 360 * 2 * pi) * ship.velocity, cos (ship.sprite.getRotation () / 360 * 2 * pi) * ship.velocity);		
	}

	if (sf::Keyboard::isKeyPressed (sf::Keyboard::Left)) {
		ship.sprite.rotate(- ship.angular_velocity * deltaTime);
	}

	if (sf::Keyboard::isKeyPressed (sf::Keyboard::Right)) {
		ship.sprite.rotate (ship.angular_velocity * deltaTime);
	}
	
	if (!lazers.empty () && (lazers.front ().rect.getPosition ().x < 0 ||
		lazers.front ().rect.getPosition ().x > gameWidth ||
		lazers.front ().rect.getPosition ().y < 0 ||
		lazers.front ().rect.getPosition ().y > gameHeight)) {
		lazers.erase(lazers.begin());
	}

	if (!asteroids.empty () && (asteroids.front ().sprite.getPosition ().x < 0 ||
		asteroids.front ().sprite.getPosition ().x > gameWidth ||
		asteroids.front ().sprite.getPosition ().y < 0 ||
		asteroids.front ().sprite.getPosition ().y > gameHeight)) {
		asteroids.erase (asteroids.begin ());
	}

	return gameState;
}

void Gameplay::astSpawn () {
	float pos_x, pos_y, angle;
	int type = rand () % 3;

	angle = rand () % 360 + 0.f;
	if (angle >= 0 && angle < 45) {
		pos_x = rand () % (gameWidth / 2) + 0.f;
		pos_y = gameHeight + 0.f;
		angle = atan ((gameWidth / 2 - pos_x) / (gameHeight / 2)) / pi * 180.f;
	} else if (angle >= 45 && angle < 90) {
		pos_x = 0.f;
		pos_y = rand () % (gameHeight / 2) + gameHeight / 2.f;
		angle = atan ((gameWidth / 2) / (pos_y - gameHeight / 2)) / pi * 180.f;
	} else if (angle >= 90 && angle < 135) {
		pos_x = 0.f;
		pos_y = rand () % (gameHeight / 2) + 0.f;
		angle = 180.f - atan ((gameWidth / 2) / (gameHeight / 2 - pos_y)) / pi * 180.f;
	} else if (angle >= 135 && angle < 180) {
		pos_x = rand () % (gameWidth / 2) + 0.f;
		pos_y = 0.f;
		angle = 180.f - atan ((gameWidth / 2 - pos_x) / (gameHeight / 2)) / pi * 180.f;
	} else if (angle >= 180 && angle < 225) {
		pos_x = rand () % (gameWidth / 2) + gameWidth / 2.f;
		pos_y = 0.f;
		angle = atan ((pos_x - gameWidth / 2) / (gameHeight / 2)) / pi * 180.f + 180.f;
	} else if (angle >= 225 && angle < 270) {
		pos_x = gameWidth + 0.f;
		pos_y = rand () % (gameHeight / 2) + 0.f;
		angle = atan ((gameWidth / 2) / (gameHeight / 2 - pos_y)) / pi * 180.f + 180.f;
	} else if (angle >= 270 && angle < 315) {
		pos_x = gameWidth + 0.f;
		pos_y = rand () % (gameHeight / 2) + gameHeight / 2.f;
		angle = 360.f - atan ((gameWidth / 2) / (pos_y - gameHeight / 2)) / pi * 180.f;
	} else if (angle >= 315 && angle < 360) {
		pos_x = rand () % (gameWidth / 2) + gameWidth / 2.f;
		pos_y = gameHeight + 0.f;
		angle = 360.f - atan ((pos_x - gameWidth / 2) / (gameHeight / 2)) / pi * 180.f;
	}

	asteroids.push_back (Asteroid());

	if (type == 0) {
		asteroids.back ().small (&smallTexture);
	} else if (type == 1) {
		asteroids.back ().medium (&mediumTexture);
	} else if (type == 2) {
		asteroids.back ().large (&largeTexture);
	}
	asteroids.back ().sprite.setRotation (angle);
	asteroids.back ().sprite.setPosition (pos_x, pos_y);
}

bool Gameplay::CircleTest (sf::CircleShape Object1, sf::CircleShape Object2) {
	sf::Vector2f Distance = Object1.getOrigin () - Object2.getOrigin ();
	return (Distance.x * Distance.x + Distance.y * Distance.y <= (Object1.getRadius() + Object2.getRadius ()) * (Object1.getRadius () + Object2.getRadius ()));
}