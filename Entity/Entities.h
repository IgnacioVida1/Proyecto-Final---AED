//
// Created by ignac on 15/02/2026.
//

#ifndef ENTITIES_H
#define ENTITIES_H

#include "Entity.h"
#include <string>

class CollisionSystem;
struct Color;

// Tipos de entidades
enum class EntityType {

    PLAYER,
    BOT,
    PELLET,
};

// Estados de BOT
enum class BotState {

    WANDER,
    CHASE,
    FLEE,
    SEEK_FOOD
};

// Entidad dentro del juego
class GameEntity : public Entity {

protected:
    float baseRadius = 10.0f; // Radio base

    Point targetPosition = Point(0, 0); // Posicion de objetivo
    bool hasTarget = false; // Tiene objetivo?

    public:
    EntityType type; // Tipo de entidad
    std::string name;

    // Stats clave
    float mass = 1.0f;
    float velX = 0.0f;
    float velY = 0.0f;
    float speed = 0.0f;
    float maxSpeed = 5.0f;
    int lastCollisionFrame = 0;

    int score = 0;

    GameEntity(EntityType t, const std::string &n = "");

    // Actualizar entidad por frame
    virtual void update(float deltatime);

    // Actualizar animacion de la entidad por frame
    void updateAnimations(float deltatime);

    // Moverse en direccion a un objetivo
    void moveTowards(const Point& target, float speedMultiplier = 1.0f);

    // Se puede comer?
    bool canEat(const GameEntity* other) const;

    // Accion de comer
    void eat(GameEntity* other);

    // Obtener colores
    virtual pair<Color, Color> getColor() const;

    // Obtener tipo
    EntityType getType() const { return type; }
};


class Player : public GameEntity {

    public:
    Player();

    void update(float deltatime) override;
};

// Entidad con prioridad para bots
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

    CollisionSystem* collisionSystem; // Sistema de colisiones

    BotState state; // Estado de bot
    GameEntity* targetEntity; // Objetivo

    float decisionTimer = 0.0f;
    float decisionInterval = 2.0f;

    Point fleeTarget;

    // Constantes para vision
    const float VIEW_RADIUS = BOT_VIEW_RADIUS;
    const float FLEE_RADIUS = BOT_FLEE_RADIUS;
    const float CHASE_RADIUS = BOT_CHASE_RADIUS;
    const float SAFE_DISTANCE = BOT_SAFE_DISTANCE;

    public:
    Bot(int id);

    void update(float deltatime, const vector<GameEntity*>& allEntities, GameEntity* player);

    // Analizar alrededores
    void analyzeEnvironment(const vector<GameEntity*>& allEntities, GameEntity* player);

    // Actuar segun ambiente
    void executeBehavior(float deltatime);

    // Escoger nuevo objetivo
    void chooseNewTarget();
    BotState getState() const { return state; }
    void setState(BotState newState) { state = newState; }
    void setCollisionSystem(CollisionSystem* cs) { collisionSystem = cs; }

    pair<Color, Color> getColor() const override;
};

class Pellet : public GameEntity {

    public:
    Pellet();

    void update(float deltatime) override;

    pair<Color, Color> getColor() const override;
};

#endif //ENTITIES_H
