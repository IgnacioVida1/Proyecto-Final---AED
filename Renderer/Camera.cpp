//
// Created by ignac on 22/02/2026.
//

#include "Camera.h"
#include <algorithm>

void Camera::clampPosition() {

    float viewWidth = viewportWidth / zoomLevel;
    float viewHeight = viewportHeight / zoomLevel;

    float minX = viewWidth / 2.0f;
    float maxX = worldWidth - viewWidth / 2.0f;
    float minY = viewHeight / 2.0f;
    float maxY = worldHeight - viewHeight / 2.0f;

    if (position.x < minX) position.x = minX;
    if (position.x > maxX) position.x = maxX;
    if (position.y < minY) position.y = minY;
    if (position.y > maxY) position.y = maxY;
}

void Camera::updateBounds() {

    float viewWidth = viewportWidth / zoomLevel;
    float viewHeight = viewportHeight / zoomLevel;
    bounds = Rect(
        position.x - viewWidth / 2.0f,
        position.y - viewHeight / 2.0f,
        viewWidth,
        viewHeight
    );
}

Camera::Camera(int viewW, int viewH, float worldW, float worldH) : viewportWidth(viewW), viewportHeight(viewH),
worldWidth(worldW), worldHeight(worldH), position(Point(worldW / 2.0f, worldH / 2.0f)), zoomLevel(1.0f), targetZoomLevel(1.0f),
          minZoom(CAMERA_MIN_ZOOM), maxZoom(CAMERA_MAX_ZOOM), zoomSpeed(CAMERA_ZOOM_SPEED) {

    updateBounds();
}

void Camera::update(const Point& target, float playerRadius) {

    calculateZoom(playerRadius);

    zoomLevel += (targetZoomLevel - zoomLevel) * zoomSpeed;

    position.x += (target.x - position.x) * followSpeed;
    position.y += (target.y - position.y) * followSpeed;

    clampPosition();
    updateBounds();
}

void Camera::calculateZoom(float playerRadius) {

    const float IDEAL_PLAYER_SIZE_RATIO = CAMERA_IDEAL_PLAYER_SIZE_RATIO;

    float desiredPlayerScreenSize = std::min(viewportWidth, viewportHeight) * IDEAL_PLAYER_SIZE_RATIO;

    float idealZoom = desiredPlayerScreenSize / (playerRadius * 2);

    targetZoomLevel = std::max(minZoom, std::min(maxZoom, idealZoom));
}

void Camera::setPosition(const Point& newPos) {

    position = newPos;
    clampPosition();
    updateBounds();
}

Point Camera::worldToScreen(const Point& worldPoint) const {

    return Point(
        (worldPoint.x - position.x) * zoomLevel + viewportWidth / 2.0f,
        (worldPoint.y - position.y) * zoomLevel + viewportHeight / 2.0f
        );
}

Point Camera::screenToWorld(const Point& screenPoint) const {
    return Point(
        (screenPoint.x + position.x - viewportWidth/ 2.0f) / zoomLevel + position.x,
        (screenPoint.y + position.y - viewportHeight / 2.0f) / zoomLevel + position.y
        );
}

Rect Camera::getViewRect() const {

    float viewWidth = viewportWidth / zoomLevel;
    float viewHeight = viewportHeight / zoomLevel;
    return Rect(
        position.x - viewWidth / 2.0f,
        position.y - viewHeight / 2.0f,
        viewWidth,
        viewHeight
    );
}

bool Camera::isVisible(const Point& worldPoint, float margin) const {

    float viewWidth = viewportWidth / zoomLevel;
    float viewHeight = viewportHeight / zoomLevel;
    float left = position.x - viewWidth / 2.0f - margin;
    float right = position.x + viewWidth / 2.0f + margin;
    float top = position.y - viewHeight / 2.0f - margin;
    float bottom = position.y + viewHeight / 2.0f + margin;

    return (worldPoint.x >= left && worldPoint.x <= right &&
            worldPoint.y >= top && worldPoint.y <= bottom);
}

bool Camera::isRectVisible(const Rect& worldRect) const {

    return getViewRect().intersects(worldRect);
}