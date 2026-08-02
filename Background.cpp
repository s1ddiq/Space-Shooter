#include "../h/Background.h"
#include "../h/Config.h"
#include <cmath>
#include <iostream>

Background::Background()
	: backgroundTexture(), sprite(backgroundTexture)
{
	if (!backgroundTexture.loadFromFile(Config::BACKGROUND_TEXTURE)) {
		std::cerr << "Failed to load background texture\n";
	}

	//sprite.setPosition({ 0.f, 0.f });
	/*sprite.setScale({
		Config::WINDOW_WIDTH / sprite.getLocalBounds().size.x,
		Config::WINDOW_HEIGHT / sprite.getLocalBounds().size.y
		});*/

	//std::cout << backgroundTexture.generateMipmap();
};

void Background::update(float dt) {
}

void Background::draw(sf::RenderWindow& window) {
	sf::RectangleShape debug({ 20.f, 20.f });
	debug.setFillColor(sf::Color::Red);

	window.draw(debug);

	window.draw(sprite);
}

