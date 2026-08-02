#include <iostream>
#include <vector>
#include "../h/EnemyManager.h"
#include "../h/WaveManager.h"
#include "../h/Formation.h"
#include "../h/Config.h"



// Spawn a new enemy

EnemyManager::EnemyManager() {
	if (!enemyTexture.loadFromFile(Config::ENEMY_TEXTURE)) {
		std::cerr << "Failed to load enemy texture\n";
	}

	if (!explosionTexture.loadFromFile(Config::SpriteConfig::EXPLOSION_TEXTURE)) {
		std::cerr << "Failed to load explosion texture\n";
	}
}



void EnemyManager::update(float dt) {
	enemyPointers.clear();

	for (Enemy& enemy : enemies) {
		if (enemy.isAlive) {
			// Push pointers LEARN MORE LATER
			enemyPointers.push_back(&enemy);

			for (auto& bullet : enemy.getBullets()) {
				bullet.update(dt);
			}
		}
	}

	for (Enemy* enemy : enemyPointers) {
		enemy->update(dt);
		//enemy->applySeparation(enemyPointers, dt); // PUSH LAST
	}

	for (size_t i = 0; i < explosions.size(); ) {
		explosions[i].update(dt);

		if (explosions[i].isFinished()) {
			explosions.erase(explosions.begin() + i);
		}
		else
			i++;
	}

	// Remove dead enemies safely
	getEnemies().erase(
		std::remove_if(getEnemies().begin(), getEnemies().end(),
			[](const Enemy& e) {return !e.isAlive; } // Only keep alive bullets
		),
		getEnemies().end()
	);


	// Remove explosions that are done safely - OLD REFACTOR LATER
	//explosions.erase(
	//	std::remove_if(explosions.begin(), explosions.end(),
	//		[](const Explosion& explosion) { return explosion.isFinished(); }),
	//	explosions.end()
	//);
}

// Draw all enemies
void EnemyManager::draw(sf::RenderWindow& window) {

	// Draw enemies if alive.
	for (auto& enemy : enemies) {
		if (enemy.isAlive) {
			//sf::CircleShape radius(50.f);           // Radius = 50
			//radius.setOrigin({ 50.f, 50.f });           // Center circle
			//radius.setPosition(enemy.getPosition());    // Same center as sprite
			//radius.setFillColor(sf::Color::Transparent);
			//radius.setOutlineThickness(1.f);
			//radius.setOutlineColor(sf::Color::Red);
			//window.draw(radius);

			for (auto& bullet : enemy.getBullets()) {
				bullet.draw(window);
			}
			enemy.draw(window);
			//std::cout << enemy.getPosition().x << "\n";
			//std::cout << enemy.getPosition().y << "\n";
		};
	};
	for (auto& ex : explosions) ex.draw(window);
}

void EnemyManager::spawnEnemy(const sf::Vector2f& startPos) {
	enemies.emplace_back(enemyTexture, startPos, false); // Add new Enemy to vector
}

void EnemyManager::spawnBoss(const sf::Vector2f& pos) {
	Enemy boss(enemyTexture, pos, true);
	boss.isAlive = true;
	boss.setSpeed(200.f);
	boss.setHealth(15000);
	enemies.push_back(boss);
}

// Refactor!!!!!!

void EnemyManager::killEnemy(Enemy& enemy) {
	if (!enemy.isAlive) {
		return;
	}

	// Spawn explosion at enemy position
	explosions.emplace_back(enemy.getPosition(), explosionTexture);

	// Kill enemy;
	enemy.kill();
}

void EnemyManager::hitEnemy(Enemy& enemy, int damage) {
	if (!enemy.isAlive) return;

	// Apply damage
	enemy.hit(damage);

	// Check if dead
	if (enemy.getHealth() <= 0) {
		killEnemy(enemy);
		//std::cout << "ENEMY_KILLED\n";
	}
}

// Get all enemies
std::vector<Enemy>& EnemyManager::getEnemies() {
	return enemies;
}