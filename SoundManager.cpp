#include "../h/SoundManager.h"
#include "../h/Config.h"
#include <SFML/Audio.hpp>
#include <iostream>

SoundManager::SoundManager() {
    // Load the sound buffers
    if (!m_ShotBuffer.loadFromFile(Config::Sounds::Bullet))
        std::cerr << "Failed to load shot sound\n";
    if (!m_DamageBuffer.loadFromFile(Config::Sounds::Damage))
        std::cerr << "Failed to load damage sound\n";
    if (!m_ExplosionBuffer.loadFromFile(Config::Sounds::Explosion))
        std::cerr << "Failed to load explosion sound\n";

    const int SHOT_POOL_SIZE = 32;
    const int DAMAGE_POOL_SIZE = 16;
    const int EXPLOSION_POOL_SIZE = 16;

    // Fill shot sounds pool
    for (int i = 0; i < SHOT_POOL_SIZE; ++i) {
        sf::Sound s(m_ShotBuffer);
        s.setBuffer(m_ShotBuffer);
        s.setPitch(0.7f + (rand() % 20) / 100.f); // varation so it doesn't feel repetitive
        m_ShotSounds.push_back(s); // adds element to vector to play
    } 

    // Fill damage sounds pool
    for (int i = 0; i < DAMAGE_POOL_SIZE; ++i) {
        sf::Sound s(m_DamageBuffer);
        s.setBuffer(m_DamageBuffer);
        s.setPitch(0.9f + (rand() % 20) / 100.f); // varation so it doesn't feel repetitive
        m_DamageSounds.push_back(s); // adds element to vector to play
    }

    // Fill explosion sounds pool 
    for (int i = 0; i < EXPLOSION_POOL_SIZE; ++i) {
        sf::Sound s(m_ExplosionBuffer);
        s.setBuffer(m_ExplosionBuffer);
        s.setPitch(0.9f + (rand() % 20) / 100.f); // varation so it doesn't feel repetitive
        m_ExplosionSounds.push_back(s); // adds element to vector to play
    }

    m_NextShotSound = 0;
    m_NextDamageSound = 0;
    m_NextExplosionSound = 0;
}

// Play a damage sound (round-robin)
void SoundManager::playDamage() {
    if (m_DamageSounds.empty()) return;

    m_DamageSounds[m_NextDamageSound].play();
    m_NextDamageSound = (m_NextDamageSound + 1) % m_DamageSounds.size();
}

// Play a shot sound (round-robin)
void SoundManager::playShot() {
    if (m_ShotSounds.empty()) return;

    m_ShotSounds[m_NextShotSound].play();
    m_NextShotSound = (m_NextShotSound + 1) % m_ShotSounds.size();
}

// Play a explosion sound (round-robin) 
void SoundManager::playExplosion() {
    if (m_ExplosionSounds.empty()) return;

    m_ExplosionSounds[m_NextExplosionSound].play();
    m_NextExplosionSound = (m_NextExplosionSound + 1) % m_ExplosionSounds.size();
}
