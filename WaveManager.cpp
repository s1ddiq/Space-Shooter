#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include "../h/WaveManager.h"
#include "../h/Formation.h"
#include "../h/Config.h"

WaveManager::WaveManager(EnemyManager* manager, Ship* player, const sf::FloatRect& area)
	: enemyManager(manager), player(player), area(area)
{
	float centerX = area.size.x / 2.f;
	float startingYPosition = -250.f;

	int wave4cols = 3;
	int wave4rows = 3;

	// Wave 1: 5 enemies in a line formation 
	waves.push_back({
		5,         // enemyCount
		0.f,       // spawnDelay
		{centerX, -250.f},
		Formation::Line(5, 150.f),
		false,     // isBossWave
		false,     // followPlayer
		false      // shoots
		});

	// Wave 2: 9 enemies in a V formation 
	waves.push_back({
		9,
		0.f,
		{centerX, -250.f},
		Formation::V(9, 150.f),
		false,
		false,
		false
		});

	// Wave 3: 6 enemies in a Vertical Line formation
	waves.push_back({
		6,
		0.f,
		{centerX, -250.f},
		Formation::VerticalLine(6, 150.f),
		false,
		false,
		false,
		});

	// Wave 4: enemies in a Grid formation
	waves.push_back({
		wave4cols * wave4rows,
		0.05f,
		{centerX, -250.f},
		Formation::Grid(wave4cols, wave4rows, 256.f),
		false,
		false,
		false
		});

	// Wave 5: Boss wave 
	waves.push_back({
		0,
		0.f,
		{centerX, -250.f},
		Formation::VerticalLine(1, 0.f),
		true,      // isBossWave
		true,      // followPlayer
		true,      // shoots
		});

	startNextWave();
}

void WaveManager::startNextWave() {
	currentWave++;
	spawnedThisWave = 0;
	spawnTimer = 0.f;
	bossSpawned = false; // Reset for new boss wave

	if (currentWave >= waves.size()) {
		std::cout << "All waves completed!\n";
		currentWave = 0; // Loop back to start (or handle differently)
	}
}

void WaveManager::update(float dt) {
	if (currentWave >= (int)waves.size()) {
		startNextWave();
		return;
	}

	Wave& wave = waves[currentWave];

	// Boss wave: spawn boss
	if (wave.isBossWave && !bossSpawned) {
		// Center the boss in the area
		sf::Vector2f bossPos = { area.position.x + area.size.x / 2.f, -150.f };
		enemyManager->spawnBoss(bossPos);
		bossSpawned = true;
		return;
	}

	// Spawn enemies over time
	spawnTimer += dt;
	if (spawnedThisWave < wave.enemyCount && spawnTimer >= wave.spawnDelay) {
		if (spawnedThisWave < wave.offsets.size()) {
			sf::Vector2f worldPos = wave.position + wave.offsets[spawnedThisWave];
			enemyManager->spawnEnemy(worldPos);
		}
		spawnedThisWave++;
		spawnTimer = 0.f;
	}

	// Check if wave cleared
	bool allDead = true;

	// Only check if we've spawned all enemies
	if (spawnedThisWave >= wave.enemyCount) {
		for (auto& enemy : enemyManager->getEnemies()) {
			if (enemy.isAlive) {
				allDead = false;
				break;
			}
		}
	}
	else {
		allDead = false;
	}

	// Update enemy behaviors
	for (auto& enemy : enemyManager->getEnemies()) {
		if (wave.followPlayer && enemy.isAlive) {
			float dx = player->getPosition().x - enemy.getPosition().x;
			float velocityX = (dx != 0) ? (dx / std::abs(dx)) * 150.f : 0.f;
			float velocityY = wave.isBossWave ? 250.f : 450.f;
			enemy.setVelocity({ velocityX, velocityY });
		}

		if (wave.shoots && enemy.isAlive) {
			enemy.shoot();
		}
	}

	// Advance to next wave if all enemies spawned AND dead
	if (allDead && spawnedThisWave >= wave.enemyCount) {
		startNextWave();
	}
}

Wave& WaveManager::getWave() {
	if (currentWave < 0 || currentWave >= waves.size()) {
		throw std::out_of_range("No active wave");
	}
	return waves[currentWave];
}

sf::FloatRect WaveManager::getWaveBounds(const Wave& wave) {
	if (wave.offsets.empty())
		return sf::FloatRect({ wave.position.x, wave.position.y }, { 0.f, 0.f });

	float minX = wave.offsets[0].x;
	float maxX = wave.offsets[0].x;
	float minY = wave.offsets[0].y;
	float maxY = wave.offsets[0].y;

	for (const auto& o : wave.offsets) {
		minX = std::min(minX, o.x);
		maxX = std::max(maxX, o.x);
		minY = std::min(minY, o.y);
		maxY = std::max(maxY, o.y);
	}

	return sf::FloatRect(
		{ wave.position.x + minX, wave.position.y + minY },
		{ maxX - minX, maxY - minY }
	);
}

bool WaveManager::isFinished() const {
	return currentWave >= (int)waves.size() && bossSpawned;
}

void WaveManager::resize(const sf::FloatRect& newArea) {
	area = newArea;
}