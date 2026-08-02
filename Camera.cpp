#include "../h/Camera.h"

Camera::Camera(sf::RenderWindow& window)
	// Sets view to the default window view
	: view(window.getDefaultView())
{
	// curr and targ zoom are already init.

}

// Smoothly update camera each frame
void Camera::update(float dt) {
	float speed = 3.f; // Controls how fast camera zoom interpolates

	// Gradually move currentZoom towards targetZoom
	// The difference is multiplied by speed * dt for smooth interpolation
	currentZoom += (targetZoom - currentZoom) * speed * dt;


	// Get current view size
	sf::Vector2f size = view.getSize();

	// Multiply the size by currentZoom to apply the zoom
	// Larger size = zoomed out, smaller size = zoomed in
	view.setSize({ size.x * currentZoom, size.y * currentZoom });
}

// Set the target zoom (camera will smoothly move towards this value)
void Camera::zoomTo(float zoom) {
	targetZoom = zoom;
}

// Reset the camera to normal zoom (1.0 = default)
void Camera::reset() {
	targetZoom = 1.f; // Set target zoom back to default
}

// Return a refernce to the internal view so it can be applied to the window
sf::View& Camera::getView() {
	return view;
}