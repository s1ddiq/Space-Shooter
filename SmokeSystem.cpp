#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <iostream>

struct Particle {
	sf::Vector2f position;
	sf::Vector2f velocity;
	float lifetime; // in seconds
	float size;
	sf::Color color;
};

class SmokeSystem {
public:
	SmokeSystem(sf::Vector2f emitterPos) : emitter(emitterPos) {}

	void update(float dt) {
		// Spawn new particles
		spawnTimer += dt;
		if (spawnTimer > 0.05f) {
			spawnParticle();
			spawnTimer = 0.f;
		}

		// Update existing partciels
		for (auto& p : particles) {
			p.position += p.velocity * dt;
			p.lifetime -= dt;
			// fade out as lifetime decreases
			p.color.a = (255 * (p.lifetime / maxLifetime));
		}

		// Remove dead particles
		particles.erase(
			std::remove_if(particles.begin(), particles.end(),
			[](const Particle& p) {return p.lifetime <= 0; }),
			particles.end()
		);
	}

	void draw(sf::RenderWindow& window) {
		for (auto& p : particles) {
			sf::CircleShape shape(p.size);
			shape.setPosition(p.position);
			shape.setFillColor(p.color);
			shape.setOrigin({ p.size, p.size }); // center origin
			window.draw(shape);
		}
	}

private:
	void spawnParticle() {
		Particle p;
		p.position = emitter;
		p.velocity = sf::Vector2f(
			(rand() % 20 - 10) * 0.1f,   // horizontal: -1 to 1
			-(50 + rand() % 50) * 0.5f  // vertical: -25 to -50, much faster
		);
		p.lifetime = maxLifetime;
		p.size = 5 + rand() % 5;
		p.color = sf::Color(200, 200, 200, 255); // gray smoke
		particles.push_back(p);
	}

	std::vector<Particle> particles;
	sf::Vector2f emitter;
	float spawnTimer = 0.f;
	const float maxLifetime = 2.f; // seconds
};