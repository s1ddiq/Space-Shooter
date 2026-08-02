// Bullet.cpp
#include <iostream>
#include "../h/Bullet.h"
#include "../h/Config.h"

// Define static texture
sf::Texture Bullet::bulletTexture;

Bullet::Bullet(Config::SHOOTER_TYPE shooter,
    const sf::FloatRect& spawnBounds,
    Config::POWERUPS powerup)
    : bullet(bulletTexture), shooter(shooter)
{
    // Load texture only once
    static bool loaded = false;
    if (!loaded) {
        if (!bulletTexture.loadFromFile(Config::SpriteConfig::PLAYER_BULLET_TEXTURE)) {
            std::cerr << "Failed to load bullet texture!\n";
        }
        loaded = true;
    }

    // Set velocity

    // Random slight speeds 
    float variation = static_cast<float>(rand() % 3 - 1); // -1, 0, +1

    if (shooter == Config::SHOOTER_TYPE::Player)
        velocity = { 0.f, -Config::PLAYER_BULLET_SPEED + variation };

    else
        velocity = { 0.f, Config::ENEMY_BULLET_SPEED };

    // Center origin
    //sf::FloatRect bulletBounds = bullet.getLocalBounds();
    //bullet.setOrigin({ bulletBounds.size.x / 2.f, bulletBounds.size.y / 2.f });

    // Spawn at center of ship (SFML 3 style)
    bullet.setPosition({
        spawnBounds.position.x + spawnBounds.size.x / 2.f,
        spawnBounds.position.y + spawnBounds.size.y / 2.f
    });

}

void Bullet::update(float dt) {
    if (!isAlive) return;
    // ADD SHRAPNEL PARTICLES
    bullet.move(velocity * dt);
}

void Bullet::draw(sf::RenderWindow& window) {
    window.draw(bullet);
}

sf::FloatRect Bullet::getBounds() const {
    return bullet.getGlobalBounds();
}

bool Bullet::isOffScreen() const {
    sf::FloatRect bounds = getBounds();
    sf::Vector2f pos = bullet.getPosition();

    return pos.y + bounds.size.y < 0.f || pos.y > Config::WINDOW_HEIGHT ||
        pos.x < 0.f || pos.x + bounds.size.x > Config::WINDOW_WIDTH;
}

Config::SHOOTER_TYPE Bullet::getShooter() const {
    return shooter;
}
