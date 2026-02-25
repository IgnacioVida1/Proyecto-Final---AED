//
// Created by ignac on 15/02/2026.
//

#ifndef ENTITIES_H
#define ENTITIES_H

#include "Entity.h"
#include <string>
#include <vector>

class CollisionSystem;
struct Color;

enum class EntityType {

    PLAYER,
    BOT,
    PELLET,
};

enum class BotState {

    WANDER,
    CHASE,
    FLEE,
    SEEK_FOOD
};

class GameEntity : public Entity {

protected:
    float baseRadius = 10.0f;

    Point targetPosition = Point(0, 0);
    bool hasTarget = false;

    public:
    EntityType type;
    std::string name;

    float mass = 1.0f;
    float velX = 0.0f;
    float velY = 0.0f;
    float speed = 0.0f;
    float maxSpeed = 5.0f;
    int lastCollisionFrame = 0;

    int score = 0;

    GameEntity(EntityType t, const std::string &n = "");

    virtual void update(float deltatime);

    void updateAnimations(float deltatime);

    void moveTowards(const Point& target, float speedMultiplier = 1.0f);

    bool canEat(const GameEntity* other) const;

    void eat(GameEntity* other);

    virtual std::pair<Color, Color> getColor() const;

    EntityType getType() const { return type; }
};


class Player : public GameEntity {

    public:
    Player();

    void update(float deltatime) override;
};

struct PriorityTarget {
    GameEntity* entity;
    float priority;
    float distance;

    PriorityTarget(GameEntity* e = nullptr, float p = 0, float d = 0)
        : entity(e), priority(p), distance(d) {}

    bool operator<(const PriorityTarget& other) const {
        return priority < other.priority;
    }

    bool operator>(const PriorityTarget& other) const {
        return priority > other.priority;
    }
};

class Bot : public GameEntity {

    CollisionSystem* collisionSystem;

    BotState state;
    GameEntity* targetEntity;

    float decisionTimer = 0.0f;
    float decisionInterval = 2.0f;

    Point fleeTarget;

    const float VIEW_RADIUS = BOT_VIEW_RADIUS;
    const float FLEE_RADIUS = BOT_FLEE_RADIUS;
    const float CHASE_RADIUS = BOT_CHASE_RADIUS;
    const float SAFE_DISTANCE = BOT_SAFE_DISTANCE;

    public:
    Bot(int id);

    void update(float deltatime, const std::vector<GameEntity*>& allEntities, GameEntity* player);

    void analyzeEnvironment(const std::vector<GameEntity*>& allEntities, GameEntity* player);

    void executeBehavior(float deltatime);

    void chooseNewTarget();

    BotState getState() const { return state; }
    void setState(BotState newState) { state = newState; }
    void setCollisionSystem(CollisionSystem* cs) { collisionSystem = cs; }

    std::pair<Color, Color> getColor() const override;
};

class Pellet : public GameEntity {

    public:
    Pellet();

    void update(float deltatime) override;

    std::pair<Color, Color> getColor() const override;
};

#endif //ENTITIES_H
