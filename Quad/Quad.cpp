//
// Created by ignac on 9/02/2026.
//

#include "Quad.h"
#include "../Entity/Entities.h"
#include <iostream>
#include <algorithm>

Quad::~Quad() {
    clear();
}

void Quad::clear() {

    entities.clear();

    if ( subdivided ) {
        delete TopLeft;
        delete TopRight;
        delete BotLeft;
        delete BotRight;

        TopLeft = TopRight = BotLeft = BotRight = nullptr;
        subdivided = false;
    }
}

void Quad::updateEntity(GameEntity* e) {

    if (!bounds.contains(e->position)) { return; }

    removeEntity(e);
    insert(e);
}

void Quad::removeEntity(GameEntity* e) {

    auto it = std::find(entities.begin(), entities.end(), e);
    if (it != entities.end()) {

        entities.erase(it);
        return;
    }

    if (subdivided) {
        TopLeft->removeEntity(e);
        TopRight->removeEntity(e);
        BotLeft->removeEntity(e);
        BotRight->removeEntity(e);
    }
}

void Quad::subdivide() {

    if ( subdivided  || level >= QUADTREE_MAX_LEVELS ) { return; }

    float halfWidth = bounds.width / 2.0f;
    float halfHeight = bounds.height / 2.0f;
    float x = bounds.x;
    float y = bounds.y;

    TopLeft = new Quad(level + 1, Rect(x, y, halfWidth, halfHeight), capacity);

    TopRight = new Quad(level + 1, Rect(x + halfWidth, y, halfWidth, halfHeight), capacity);

    BotLeft = new Quad(level + 1, Rect(x, y + halfHeight, halfWidth, halfHeight), capacity);

    BotRight = new Quad(level + 1, Rect(x + halfWidth, y + halfHeight, halfWidth, halfHeight), capacity);

    subdivided = true;

    std::vector<GameEntity*> temp = entities;
    entities.clear();

    for (auto entity : temp) {
        bool inserted = false;
        if (TopLeft->bounds.intersects(entity->getBoundingBox())) {
            TopLeft->insert(entity);
            inserted = true;
        }
        if (TopRight->bounds.intersects(entity->getBoundingBox())) {
            TopRight->insert(entity);
            inserted = true;
        }
        if (BotLeft->bounds.intersects(entity->getBoundingBox())) {
            BotLeft->insert(entity);
            inserted = true;
        }
        if (BotRight->bounds.intersects(entity->getBoundingBox())) {
            BotRight->insert(entity);
            inserted = true;
        }
        if (!inserted) {
            entities.push_back(entity);
        }
    }
}

bool Quad::insert(GameEntity* e) {

    if ( !bounds.intersects(e->getBoundingBox()) ) { return false; }

    if ( !subdivided && entities.size() < capacity ) {
        entities.push_back(e);
        return true;
    }

    if ( level >= QUADTREE_MAX_LEVELS ) {
        entities.push_back(e);
        return true;
    }

    if ( !subdivided ) { subdivide(); }

    bool inserted = false;
    if (TopLeft->bounds.intersects(e->getBoundingBox())) {
        inserted |= TopLeft->insert(e);
    }
    if (TopRight->bounds.intersects(e->getBoundingBox())) {
        inserted |= TopRight->insert(e);
    }
    if (BotLeft->bounds.intersects(e->getBoundingBox())) {
        inserted |= BotLeft->insert(e);
    }
    if (BotRight->bounds.intersects(e->getBoundingBox())) {
        inserted |= BotRight->insert(e);
    }

    if (!inserted) {
        entities.push_back(e);
    }


    return inserted;
}

void Quad::queryRange(const Rect& range, std::vector<GameEntity*>& result) {

    if ( !bounds.intersects(range) ) { return; }

    for ( auto e : entities ) {
        if ( e->active && range.intersects(e->getBoundingBox()) ) {
            result.push_back(e);
        }
    }

    if ( subdivided ) {
        TopLeft->queryRange(range, result);
        TopRight->queryRange(range, result);
        BotLeft->queryRange(range, result);
        BotRight->queryRange(range, result);
    }
}

void Quad::queryPoint(const Point& point, std::vector<GameEntity*>& result) {

    if ( !bounds.contains(point) ) { return; }

    for ( auto e : entities ) {
        if (e->active) {
            Rect bbox = e->getBoundingBox();
            if ( bbox.contains(point) ) {
                result.push_back(e);
            }
        }
    }

    if ( subdivided ) {
        TopLeft->queryPoint(point, result);
        TopRight->queryPoint(point, result);
        BotLeft->queryPoint(point, result);
        BotRight->queryPoint(point, result);
    }
}

void Quad::getAllEntities(std::vector<GameEntity*>& allEntities) {

    for (auto e : entities) {
        if ( e->active ) { allEntities.push_back(e); }
    }

    if ( subdivided ) {
        TopLeft->getAllEntities(allEntities);
        TopRight->getAllEntities(allEntities);
        BotLeft->getAllEntities(allEntities);
        BotRight->getAllEntities(allEntities);
    }
}

void Quad::draw() {

    std::cout << "Nodo en nivel " << level << ": " << entities.size() << "entidades" << std::endl;
    std::cout << "Limites: (" << bounds.x << ", " << bounds.y << ") [" << bounds.width << " x " << bounds.height << "]" << std::endl;

    if ( subdivided ) {
        TopLeft->draw();
        TopRight->draw();
        BotLeft->draw();
        BotRight->draw();
    }
}

const Rect& Quad::getBounds() const { return bounds; }
int Quad::getLevel() const { return level; }
bool Quad::isSubdivided() const { return subdivided; }
int Quad::getEntityCount() const { return entities.size(); }

int Quad::countNodes() const {

    int count = 1;

    if ( subdivided ) {
        count += TopLeft->countNodes();
        count += TopRight->countNodes();
        count += BotLeft->countNodes();
        count += BotRight->countNodes();
    }
    return count;
}