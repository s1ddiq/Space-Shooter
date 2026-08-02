#include "../h/Formation.h"
#include <cmath>

/* -----------------
	LINE FORMATION
	Creates a horizontal lne of enemies
	start = pos of n0 enemy
	count = num of enemies
	spacing = dist between each enemy
   ----------------- */

std::vector<sf::Vector2f> Formation::Line(int count, float spacing) {
	std::vector<sf::Vector2f> out; // Vector to store positions

	for (int i = 0; i < count; i++) {
		// Each enemy is spaced along the x axis
		out.push_back({ i * spacing, 0.f }); // x offset from wave pos, same y level
	}

	return out; // Return local offsets
}

/* -----------------
	VERTICAL LINE FORMATION
	Creates a vertical line of enemies
	start = pos of n0 enemy
	count = num of enemies
	spacing = dist between each enemy
   -----------------
*/


std::vector<sf::Vector2f> Formation::VerticalLine( int count, float spacing) {
	std::vector<sf::Vector2f> out; // Vector to store positions

	for (int i = 0; i < count; i++) {
		// Each enemy is spacing along the y axis
		out.push_back({ 0.f, i * spacing });
	}

	return out; // Return the positions
}

std::vector<sf::Vector2f> Formation::Grid(int cols, int rows, float spacing) {
	std::vector<sf::Vector2f> out; // Vector to store positions
	
	for (int x = 0; x < cols; x++) {
		for (int y = 0; y < rows; y++) {
			//float ts = 256.f;
			out.push_back({ x * spacing, y * spacing }); // col, row offset 
		}
	}

	return out;
}


/* -----------------
	V FORMATION
	Creates a V-shaped formation of enemies
	center = pos of middle of the V
	count = num of enemies
	spacing = dist between each enemy
   ----------------- */

std::vector<sf::Vector2f> Formation::V(int count, float spacing) {
	std::vector<sf::Vector2f> out; // Vector to store positions

	float half = (count - 1) / 2.f; // middle index float for symmetry

	for (int i = 0; i < count; i++) {
		// Calculate horizontal offset from the center
		float offsetX = (i - half) * spacing;
		float offsetY = std::abs(offsetX); // V shape downward

		// The Y position is offset to form the V shape
		out.push_back({ offsetX, offsetY });
	}

	return out; // Return the positions
}

