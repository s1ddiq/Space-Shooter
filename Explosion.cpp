#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "../h/Explosion.h"
#include "../h/Audio.h"
#include "../h/Config.h"

Explosion::Explosion(sf::Vector2f pos, sf::Texture& explosionTexture) : sprite(explosionTexture) {
	sprite.setTexture(explosionTexture);
	sprite.setOrigin({ 26.f, 26.f }); // change to use config explosion w and h / 2
	sprite.setPosition(pos);
	sprite.setScale({Config::SHIP_SCALE, Config::SHIP_SCALE});
	sprite.setTextureRect(sf::IntRect({ 0, 0 }, { Config::SpriteConfig::EXPLOSION_FRAME_WIDTH, Config::SpriteConfig::EXPLOSION_FRAME_HEIGHT }));
}

// CHANGE to config
void Explosion::setFrame(int frame) {
	sprite.setTextureRect(sf::IntRect({
		{ Config::SpriteConfig::EXPLOSION_FRAME_WIDTH * frame, 0 },
		{ Config::SpriteConfig::EXPLOSION_FRAME_WIDTH, Config::SpriteConfig::EXPLOSION_FRAME_HEIGHT }
	}));
}

void Explosion::update(float dt) {
	Audio::sound().playExplosion();

	if (frameClock.getElapsedTime().asSeconds() > 0.03f) {
		currentFrame++;

		if (currentFrame > frameCount) {
			animationDone = true;
			return;
		}

		setFrame(currentFrame);
		frameClock.restart();
	}

}

void Explosion::draw(sf::RenderWindow& window) {
	window.draw(sprite);
}

bool Explosion::isFinished() const {
	//return animationDone && Audio::sound().getStatus() == sf::Sound::Status::Stopped;
	return false;
}