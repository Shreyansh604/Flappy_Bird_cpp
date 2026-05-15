#include "Game.h"
#include "Globals.h"
#include <sstream>

Game::Game(sf::RenderWindow& window) : 
	win(window), 
	is_enter_pressed(false), 
	run_game(true),
	pipe_counter(71),
	pipe_spawn_time(70),
	start_monitoring(false),
	score(0)
{
	win.setFramerateLimit(60);
	bg_texture.loadFromFile("assets/bg.png");
	bg_sprite.setTexture(bg_texture);
	bg_sprite.setScale(sf::Vector2f(SCALE_FACTOR, SCALE_FACTOR));
	bg_sprite.setPosition(sf::Vector2f(0.f, -250.f));

	ground_texture.loadFromFile("assets/ground.png");
	ground_sprite1.setTexture(ground_texture);
	ground_sprite2.setTexture(ground_texture);

	ground_sprite1.setScale(sf::Vector2f(SCALE_FACTOR, SCALE_FACTOR));
	ground_sprite2.setScale(sf::Vector2f(SCALE_FACTOR, SCALE_FACTOR));

	ground_sprite1.setPosition(sf::Vector2f(0.f, 578.f));
	ground_sprite2.setPosition(sf::Vector2f(ground_sprite1.getGlobalBounds().width, 578.f));

	font.loadFromFile("assets/arial.ttf");
	restart_text.setFont(font);
	restart_text.setCharacterSize(48);
	restart_text.setFillColor(sf::Color::Black);
	restart_text.setPosition(sf::Vector2f(150, 650));
	restart_text.setString("Restart Game!!");

	score_text.setFont(font);
	score_text.setCharacterSize(24);
	score_text.setFillColor(sf::Color::Black);
	score_text.setPosition(sf::Vector2f(15, 15));
	score_text.setString("Score: 0");

	Pipe::loadTextures();
}

void Game::startGameLoop()
{
	//Game loop
	while (win.isOpen())
	{
		sf::Time dt = clock.restart();
		sf::Event event;
		//Event loop
		while (win.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				win.close();
			}
			if (event.type == sf::Event::KeyPressed && run_game)
			{
				if (event.key.code == sf::Keyboard::Enter && !is_enter_pressed)
				{
					is_enter_pressed = true;
					bird.setShouldFly(true);
				}
				if (event.key.code == sf::Keyboard::Space && is_enter_pressed)
				{
					bird.flapBird(dt);
				}
			}
			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left && !run_game)
			{
				if (restart_text.getGlobalBounds().contains(sf::Vector2f(event.mouseButton.x, event.mouseButton.y)))
				{
					restartGame();
				}
			}
		}

		doProcessing(dt);

		draw();

		//displat the window
		win.display();
	}
}

void Game::draw()
{
	win.draw(bg_sprite);
	for (Pipe& pipe : pipes)
	{
		win.draw(pipe.sprite_down);
		win.draw(pipe.sprite_up);
	}
	win.draw(ground_sprite1);
	win.draw(ground_sprite2);
	win.draw(bird.bird_sprite);
	win.draw(score_text);

	if (!run_game)
	{
		win.draw(restart_text);
	}
}

void Game::moveGround(sf::Time& dt)
{
	ground_sprite1.move(-moveSpeed * dt.asSeconds(), 0.f);
	ground_sprite2.move(-moveSpeed * dt.asSeconds(), 0.f);

	float width = ground_sprite1.getGlobalBounds().width;
	
	// Position set for ground_sprite1 to start(x coordinates) where ground_sprite2 ends(y coordinates) 
	if (ground_sprite1.getGlobalBounds().left + width < 0)
	{
		float newX = ground_sprite2.getPosition().x + width;
		ground_sprite1.setPosition(newX, ground_sprite1.getPosition().y);
	}
	// Position set for ground_sprite2 to start(x coordinates) where ground_sprite1 ends(y coordinates)
	if (ground_sprite2.getGlobalBounds().left + width < 0)
	{
		float newX = ground_sprite1.getPosition().x + width;
		ground_sprite2.setPosition(newX, ground_sprite2.getPosition().y);
	}
}

void Game::doProcessing(sf::Time& dt)
{
	if (is_enter_pressed)
	{
		moveGround(dt);
		if (pipe_counter > pipe_spawn_time)
		{
			pipes.push_back(Pipe(dist(rd)));
			pipe_counter = 0;
		}
		pipe_counter++;

		for (int i = 0; i < pipes.size(); i++)
		{
			pipes[i].update(dt);
			if (pipes[i].getRightBound() < 0)
			{
				pipes.erase(pipes.begin() + i);
			}
		}
		checkCollisions();
		checkScore();
	}
	bird.update(dt);
}

void Game::checkCollisions()
{
	if (pipes.size() > 0)
	{
		if (pipes[0].sprite_down.getGlobalBounds().intersects(bird.bird_sprite.getGlobalBounds()) || pipes[0].sprite_up.getGlobalBounds().intersects(bird.bird_sprite.getGlobalBounds()) ||
			bird.bird_sprite.getGlobalBounds().top >= 540)
		{
			is_enter_pressed = false;
			run_game = false;
		}
	}
}

void Game::restartGame()
{
	bird.resetBirdPostion();
	bird.setShouldFly(false);
	run_game = true;
	is_enter_pressed = false;
	pipe_counter = 71;
	pipes.clear();
	score = 0;
	score_text.setString("Score: 0");
}

void Game::checkScore()
{
	if (pipes.size() > 0)
	{
		if (!start_monitoring)
		{
			if (bird.bird_sprite.getGlobalBounds().left > pipes[0].sprite_down.getGlobalBounds().left && bird.getRightBound() < pipes[0].getRightBound())
			{
				start_monitoring = true;
			}
		}
		else
		{
			if (bird.bird_sprite.getGlobalBounds().left > pipes[0].getRightBound())
			{
				score++;
				score_text.setString("Score: " + toString(score));
				start_monitoring = false;
			}
		}
	}
}

std::string Game::toString(int num)
{
	std::stringstream ss;
	ss << num;
	return ss.str();
}
