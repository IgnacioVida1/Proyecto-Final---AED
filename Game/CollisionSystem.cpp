//
// Created by ignac on 9/02/2026.
//

#include "CollisionSystem.h"
#include "../Entity/Entities.h"
#include <cmath>

CollisionSystem::CollisionSystem(const GameConfig& c_) : config(c_), WorldBounds(0, 0, c_.WorldWidth, c_.WorldHeight),
                                                         activeEntitiesRef(nullptr) {

    quad = new Quad(0, WorldBounds, config.quadCapacity);
    lastTime = std::chrono::high_resolution_clock::now();
}

CollisionSystem::~CollisionSystem() {

    delete quad;
}

void CollisionSystem::setActiveEntities(std::vector<GameEntity*>& entities) {

    activeEntitiesRef = &entities;
}

void CollisionSystem::update() {

    auto startTime = std::chrono::high_resolution_clock::now();

    if (!activeEntitiesRef || activeEntitiesRef->empty()) {

        return;
    }

    stats.resetFrame();
    stats.activeEntities = 0;

    for (auto e : *activeEntitiesRef) {

        if (e->active) stats.activeEntities++;
    }

    auto rebuildStart = std::chrono::high_resolution_clock::now();

    rebuildQuad();

    stats.quadNodes = quad->countNodes();

    auto rebuildEnd = std::chrono::high_resolution_clock::now();

    auto collisionStart = std::chrono::high_resolution_clock::now();

    auto collisions = detectCollisions();

    auto collisionEnd = std::chrono::high_resolution_clock::now();

    resolveCollisions(collisions);

    auto endTime = std::chrono::high_resolution_clock::now();

    stats.quadRebuildTime = std::chrono::duration<float, std::milli>(rebuildEnd - rebuildStart).count();
    stats.collisionDetectionTime = std::chrono::duration<float, std::milli>(collisionEnd - collisionStart).count();
    stats.updateTime = std::chrono::duration<float, std::milli>(endTime - startTime).count();

    stats.framesProcessed++;

    // if (stats.framesProcessed % 60 == 0) printPerformanceStats();
}

void CollisionSystem::rebuildQuad() {

    quad->clear();

    for (auto e : *activeEntitiesRef) {

        if (e->active) quad->insert(e);
    }

    stats.quadNodes = quad->countNodes();
}

std::vector<std::pair<GameEntity*, GameEntity*>> CollisionSystem::detectCollisions() {

    collisions.clear();

    int totalCandidates = 0;
    float avgCandidates = 0;
    int entityCount = 0;

    for (auto e : *activeEntitiesRef) {

        if (!e->active) continue;

        Rect searchArea = createSearchArea(e);

        candidates.clear();
        quad->queryRange(searchArea, candidates);

        totalCandidates += candidates.size();
        entityCount++;

        /* if (e->type == EntityType::PLAYER) {
            std::cout << "Jugador: radio=" << e->radius
                      << ", área=" << searchArea.width << "x" << searchArea.height
                      << ", candidatos=" << candidates.size() << std::endl;
        } */

        for (auto candidate: candidates) {

            if (candidate == e || !candidate->active) continue;

            stats.quadComparisons++;

            if (checkCollisions(e, candidate)) {
                collisions.emplace_back(e, candidate);
            }
        }
    }

    if (entityCount > 0) {
        avgCandidates = totalCandidates / (float)entityCount;
        // std::cout << "Promedio candidatos por entidad: " << avgCandidates << std::endl;
    }

    stats.collisionsDetected = static_cast<int>(collisions.size());

    return collisions;
}

bool CollisionSystem::checkCollisions(GameEntity *a, GameEntity *b) {

    Rect boxA = a->getBoundingBox();
    Rect boxB = b->getBoundingBox();

    return !(boxA.x + boxA.width < boxB.x ||
             boxB.x + boxB.width < boxA.x ||
             boxA.y + boxA.height < boxB.y ||
             boxB.y + boxB.height < boxA.y);
}


bool CollisionSystem::checkCollisionsCircle(GameEntity* a, GameEntity* b) {

    float dx = a->position.x - b->position.x;
    float dy = a->position.y - b->position.y;
    float dist = dx * dx + dy * dy;
    float minDist = a->radius + b->radius;

    return dist < (minDist * minDist);
}

Rect CollisionSystem::createSearchArea(GameEntity* entity) {

    Rect bbox = entity->getBoundingBox();

    return Rect(
        bbox.x - config.CollisionPadding,
        bbox.y - config.CollisionPadding,
        bbox.width + ( 2 * config.CollisionPadding ),
        bbox.height + ( 2 * config.CollisionPadding)
    );
}

void CollisionSystem::resolveCollisions(const std::vector<std::pair<GameEntity*, GameEntity*>>& collisions) {

    for (const auto& c : collisions) {
        GameEntity* a = c.first;
        GameEntity* b = c.second;

        if (!a->active || !b->active) continue;

        if (a->canEat(b)) {
            a->eat(b);
        } else if (b->canEat(a)) {
            b->eat(a);
        } else {
            float dx = a->position.x - b->position.x;
            float dy = a->position.y - b->position.y;
            float dist = sqrt(dx*dx + dy*dy);
            if (dist > 0) {
                float overlap = (a->radius + b->radius) - dist;
                float nx = dx / dist;
                float ny = dy / dist;

                if (a->type == EntityType::PLAYER || a->type == EntityType::BOT) {
                    a->position.x += nx * overlap * 0.5f;
                    a->position.y += ny * overlap * 0.5f;
                }
                if (b->type == EntityType::PLAYER || b->type == EntityType::BOT) {
                    b->position.x -= nx * overlap * 0.5f;
                    b->position.y -= ny * overlap * 0.5f;
                }

                float aMass = (a->type == EntityType::PELLET) ? PELLET_MASS_FACTOR : a->mass;
                float bMass = (b->type == EntityType::PELLET) ? PELLET_MASS_FACTOR : b->mass;

                a->velX += nx * COLLISION_REBOUND_FORCE * (bMass / aMass);
                a->velY += ny * COLLISION_REBOUND_FORCE * (bMass / aMass);
                b->velX -= nx * COLLISION_REBOUND_FORCE * (aMass / bMass);
                b->velY -= ny * COLLISION_REBOUND_FORCE * (aMass / bMass);

                float aSpeed = sqrt(a->velX*a->velX + a->velY*a->velY);
                if (aSpeed > a->maxSpeed * MAX_SPEED_MULTIPLIER_COLLISION) {
                    a->velX = (a->velX / aSpeed) * a->maxSpeed * 2;
                    a->velY = (a->velY / aSpeed) * a->maxSpeed * 2;
                }

                float bSpeed = sqrt(b->velX*b->velX + b->velY*b->velY);
                if (bSpeed > b->maxSpeed * MAX_SPEED_MULTIPLIER_COLLISION) {
                    b->velX = (b->velX / bSpeed) * b->maxSpeed * 2;
                    b->velY = (b->velY / bSpeed) * b->maxSpeed * 2;
                }
            }
        }

        a->lastCollisionFrame = stats.framesProcessed;
        b->lastCollisionFrame = stats.framesProcessed;
    }
}

std::vector<GameEntity*> CollisionSystem::queryArea(const Rect& area) {

    std::vector<GameEntity*> results;

    quad->queryRange(area, results);

    return results;
}

std::vector<GameEntity*> CollisionSystem::queryNearby(const Point& position, float radius) {

    Rect area(position.x - radius, position.y - radius, radius * 2, radius * 2);

    return queryArea(area);
}

const PerformanceStats& CollisionSystem::getStats() const { return stats; }
const GameConfig& CollisionSystem::getConfig() const { return config; }
Quad* CollisionSystem::getQuad() const { return quad; }
void CollisionSystem::setQuadCapacity(int capacity) {
    config.quadCapacity = capacity;
    delete quad;
    quad = new Quad(0, WorldBounds, config.quadCapacity);
}
void CollisionSystem::setMaxQuadLevels(int levels) {
    config.maxQuadLevels = levels;
    quad->MAX_LEVELS = levels;
}
void CollisionSystem::printPerformanceStats() const {

    std::cout << "\n=== ESTADÍSTICAS DE RENDIMIENTO ===" << std::endl;
    std::cout << "Frames procesados: " << stats.framesProcessed << std::endl;
    std::cout << "Entidades activas: " << stats.activeEntities << std::endl;
    std::cout << "\nTIEMPOS (ms):" << std::endl;
    std::cout << "  Rebuild QuadTree: " << stats.quadRebuildTime << std::endl;
    std::cout << "  Detección colisiones: " << stats.collisionDetectionTime << std::endl;
    std::cout << "  Total update: " << stats.updateTime << std::endl;

    if (stats.updateTime > 0) {
        std::cout << "\nFPS estimado: " << (1000.0f / stats.updateTime) << std::endl;
    }

    std::cout << "\nCOMPARACIONES:" << std::endl;
    std::cout << "  QuadTree: " << stats.quadComparisons << std::endl;


    std::cout << "\nMEMORIA:" << std::endl;
    std::cout << "  Nodos QuadTree: " << stats.quadNodes << std::endl;
    std::cout << "  Colisiones este frame: " << stats.collisionsDetected << std::endl;
}