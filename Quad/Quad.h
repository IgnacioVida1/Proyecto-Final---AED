//
// Created by ignac on 9/02/2026.
//

#ifndef QUAD_H
#define QUAD_H

#include <vector>

#include "Rect.h"

struct GameEntity;

class Quad {
public:

    Rect bounds;
    int capacity;
    int level;
    std::vector<GameEntity*> entities;

    Quad* TopLeft;
    Quad* TopRight;
    Quad* BotLeft;
    Quad* BotRight;

    bool subdivided;

    int DEFAULT_CAPACITY = 4;
    int MAX_LEVELS = 8;

    Quad(const int l_, const Rect& b_, int c) : level(l_), capacity(c), bounds(b_),
    subdivided(false), TopLeft(nullptr), TopRight(nullptr), BotLeft(nullptr), BotRight(nullptr) { entities.reserve(capacity); }

    ~Quad();

    void clear();

    void updateEntity(GameEntity* e);

    void removeEntity(GameEntity* e);

    void subdivide();

    bool insert(GameEntity* e);

    void queryRange(const Rect& range, std::vector<GameEntity*>& result);

    void queryPoint(const Point& point, std::vector<GameEntity*>& result);

    void getAllEntities(std::vector<GameEntity*>& allEntities);

    void draw();

    const Rect& getBounds() const;
    int getLevel() const;
    bool isSubdivided() const;
    int getEntityCount() const;

    int countNodes() const;
};



#endif //QUAD_H
