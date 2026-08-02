#include <iostream>
#include <vector>
#include "../h/Game.h"
#include "../h/Audio.h"
#include "../cpp/SmokeSystem.cpp"
#include "../h/Config.h"

namespace {
	static sf::FloatRect getViewBounds(const sf::View& view) {
		sf::Vector2f size = view.getSize();
		sf::Vector2f center = view.getCenter();

		return sf::FloatRect(
			{
			center.x - size.x / 2.f,
			center.y - size.y / 2.f,
			},
			{
			size.x,
			size.y
			}
		);
	};
}

Game::Game() : window(
	sf::VideoMode(
		{ Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT }
	),
	Config::WINDOW_TITLE
)
, playerShip({ Config::WINDOW_WIDTH / 2.f, 500.f }),
smokeSystem({ 200, 400 })
{
	Audio::init(); // init audio once?
	window.setFramerateLimit(120);

	waveManager = new WaveManager(&enemyManager, &playerShip, getViewBounds(gameView));

	window.setView(gameView);
	//float w = static_cast<float>(window.getSize().x);
	//float h = static_cast<float>(window.getSize().y);
	float w = Config::WINDOW_WIDTH;
	float h = Config::WINDOW_HEIGHT;

	// Right 80% of window
	gameView.setSize({ w * 0.8f, h });
	gameView.setCenter({ (w * 0.8f) / 2.f, h / 2.f });
	gameView.setViewport(sf::FloatRect(
		{ 0.1f, // left padding
		0.f }, // top
		{ 0.8f, // widths
		1.f })); // height

	stars = new Stars(100, getViewBounds(gameView)); // Assuming Stars is movable

	sf::FloatRect gameBounds = getViewBounds(gameView);
	

	if (!hitEffectTexture.loadFromFile(Config::SpriteConfig::HIT_EFFECT_TEXTURE)) {
		std::cerr << "Failed to load explosion texture\n";
	}
	Wave& wave = waveManager->getWave();

	sf::FloatRect waveBounds = waveManager->getWaveBounds(wave);
	rect.setPosition(waveBounds.position);
	rect.setSize(waveBounds.size);
	

}

void Game::run() {
	while (window.isOpen()) {
		//sf::View view(sf::FloatRect({ 0.f, 0.f }, sf::Vector2f(window.getSize())));
		//view.zoom(2.f)
		//double ending = Clock.getElapsedTime().asSeconds();

		/*double starting = 0.0001;
		double da = ending - starting;
		starting = ending;*/
		double dt = deltaClock.restart().asSeconds(); // Time since last frame

		//std::string FPS = std::to_string(int(1 / dt));

		//std::cout << FPS << " fps \n";
		processEvents(); // Handle input / close events
		//float dt = deltaClock.restart().asSeconds(); // Time since last frame
		update(dt); // Update game objects
		render(); // Draw everything
	}
}

void Game::processEvents() {
	while (auto event = window.pollEvent()) {
		if (event->is<sf::Event::Closed>()) {
			window.close();
		}

		if (event->is<sf::Event::Resized>()) {
			// do something on resize'
			float w = static_cast<float>(window.getSize().x);
			float h = static_cast<float>(window.getSize().y);

			//sf::View view(sf::FloatRect({ 0.f, 0.f }, sf::Vector2f(window.getSize())));
			//window.setView(view);

			gameView.setSize({ w * 0.8f, h });
			gameView.setCenter({ (w * 0.8f) / 2.f, h / 2.f });
			gameView.setViewport(sf::FloatRect(
				{ 0.1f, // left padding
				0.f }, // top
				{ 0.8f, // width
				1.f })); // height

			Config::WINDOW_WIDTH = window.getSize().x;
			Config::WINDOW_HEIGHT = window.getSize().y;

			waveManager->resize(getViewBounds(gameView));
			stars->resize(getViewBounds(gameView));
		}
	}


}

void Game::update(float dt) {
	sf::FloatRect gameBounds = getViewBounds(gameView);
	Wave& wave = waveManager->getWave();

	sf::FloatRect waveBounds = waveManager->getWaveBounds(wave);
	std::cout << waveBounds.position.x;
	std::cout << waveBounds.position.y;
	//background.update(dt);
	stars->update(dt);

	playerShip.update(dt, gameBounds); // Movement and animation
	playerShip.updateBullets(dt); // Update bullets ^^ can combine these 
	// Update bullets
	enemyManager.update(dt); // Update all enemies
	waveManager->update(dt);

	//smokeSystem.update(dt);
	
	rect.move({ 0.f, 500.f * dt});

	for (auto& hitEffect : hitEffects) {
		hitEffect.update(dt);
	}

	for (auto& bullet : playerShip.getBullets()) {
		for (auto& enemy : enemyManager.getEnemies()) {
			if (bullet.getBounds().findIntersection(enemy.getBounds())) {
				enemyManager.hitEnemy(enemy, playerShip.getDamage());
				bullet.destroy();
				hitEffects.emplace_back(bullet.getPosition(), hitEffectTexture);
			}
		}
	}

	// Remove dead bullets safely
	playerShip.getBullets().erase(
		std::remove_if(playerShip.getBullets().begin(), playerShip.getBullets().end(),
			[](const Bullet& b) {return !b.isAlive; } // Only keep alive bullets
		),
		playerShip.getBullets().end()
	);

	// Remove dead hit effects safely
	hitEffects.erase(
		std::remove_if(hitEffects.begin(), hitEffects.end(),
			[](const HitEffect& e) { return e.isDone; }),
		hitEffects.end()
	);
}

void Game::render() {
	Wave& wave = waveManager->getWave();
	sf::FloatRect waveBounds = waveManager->getWaveBounds(wave);

	window.clear(sf::Color(47, 99, 150)); // Clear previous frame
	window.setView(gameView);
	// Draw rectangle for backgroudn view
	sf::RectangleShape bg(gameView.getSize());
	bg.setPosition(gameView.getCenter() - gameView.getSize() / 2.f );
	bg.setFillColor(sf::Color::Black);
	bg.setOutlineThickness(-3.f);
	bg.setOutlineColor(sf::Color::White);

	//rect.setPosition(waveBounds.position);
	//rect.setSize(waveBounds.size);

	rect.setFillColor(sf::Color::Transparent);
	rect.setOutlineColor(sf::Color::Green);
	rect.setOutlineThickness(2.f);

	window.draw(bg);
	stars->draw(window);
	//background.draw(window); // 1 | Draw the bg

	enemyManager.draw(window); // 4 | Draw the enemies

	playerShip.drawBullets(window); // 2 | Draw bullets first so they can appear under the playerShip.
	playerShip.draw(window); // 3 | Draw the ship 

	window.draw(rect);
	//smokeSystem.draw(window); // 4 | draw window smoke test!

	// USE EFFECT MANAGER LATER
	for (auto& hitEffect : hitEffects) {
		hitEffect.draw(window);
	}

	// ---------- UI VIEW
	// Draw ship collision bounds (DEBUG)
	// TODO: Make this a reusable thing you just call on something (2)
	//sf::FloatRect pb = playerShip.getBounds();
	//sf::RectangleShape pbcv({pb.size.x, pb.size.y});

	/*pbcv.setPosition({ pb.position.x, pb.position.y });
	pbcv.setFillColor(sf::Color::Transparent);
	pbcv.setOutlineColor(sf::Color::Red);
	pbcv.setOutlineThickness(1.f);*/


	// Player Bounds Collision Visual PBCV
	//window.draw(pbcv);

	window.display(); // Display frame
}

void Game::drawUI() {
	//sf::Text scoreText;
}
