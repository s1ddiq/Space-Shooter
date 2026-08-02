#include "../h/HealthBar.h"

HealthBar::HealthBar(int max)
	: maxHealth(max)
{
	// Set up the background rectangle (static, grey)
	back.setSize({ 40.f, 20.f }); // Same size initially
	back.setFillColor(sf::Color(50, 50, 50));

	// Set up the foreground rectangle
	fill.setSize({ 40.f, 5.f });
	fill.setFillColor(sf::Color::Green);
}

// Update the health bar every frame

void HealthBar::update(int hp, sf::Vector2f pos) {
	// Calculate the ratio of health (0.0 to 1.0)
	float ratio = hp / maxHealth;

	fill.setSize({ 40.f * ratio, 20.f });

	back.setPosition({
		pos.x,
		0.f
	});
	fill.setPosition(back.getPosition()); // Fill follows the back 
}

void HealthBar::draw(sf::RenderWindow& window) {
	window.draw(back);
	window.draw(fill); 
}