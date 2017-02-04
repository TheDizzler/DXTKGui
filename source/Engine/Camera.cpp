#include "Camera.h"

Camera::Camera(int vwprtWdth, int vwprtHght) {

	zoom = 1.0f;

	viewportWidth = vwprtWdth;
	viewportHeight = vwprtHght;
	viewportCenter = Vector3(viewportWidth * .5, viewportHeight * .5, 0);

	buildingWidth = 100;
	buildingHeight = 50;

	viewX = (viewportWidth) / zoom / 2;
	viewY = (viewportHeight) / zoom / 2;

	position = Vector2::Zero;
}

Camera::Camera(const Vector2& viewport) {

	zoom = 1.0f;

	viewportWidth = viewport.x;
	viewportHeight = viewport.y;
	viewportCenter = Vector3(viewportWidth * .5, viewportHeight * .5, 0);

	buildingWidth = 100;
	buildingHeight = 50;

	viewX = (viewportWidth) / zoom / 2;
	viewY = (viewportHeight) / zoom / 2;

	position = Vector2::Zero;
}

Camera::~Camera() {
}

void Camera::updateViewport(const Vector2& viewport, const Vector2& viewportPos, bool zoomToFit) {

	viewportWidth = viewport.x - viewportPos.x;
	viewportHeight = viewport.y - viewportPos.y;
	viewportPosition = viewportPos;
	viewportCenter = Vector3((viewportWidth) * .5 + viewportPosition.x,
		(viewportHeight) * .5 + viewportPosition.y, 0);

	viewX = (viewportWidth) / zoom / 2;
	viewY = (viewportHeight) / zoom / 2;

}


void Camera::adjustZoom(float amount) {

	zoom += amount;
	if (zoom < 0.2f)
		zoom = 0.2f;
	else if (zoom > 2.5f)
		zoom = 2.5;

	viewX = (viewportWidth / zoom / 2);
	viewY = (viewportHeight / zoom / 2);
}


void Camera::moveCamera(Vector2 cameraMovement, bool clampToArea) {
	position = position + cameraMovement;
}


RECT* Camera::viewportWorldBoundary() {

	Vector2* viewportCorner = screenToWorld(Vector2::Zero);
	Vector2* viewportBottomCorner =
		screenToWorld(Vector2(viewportWidth, viewportHeight));

	RECT* rect = new RECT{
		(int) viewportCorner->x, (int) viewportCorner->y,
		(int) (viewportBottomCorner->x - viewportCorner->x),
		(int) (viewportBottomCorner->y - viewportCorner->y)};

	delete viewportCorner;
	delete viewportBottomCorner;

	return rect;
}


void Camera::centerOn(Vector2 pos, bool clampToArea) {

	position = pos;
}

Vector2& Camera::worldToScreen(Vector2 worldPosition) {
	return Vector2::Transform(worldPosition, translationMatrix());
}

Vector2* Camera::screenToWorld(Vector2 screenPosition) {

	Vector2* vec = new Vector2();
	Vector2::Transform(screenPosition, translationMatrix().Invert(), *vec);

	return vec;
}



Matrix Camera::translationMatrix() {
	// casting to int prevents filtering artifacts??
	return Matrix::CreateTranslation(-(int) position.x, -(int) position.y, 0)
		* Matrix::CreateRotationZ(rotation)
		* Matrix::CreateScale(zoom, zoom, 1)
		* Matrix::CreateTranslation(viewportCenter);
}
