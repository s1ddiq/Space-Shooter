// Enemy.cpp
#include "../h/Enemy.h"
#include "../h/Audio.h"
#include <iostream>
#include <cmath>

// Constructor implementation
Enemy::Enemy(sf::Texture& texture, const sf::Vector2f& startPos, bool isBoss)
	:
	sprite(texture),
	boss(isBoss)
{
	// Set initial texture rectangle (first animation s)
	sprite.setTextureRect({ {0, 0}, {frameWidth, frameHeight} });

	// Scale enemy sprite
	sprite.setScale({ Config::SHIP_SCALE, Config::SHIP_SCALE });
	if (boss) {
		sprite.setScale({ 2.f, 2.f });
		//setVelocity({ 0.f, 0.f });
		healthBar = new HealthBar(getHealth()); // bigger health for boss
	}

	// Set starting position
	sprite.setPosition(startPos);

	// Center sprite origin
	sprite.setOrigin(
		{
			sprite.getLocalBounds().size.x / 2.f,
			sprite.getLocalBounds().size.y / 2.f
		}
	);
}

// Sets animation frame
void Enemy::setFrame(int frame) {
	sprite.setTextureRect({
		{ frame * frameWidth, 0 },
		{ frameWidth, frameHeight }
		});
}

// Updates enemy logic
void Enemy::update(float dt) {

	// Stop updating if dead
	if (!isAlive) return;
	// This line creates a bug because isOffScreen checks for top collisions or position being < 0, so that will delete any enemy above 0 on the y axis.
	// ^^ See line 181-183

	if (isOffScreen()) isAlive = false; 

	// Move enemy based on velocity
	//if (boss) {
	//	if (getPosition().y >= 300.f) {
	//		// Set velocity to move left and right
	//		sprite.move({ velocity.x * dt, 0.f });
	//	}
	//	else {
	//		sprite.move(velocity * dt);
	//	}
	//}

	/*else {
		sprite.move(velocity * dt);
	}*/
	sprite.move(velocity * dt);

	// Adjust animation frame based on horizontal movement
	if (velocity.x > 30.f)
		targetFrame = Config::SHIP_FRAMES::FullRight;
	else if (velocity.x < -30.f)
		targetFrame = Config::SHIP_FRAMES::FullLeft;
	else
		targetFrame = Config::SHIP_FRAMES::Slight;

	// Animate sprite smoothly
	if (frameClock.getElapsedTime().asSeconds() >= Config::ENEMY_FRAME_TIME) {
		if (currentFrame < targetFrame) currentFrame++;
		else if (currentFrame > targetFrame) currentFrame--;
		setFrame(currentFrame);
		frameClock.restart();
	}

	// Fire bullets periodically
	if (shootClock.getElapsedTime().asSeconds() >= 0.2f) {
		if (boss) {
			shoot();
		}
	}

	// Handle hit flash
	if (isHit && flashClock.getElapsedTime().asSeconds() >= 0.05f) {
		sprite.setColor(sf::Color::White);
		isHit = false;
	}

	if (boss && healthBar) {
		healthBar->update(health, sprite.getPosition());
	}
}

// Draw enemy
void Enemy::draw(sf::RenderWindow& window) {
	if (isAlive) {
		window.draw(sprite);
			if (boss && healthBar)
				healthBar->draw(window);
	}
}

// Shoot bullets
void Enemy::shoot() {
	sf::FloatRect bounds = sprite.getGlobalBounds();

	// Spawn bullet from enemy center
	bullets.emplace_back(
		Config::SHOOTER_TYPE::Enemy,
		bounds,
		Config::POWERUPS::Automatic
	);

	// Restart shooting timer
	shootClock.restart();
}

// Apply separation force from nearby enemies
void Enemy::applySeparation(const std::vector<Enemy*>& enemies, float dt) {
	sf::Vector2f separationForce(0.f, 0.f);
	int nearbyCount = 0;

	for (const Enemy* other : enemies) {
		if (other == this || !other->isAlive) continue;

		sf::Vector2f diff = sprite.getPosition() - other->getPosition();
		float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

		if (distance > 0.f && distance < SEPARATION_RADIUS) {
			diff /= distance; // normalize
			separationForce += diff;
			nearbyCount++;
		}
	}

	if (nearbyCount > 0) {
		separationForce *= SEPARATION_FORCE;
		velocity += separationForce * dt;
	}
}

// Update bullets
void Enemy::updateBullets(float dt) {
	for (size_t i = 0; i < bullets.size();) {
		bullets[i].update(dt);

		if (bullets[i].isOffScreen())
			bullets.erase(bullets.begin() + i);
		else
			++i;
	}
}

// Draw bullets
void Enemy::drawBullets(sf::RenderWindow& window) {
	for (auto& bullet : bullets)
		bullet.draw(window);
}

// Return bullets
std::vector<Bullet>& Enemy::getBullets() {
	return bullets;
}

// Get position
sf::Vector2f Enemy::getPosition() const {
	return sprite.getPosition();
}

// Get bounds
sf::FloatRect Enemy::getBounds() const {
	return sprite.getGlobalBounds();
}

// Apply damage
void Enemy::hit(int damage) {
	health -= damage;
	isHit = true;

	Audio::sound().playDamage(); // this is it!

	sprite.setColor(sf::Color(255, 255, 255, 100));
	flashClock.restart();
}

// Kill enemy
void Enemy::kill() {
	isAlive = false;

	// delete later / clean up here
	float velocityY = 0.05; 
	sf::Clock clock;
	if (clock.getElapsedTime().asSeconds() > 0.02f) {
		velocityY += 0.07;
		velocity = { 0.f, velocityY };
		clock.restart();
	}
}

bool Enemy::isOffScreen() const {
	sf::FloatRect bounds = getBounds();
	sf::Vector2f pos = sprite.getPosition();
	// Top Bottom Left Right
	// Was PREVIOUSLY: pos.y + bounds.size.y < 0.f
	return pos.y > Config::WINDOW_HEIGHT; //||
	//pos.x < 0.f || pos.x + bounds.size.x > Config::WINDOW_WIDTH;
}