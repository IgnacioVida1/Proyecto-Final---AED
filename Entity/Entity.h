//
// Created by ignac on 9/02/2026.
//

#ifndef ENTITY_H
#define ENTITY_H

#include "../Quad/Point.h"
#include "../Quad/Rect.h"

class Entity {
    public:
    virtual ~Entity() = default;

    Point position; // Ubicacion de entidad
    float radius; // Radio de entidad
    bool active; // Activo o inactivo
    int lastCollisionFrame;
    int id;

    // Animaciones
    bool justAte = false;
    float ateTimer = 0.0f;
    float pulseScale = 1.0f;

    // Caja de colisiones de la entidad
    virtual Rect getBoundingBox() const {
        return {position.x - radius, position.y - radius, radius * 2, radius * 2};
    }

    Entity() : position(Point(0, 0)), radius(10.0f), active(true), id(0) {}

};



#endif //ENTITY_H
