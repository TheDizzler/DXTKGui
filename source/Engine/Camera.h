#pragma once

#include <d3d11_1.h>
#include <SimpleMath.h>

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

/** Based on
https://roguesharp.wordpress.com/2014/07/13/tutorial-5-creating-a-2d-camera-with-pan-and-zoom-in-monogame/ */
class Camera {
public:
	Camera(int viewportWidth, int viewportHeight);
	Camera(const Vector2& viewport);
	virtual ~Camera();

	void updateViewport(const Vector2& viewportArea,
		const Vector2& viewportPosition, bool zoomToFit = false);


	//Viewport* viewport;


	float rotation = 0.0f;

	Vector2 viewportPosition;
	int viewportWidth;
	int viewportHeight;
	Vector3 viewportCenter;


	void adjustZoom(float amount);
	/** Move the camera in an X and Y amount based on the cameraMovement param.
	*	If clampToMap is true the camera will try not to pan outside of the
	*	bounds of the map. */
	void moveCamera(Vector2 cameraMovement, bool clampToArea = false);

	RECT* viewportWorldBoundary();

	void centerOn(Vector2 pos, bool clampToArea = false);

	Matrix translationMatrix();
	Vector2& worldToScreen(Vector2 worldPosition);
private:

	Vector2 position;

	float zoom;
	float buildingWidth;
	float buildingHeight;

	float viewX = (viewportWidth / zoom / 2);
	float viewY = (viewportHeight / zoom / 2);

	Vector2* screenToWorld(Vector2 screenPosition);

};