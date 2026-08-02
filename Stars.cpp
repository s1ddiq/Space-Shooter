#include "../h/Stars.h"
#include <SFML/Graphics.hpp>
#include <vector>

Stars::Stars(int count, const sf::FloatRect& area) : area(area), layers(3) {

	// Generate stars randomly inside the given area
	for (int i = 0; i < layers; i++) {
		for (int j = 0; j < count; j++) {
			Star star;
			star.shape.setRadius(1.f);
			star.layer = i;

			int depth = i / (layers - 1);
			std::cout << depth << "\n";
			int minAlpha = 55;
			std::cout << minAlpha << "\n";
			int maxAlpha = 255;
			std::cout << maxAlpha << "\n";

			int alpha = minAlpha + depth * (maxAlpha - minAlpha);
			std::cout << alpha;

			star.shape.setFillColor(sf::Color(255, 255, 255, alpha));

			float x = area.position.x + static_cast<float>(rand()) / RAND_MAX * area.size.x;
			float y = area.position.y + static_cast<float>(rand()) / RAND_MAX * area.size.y;

			star.shape.setPosition({ x, y });

			stars.push_back(star);
		}
	}
}

void Stars::update(float dt) {
	float baseSpeed = 50.f; // px/s
	float maxExtraSpeed = 250.f;

	for (auto& star : stars) {
		float depth = float(star.layer) / float(layers - 1);
		float speed = baseSpeed + depth * maxExtraSpeed;
		star.shape.move({ (rand() % 3 - 1) * dt, speed * dt });

		// wrap around smoothly
		if (star.shape.getPosition().y > area.position.y + area.size.y) {
			float x = area.position.x + static_cast<float>(rand()) / RAND_MAX * area.size.x;

			// subtract area.size.y to wrap to top instead of tiny random offset
			float y = star.shape.getPosition().y - (area.size.y + 1.f);
			star.shape.setPosition({ x, y });
		}
	}
}


void Stars::draw(sf::RenderWindow& window) {
	for (auto& star : stars) {
		window.draw(star.shape);
	}
}

void Stars::resize(const sf::FloatRect& newArea) {
	area = newArea; // you just update the area to the new area lol
}