#include <iostream>
#include <SFML/Graphics.hpp>
#include "../h/HitEffect.h"
#include "../h/Config.h"

HitEffect::HitEffect(sf::Vector2f pos, sf::Texture& hitEffectTexture)
	: hitEffectSoundBuffer(),
	hitEffectSound(hitEffectSoundBuffer),
	sprite(hitEffectTexture) {
	if (!hitEffectSoundBuffer.loadFromFile(Config::Sounds::Explosion)) {
		std::cerr << "Failed to load explosion sound\n";
	}

	hitEffectSound.setBuffer(hitEffectSoundBuffer);
	hitEffectSound.play();

	sprite.setTexture(hitEffectTexture);
	sprite.setScale({ .7f, .7f });
	//sprite.setOrigin()
	sprite.setPosition(pos);
	//sprite.setScale()
	sprite.setTextureRect(sf::IntRect({ 0, 0 }, { Config::SpriteConfig::HIT_EFFECT_FRAME_WIDTH, Config::SpriteConfig::HIT_EFFECT_FRAME_HEIGHT }));
}

void HitEffect::setFrame(int frame) {
	sprite.setTextureRect(sf::IntRect({
		{Config::SpriteConfig::HIT_EFFECT_FRAME_WIDTH * frame, 0},
		{Config::SpriteConfig::HIT_EFFECT_FRAME_WIDTH, Config::SpriteConfig::HIT_EFFECT_FRAME_HEIGHT},
	}));
}

void HitEffect::update(float dt) {
	if (frameClock.getElapsedTime().asSeconds() > 0.03f) {
		currentFrame++;

		if (currentFrame >= frameCount) {
			isDone = true;
			return;
		}

		setFrame(currentFrame);
		frameClock.restart();
	}
}

void HitEffect::draw(sf::RenderWindow& window) {
	window.draw(sprite);
}