#include <iostream>
#include "../h/Ship.h"
#include "../h/Config.h"
#include "../h/Bullet.h"
#include "../h/Audio.h"

Ship::Ship(const sf::Vector2f& startPos) :
	bulletSoundBuffer(),
	thrusterSoundBuffer(),
	shipTexture(),
	exhaustTexture(),
	bulletSound(bulletSoundBuffer),
	thrusterSound(thrusterSoundBuffer),
	sprite(shipTexture),
	exhaust(exhaustTexture)
{

	if (!thrusterSoundBuffer.loadFromFile(Config::Sounds::Thrusters)) {
		std::cerr << "Failed to load bullet fired sound!\n";
	}

	if (!shipTexture.loadFromFile(Config::SHIP_TEXTURE)) {
		std::cerr << "Failed to load ship texture!\n";
	}
	if (!exhaustTexture.loadFromFile(Config::SpriteConfig::EXHAUST_TEXTURE)) {
		std::cerr << "Failed to load exhaust texture!\n";
	}

	//shipTexture.setSmooth(true);


	sprite.setTextureRect(sf::IntRect({ 0, 0 }, { 64, 64 })); // change later
	sprite.setTexture(shipTexture); // Apply texture
	sprite.setScale({ Config::SHIP_SCALE, Config::SHIP_SCALE }); // Scale ship
	sprite.setPosition(startPos); // Set starting psoition

	// Fix exhaust origin scaling issue
	sprite.setOrigin(
		{
			sprite.getLocalBounds().size.x / 2.f,
			sprite.getLocalBounds().size.y / 2.f
		}
	);


	exhaust.setTextureRect(sf::IntRect({ 0, 0 }, { 64, 64 }));
	exhaust.setTexture(exhaustTexture); // Applies exhaust texture
	exhaust.setScale({ Config::SHIP_SCALE, Config::SHIP_SCALE }); // Scale ship
	sf::FloatRect shipBounds = sprite.getGlobalBounds();
	/*exhaust.setOrigin(
		{
			exhaust.getLocalBounds().size.x / 2.f,
			exhaust.getLocalBounds().size.y / 2.f,
	});*/
	
	thrusterSound.setVolume(1.f);
	thrusterSound.setRelativeToListener(true);
	thrusterSound.play();
	thrusterSound.setLooping(true);

	//bulletSound.setVolume(15.f);
}

void Ship::setFrame(int shipFrame, int exhaustFrame) {
	currentFrame = shipFrame;
	currentExhaustFrame = exhaustFrame;

	// Ship sprite
	sprite.setTextureRect({
		{ currentFrame * frameWidth, 0 },
		{ frameWidth, frameHeight }
		});

	// Exhaust sprite
	exhaust.setTextureRect({
		{ currentExhaustFrame * Config::SpriteConfig::EXHAUST_FRAME_WIDTH, 0 },
		{ Config::SpriteConfig::EXHAUST_FRAME_WIDTH, Config::SpriteConfig::EXHAUST_FRAME_HEIGHT }
		});
}


void Ship::update(float dt, sf::FloatRect& gameBounds) {
	// Global bounds of player ship. 
	sf::FloatRect shipBounds = sprite.getGlobalBounds();
	sf::FloatRect lShipBounds = sprite.getLocalBounds();

	float exhaustOffsetY = 6.95f;
	float exhaustOffsetX = 1.5f;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A)) {
		sprite.move({ -Config::SHIP_SPEED * dt, 0.f }); // Move left

		// Set target frame to FULL LEFT
		targetFrame = Config::SHIP_FRAMES::FullLeft;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D)) {
		sprite.move({ Config::SHIP_SPEED * dt, 0.f }); // Move right 
		// Set target frame to FULL LEFT
		targetFrame = Config::SHIP_FRAMES::FullRight;
	}
	else {
		// Else if released, set the SHIP_FRAME to default
		targetFrame = Config::SHIP_FRAMES::Slight;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::W)) {
		sprite.move({ 0.f, -Config::SHIP_SPEED * dt }); // Move up
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S)) {
		sprite.move({ 0.f, Config::SHIP_SPEED * dt }); // Move down
	}

	// ALWAYS SET POSITION TO BOTTOM OF SHIP
	//exhaust.setPosition({
	//	sprite.getPosition().x, // minus 3 frames to account for discenter
	//	sprite.getPosition().y + Config::SpriteConfig::EXHAUST_FRAME_HEIGHT / 2.f + 4.f // plus 4 frames
	//	});

	exhaust.setPosition({
		shipBounds.position.x - exhaustOffsetX,
		shipBounds.position.y + shipBounds.size.y / 2.f - exhaustOffsetY
	});


	// TODO: Create a wait for shooting.
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Space) || sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
		shoot(); // Ship handles its own bullets now
	}


	// COLLISIONS

	// Get ship bounding box (position + size)
	float left = gameBounds.position.x;
	float right = gameBounds.position.x + gameBounds.size.x;
	float top = gameBounds.position.y;
	float bottom = gameBounds.position.y + gameBounds.size.y;

	sf::FloatRect ship = getBounds();
	sf::Vector2f pos = getPosition();

	// Left wall
	if (ship.position.x < left) {
		sprite.setPosition({ left + ship.size.x / 2.f, pos.y });
	}

	// Right wall
	if (ship.position.x + ship.size.x > right) {
		sprite.setPosition({ right - ship.size.x / 2.f, pos.y });
	}

	// Top wall
	if (ship.position.y < top) {
		sprite.setPosition({ pos.x, top + ship.size.y / 2.f });
	}

	// Bottom wall
	if (ship.position.y + ship.size.y > bottom) {
		sprite.setPosition({ pos.x, bottom - ship.size.y / 2.f });
	}


	// SPRITE ANIMATION
	if (frameClock.getElapsedTime().asSeconds() >= Config::SHIP_FRAME_TIME) {

		// Animate ship frame
		if (currentFrame > targetFrame) currentFrame--;
		else if (currentFrame < targetFrame) currentFrame++;

		// Animate exhaust frame
		currentExhaustFrame++;
		if (currentExhaustFrame >= Config::SpriteConfig::EXHAUST_FRAME_COUNT)
			currentExhaustFrame = 0; // loop animation

		setFrame(currentFrame, currentExhaustFrame);
		frameClock.restart();
	}

}

// Draw ship to the window 
void Ship::draw(sf::RenderWindow& window) {

	/*sf::FloatRect pb = getBounds();
	sf::FloatRect a = exhaust.getGlobalBounds();
	sf::RectangleShape pbcv({pb.size.x, pb.size.y});
	sf::RectangleShape ab({a.size.x, a.size.y});

	pbcv.setPosition({ pb.position.x, pb.position.y });
	pbcv.setFillColor(sf::Color::Transparent);
	pbcv.setOutlineColor(sf::Color::Red);
	pbcv.setOutlineThickness(1.f);

	ab.setPosition({ a.position.x, a.position.y });
	ab.setFillColor(sf::Color::Transparent);
	ab.setOutlineColor(sf::Color::Red);
	ab.setOutlineThickness(1.f);*/

	window.draw(exhaust);
	// Render sprite exhaust 2
	window.draw(sprite);

	//window.draw(pbcv);
	//window.draw(ab);
	// Render sprite 1

}

// ---- SHOOTING ----

void Ship::shoot() {
	sf::FloatRect bounds = sprite.getGlobalBounds();

	if (shootClock.getElapsedTime().asSeconds() > Config::SHOOT_COOLDOWN) {
		// Play the bullet sound when a shot actually fires
		Audio::sound().playShot();

		// ---- DOUBLE SHOT POWERUP ----
		if (activePowerUp == Config::POWERUPS::DoubleShot) {
			float offset = 10.f;

			// Left bullet
			sf::Rect<float> leftBounds = bounds;
			leftBounds.position.x -= offset;   // move left
			bullets.emplace_back(Config::SHOOTER_TYPE::Player, leftBounds, activePowerUp);

			// Right bullet
			sf::Rect<float> rightBounds = bounds;
			rightBounds.position.x += offset - 3.f;  // move right minus 3.f to make up for the discenter
			bullets.emplace_back(Config::SHOOTER_TYPE::Player, rightBounds, activePowerUp);
		}
		// ---- TRIPLE SHOT POWERUP ----
		else if (activePowerUp == Config::POWERUPS::TripleShot) {
			float offset = 10.f;

			// Left bullet
			sf::FloatRect leftBounds = bounds;
			leftBounds.position.x -= offset;
			bullets.emplace_back(Config::SHOOTER_TYPE::Player, leftBounds, activePowerUp);

			// Center bullet (no offset)
			sf::FloatRect centerBounds = bounds;
			bullets.emplace_back(Config::SHOOTER_TYPE::Player, centerBounds, activePowerUp);

			// Right bullet
			sf::FloatRect rightBounds = bounds;
			rightBounds.position.x += offset;
			bullets.emplace_back(Config::SHOOTER_TYPE::Player, rightBounds, activePowerUp);
		}
		// ---- QUADRUPLE SHOT POWERUP ----
		else if (activePowerUp == Config::POWERUPS::QuadrupleShot) {
			float offset = 16.f; // distance between bullets

			// Leftmost bullet
			sf::FloatRect leftmost = bounds;
			leftmost.position.x -= offset * 1.5f;
			bullets.emplace_back(Config::SHOOTER_TYPE::Player, leftmost, activePowerUp);

			// Left-center bullet
			sf::FloatRect leftCenter = bounds;
			leftCenter.position.x -= offset * 0.5f;
			leftCenter.position.y -= offset * 0.5f;
			bullets.emplace_back(Config::SHOOTER_TYPE::Player, leftCenter, activePowerUp);

			// Right-center bullet
			sf::FloatRect rightCenter = bounds;
			rightCenter.position.x += offset * 0.5f;
			rightCenter.position.y -= offset * 0.5f;
			bullets.emplace_back(Config::SHOOTER_TYPE::Player, rightCenter, activePowerUp);

			// Rightmost bullet
			sf::FloatRect rightmost = bounds;
			rightmost.position.x += offset * 1.5f - 3.5f;
			bullets.emplace_back(Config::SHOOTER_TYPE::Player, rightmost, activePowerUp);
		}

		shootClock.restart();
	}
}

void Ship::updateBullets(float dt) {
	for (size_t i = 0; i < bullets.size(); ) {
		bullets[i].update(dt);
		if (bullets[i].isOffScreen()) {
			bullets.erase(bullets.begin() + i);
		}
		else {
			++i;
		}
	}
}

void Ship::drawBullets(sf::RenderWindow& window) {
	for (auto& bullet : bullets) {
		//// DEBUG
		/*sf::FloatRect bb = bullet.getBounds();
		sf::RectangleShape bbcv({ bb.size.x, bb.size.y });

		bbcv.setPosition({ bb.position.x, bb.position.y });
		bbcv.setFillColor(sf::Color::Transparent);
		bbcv.setOutlineColor(sf::Color::Red);
		bbcv.setOutlineThickness(1.f);
		window.draw(bbcv);*/
		bullet.draw(window);
	}
}

std::vector<Bullet>& Ship::getBullets() { return bullets; }

// Get ship collision bounds
sf::FloatRect Ship::getBounds() const {
	return sprite.getGlobalBounds();
}

// Get ship position
sf::Vector2f Ship::getPosition() const {
	return sprite.getPosition();
}

// Get active powerup
Config::POWERUPS Ship::getActivePowerUp() const {
	return activePowerUp;
}

void Ship::setActivePowerUp(Config::POWERUPS powerup) {
	activePowerUp = powerup;
}