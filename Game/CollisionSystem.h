//
// Created by ignac on 9/02/2026.
//

#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "../Quad/Quad.h"
#include "../Entity/Entities.h"
#include <vector>
#include <chrono>
#include <iostream>

struct GameConfig {

    float WorldWidth = WORLD_WIDTH;
    float WorldHeight = WORLD_HEIGHT;

    int quadCapacity = QUADTREE_DEFAULT_CAPACITY;
    int maxQuadLevels = QUADTREE_MAX_LEVELS;

    float CollisionPadding = QUADTREE_COLLISION_PADDING;

    mutable bool showQuad = false;
    bool showCollisions = false;
};

struct PerformanceStats {
    int framesProcessed = 0;
    int totalEntities = 0;
    int activeEntities = 0;

    float updateTime = 0.0f;
    float quadRebuildTime = 0.0f;
    float collisionDetectionTime = 0.0f;
    float renderTime = 0.0f;

    int quadComparisons = 0;
    int collisionsDetected = 0;

    int quadNodes = 0;

    void resetFrame() {
        quadComparisons = 0;
        collisionDetectionTime = 0.0f;
    }

    void printFrameStats() const {

        std::cout << "\n--- Frame " << framesProcessed << " ---" << std::endl;
        std::cout << "Entidades: " << activeEntities << "/" << totalEntities << std::endl;
        std::cout << "Colisiones: " << collisionsDetected << std::endl;
        std::cout << "Comparaciones QuadTree: " << quadComparisons << std::endl;
    }
};

class CollisionSystem {

    private:
    GameConfig config;

    Quad* quad;
    Rect WorldBounds;

    PerformanceStats stats;

    std::chrono::high_resolution_clock::time_point lastTime;

    vector<GameEntity*>* activeEntitiesRef;

    vector<GameEntity*> candidates;
    vector<pair<GameEntity*, GameEntity*>> collisions;

    public:

    CollisionSystem(const GameConfig& c_);

    ~CollisionSystem();

    void setActiveEntities(vector<GameEntity*>& entities);

    void update();

    void rebuildQuad();

    vector<pair<GameEntity*, GameEntity*>> detectCollisions();

    bool checkCollisions(GameEntity* a, GameEntity* b);

    bool checkCollisionsCircle(GameEntity* a, GameEntity* b);

    Rect createSearchArea(GameEntity* entity);

    void resolveCollisions(const vector<pair<GameEntity*, GameEntity*>>& collisions);

    vector<GameEntity*> queryArea(const Rect& area);

    vector<GameEntity*> queryNearby(const Point& position, float radius);
    const PerformanceStats& getStats() const;
    const GameConfig& getConfig() const;
    Quad* getQuad() const;
    void setQuadCapacity(int capacity);
    void setMaxQuadLevels(int levels);
    void printPerformanceStats() const;
};



#endif //COLLISIONSYSTEM_H
