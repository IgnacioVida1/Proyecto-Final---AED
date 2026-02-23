//
// Created by ignac on 22/02/2026.
//

#ifndef CAMERA_H
#define CAMERA_H

#include "../Quad/Point.h"
#include "../Quad/Rect.h"

class Camera {

    Point position;
    int viewportWidth;
    int viewportHeight;
    float worldWidth;
    float worldHeight;

    float zoomLevel;
    float targetZoomLevel;
    float minZoom;
    float maxZoom;
    float zoomSpeed;

    float followSpeed = 0.07f;
    Rect bounds;

    void clampPosition();

    void updateBounds();

public:
    Camera(int viewW, int viewH, float worldW, float worldH);

    void update(const Point& target, float playerRadius);

    void calculateZoom(float playerRadius);

    void setPosition(const Point& newPos);

    void setMinZoom(float min) { minZoom = min; }
    void setMaxZoom(float max) { maxZoom = max; }
    void setZoomSpeed(float speed) { zoomSpeed = speed; }

    void setFollowSpeed(float speed) { followSpeed = speed; }

    Point worldToScreen(const Point& worldPoint) const;

    Point screenToWorld(const Point& screenPoint) const;

    Rect getViewRect() const;

    bool isVisible(const Point& worldPoint, float margin = 0) const;

    bool isRectVisible(const Rect& worldRect) const;

    float worldToScreenSize(float worldSize) const {
        return worldSize * zoomLevel;
    }

    const Point& getPosition() const { return position; }

    const Rect& getBounds() const { return bounds; }

    float getZoomLevel() const { return zoomLevel; }
    float getTargetZoomLevel() const { return targetZoomLevel; }

};



#endif //CAMERA_H
