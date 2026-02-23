//
// Created by ignac on 15/02/2026.
//

#include "Entities.h"

#include <algorithm>

#include "../Renderer/Renderer.h"
#include <cmath>

#include "../Game/CollisionSystem.h"


GameEntity::GameEntity(EntityType t, const std::string &n) : type(t), name(n) {
    radius = baseRadius;
}

void GameEntity::update(float deltatime) {

    position.x += velX * deltatime * 60.0f;
    position.y += velY * deltatime * 60.0f;

    radius = baseRadius * sqrtf(mass);

    float currentSpeed = sqrtf(velX * velX + velY * velY);
    if (currentSpeed > maxSpeed) {
        velX = (velX / currentSpeed) * maxSpeed;
        velY = (velY / currentSpeed) * maxSpeed;
    }

    velX *= MOVEMENT_FRICTION;
    velY *= MOVEMENT_FRICTION;

    updateAnimations(deltatime);
}

void GameEntity::updateAnimations(float deltatime) {

    if (justAte) {

        ateTimer += deltatime;
        pulseScale = 1.0f + (1.0f / (ateTimer + 1));

        if (ateTimer > 0.3f) {
            justAte = false;
            ateTimer = 0.0f;
            pulseScale = 1.0f;
        }
    }

    if (velX != 0 || velY != 0) {
        rotationAngle += sqrt(velX*velX + velY*velY) * deltatime * 2.0f;
    }
}


void GameEntity::moveTowards(const Point& target, float speedMultiplier) {

    float dx = target.x - position.x;
    float dy = target.y - position.y;
    float distance = sqrtf(dx * dx + dy * dy);

    if (distance > MOVE_TOWARDS_THRESHOLD) {

        float desiredVelX = (dx / distance) * maxSpeed * speedMultiplier;
        float desiredVelY = (dy / distance) * maxSpeed * speedMultiplier;

        velX += (desiredVelX - velX) * MOVEMENT_ACCELERATION;
        velY += (desiredVelY - velY) * MOVEMENT_ACCELERATION;

        float currentSpeed = sqrtf(velX*velX + velY*velY);
        float maxAllowedSpeed = maxSpeed * speedMultiplier;

        if (currentSpeed > maxAllowedSpeed) {
            velX = (velX / currentSpeed) * maxAllowedSpeed;
            velY = (velY / currentSpeed) * maxAllowedSpeed;
        }
    }
    else if (distance < MOVE_TOWARDS_STOP_DISTANCE){
        velX = 0;
        velY = 0;
        hasTarget = false;
    } else {
        velX = (dx / distance) * maxSpeed * speedMultiplier * MOVE_TOWARDS_SLOW_FACTOR;
        velY = (dy / distance) * maxSpeed * speedMultiplier * MOVE_TOWARDS_SLOW_FACTOR;
    }
}

bool GameEntity::canEat(const GameEntity* other) const {

    float myArea = (radius * 2) * (radius * 2);
    float otherArea = (other->radius * 2) * (other->radius * 2);
    return (myArea > otherArea * EAT_SIZE_COMPARISON_THRESHOLD);
}

void GameEntity::eat(GameEntity* other) {

    if (canEat(other)) {

        mass += other->mass * EAT_MASS_GAIN_FACTOR;
        score += static_cast<int>(other->mass * EAT_SCORE_MULTIPLIER);
        other->active = false;

        justAte = true;
        ateTimer = 0.0f;
        pulseScale = 1.2f;
    }
}

std::pair<Color, Color> GameEntity::getColor() const {

    switch (type) {
        case EntityType::PLAYER: return { Color::Green(), Color(0, 200, 0, 255) };
        case EntityType::BOT: return {Color::Red(), Color(200, 0, 0, 255)};
        case EntityType::PELLET: return {Color::Yellow(), Color(200, 200, 0, 255)};
        default: return {Color::White(), Color(200, 200, 200, 255)};
    }
}


Player::Player() : GameEntity(EntityType::PLAYER, "Player") {

    mass = PLAYER_INITIAL_MASS;
    baseRadius = PLAYER_BASE_RADIUS;
    maxSpeed = PLAYER_MAX_SPEED;
    id = PLAYER_ID;
}

void Player::update(float deltatime) {
    GameEntity::update(deltatime);
}



Bot::Bot(int id_) : GameEntity(EntityType::BOT, "Bot_" + std::to_string(id)), fleeTarget(Point(0, 0)) {
    mass = BOT_INITIAL_MASS_MIN + (rand() % 100) / 100.0f;
    baseRadius = BOT_BASE_RADIUS_MIN + rand() % 5;
    maxSpeed = BOT_MAX_SPEED;

    position.x = WORLD_BOT_SPAWN_MARGIN + rand() % (int)(WORLD_WIDTH - WORLD_BOT_SPAWN_MARGIN * 2);
    position.y = WORLD_BOT_SPAWN_MARGIN + rand() % (int)(WORLD_HEIGHT - WORLD_BOT_SPAWN_MARGIN * 2);

    decisionInterval = BOT_DECISION_INTERVAL;
    id = id_;
    state = BotState::WANDER;
    targetEntity = nullptr;

    chooseNewTarget();
}

void Bot::update(float deltatime, const std::vector<GameEntity*>& allEntities, GameEntity* player) {

    GameEntity::update(deltatime);

    decisionTimer += deltatime;

    if (decisionTimer > decisionInterval) {
        analyzeEnvironment(allEntities, player);
        decisionTimer = 0.0f;
    }

    executeBehavior(deltatime);
}

void Bot::analyzeEnvironment(const std::vector<GameEntity *> &allEntities, GameEntity *player) {

    if (!collisionSystem) { return; }

    std::vector<PriorityTarget> threats;
    std::vector<PriorityTarget> prey;
    std::vector<PriorityTarget> food;

    Rect viewArea(position.x - VIEW_RADIUS, position.y - VIEW_RADIUS, VIEW_RADIUS * 2, VIEW_RADIUS * 2);

    auto nearby = collisionSystem->queryArea(viewArea);

    for (auto entity : nearby) {

        if (!entity->active || entity == this) continue;

        float dx = entity->position.x - position.x;
        float dy = entity->position.y - position.y;
        float distance = sqrt(dx * dx + dy * dy);

        if (distance > VIEW_RADIUS) continue;

        if (entity->type == EntityType::PLAYER) {
            if (entity->mass > mass * BOT_SIZE_COMPARISON_THRESHOLD) {
                float priority = (entity->mass / mass) * (VIEW_RADIUS / distance);
                threats.emplace_back(entity, priority, distance);
            } else if (mass > entity->mass * BOT_SIZE_COMPARISON_THRESHOLD) {
                float priority = (mass / entity->mass) * (VIEW_RADIUS / distance);
                prey.emplace_back(entity, priority, distance);
            }
        }
        else if (entity->type == EntityType::BOT && entity != this) {
            Bot* otherBot = dynamic_cast<Bot*>(entity);
            if (otherBot) {
                if (otherBot->mass > mass * BOT_SIZE_COMPARISON_THRESHOLD) {
                    float priority = (otherBot->mass / mass) * (VIEW_RADIUS / distance);
                    threats.emplace_back(otherBot, priority, distance);
                } else if (mass > otherBot->mass * BOT_SIZE_COMPARISON_THRESHOLD) {
                    float priority = (mass / otherBot->mass) * (VIEW_RADIUS / distance);
                    prey.emplace_back(otherBot, priority, distance);
                }
            }
        }
        else if (entity->type == EntityType::PELLET) {
            float priority = (VIEW_RADIUS / distance) * BOT_FOOD_PRIORITY_MULTIPLIER;
            food.emplace_back(entity, priority, distance);
        }
    }

    std::sort(threats.begin(), threats.end(), std::greater<PriorityTarget>());
    std::sort(prey.begin(), prey.end(), std::greater<PriorityTarget>());
    std::sort(food.begin(), food.end(), std::greater<PriorityTarget>());

    if (!threats.empty() && threats[0].priority > BOT_THREAT_PRIORITY_THRESHOLD) {
        state = BotState::FLEE;
        targetEntity = threats[0].entity;
    }
    else if (!prey.empty() && prey[0].priority > BOT_PREY_PRIORITY_THRESHOLD) {
        state = BotState::CHASE;
        targetEntity = prey[0].entity;
    }
    else if (!food.empty()) {
        state = BotState::SEEK_FOOD;
        targetEntity = food[0].entity;
    }
    else {
        state = BotState::WANDER;
        targetEntity = nullptr;
    }
}

void Bot::executeBehavior(float deltatime) {

    switch (state) {

        case BotState::FLEE:
            if (targetEntity && targetEntity->active) {
                float dx = position.x - targetEntity->position.x;
                float dy = position.y - targetEntity->position.y;
                float distance = sqrt(dx*dx + dy*dy);

                if (distance < FLEE_RADIUS) {
                    if (distance > 0) {
                        targetPosition.x = position.x + (dx / distance) * SAFE_DISTANCE;
                        targetPosition.y = position.y + (dy / distance) * SAFE_DISTANCE;

                        if (targetPosition.x < WORLD_SPAWN_MARGIN) targetPosition.x = WORLD_SPAWN_MARGIN;
                        if (targetPosition.x > WORLD_WIDTH - WORLD_SPAWN_MARGIN) {
                            targetPosition.x = WORLD_WIDTH - WORLD_SPAWN_MARGIN;
                        }
                        if (targetPosition.y < WORLD_SPAWN_MARGIN) targetPosition.y = WORLD_SPAWN_MARGIN;
                        if (targetPosition.y > WORLD_HEIGHT - WORLD_SPAWN_MARGIN) {
                            targetPosition.y = WORLD_HEIGHT - WORLD_SPAWN_MARGIN;
                        }

                        moveTowards(targetPosition, 1.0f);
                    }
                } else {
                    state = BotState::WANDER;
                }
            } else {
                state = BotState::WANDER;
            }
            break;

        case BotState::CHASE:
            if (targetEntity && targetEntity->active) {
                targetPosition = targetEntity->position;
                moveTowards(targetPosition, 1.0f);
            } else {
                state = BotState::WANDER;
            }
            break;

        case BotState::SEEK_FOOD:
            if (targetEntity && targetEntity->active) {
                targetPosition = targetEntity->position;
                moveTowards(targetPosition, 1.0f);
            } else {
                state = BotState::WANDER;
            }
            break;

        case BotState::WANDER:
            default:
            if (!hasTarget) {
                chooseNewTarget();
            }
        if (hasTarget) {
            moveTowards(targetPosition, 1.0f);
        }
        break;
    }
}


void Bot::chooseNewTarget() {

    int attempts = 0;
    float minDistance = 100.0f;

    do {
        targetPosition.x = rand() % (int)WORLD_WIDTH;
        targetPosition.y = rand() % (int)WORLD_HEIGHT;
        attempts++;

        float dx = targetPosition.x - position.x;
        float dy = targetPosition.y - position.y;
        float distance = sqrt(dx*dx + dy*dy);

        if (distance > minDistance || attempts > 10) {
            hasTarget = true;
            break;
        }
    } while (attempts < 10);
}

std::pair<Color, Color> Bot::getColor() const {
    static Color botColors[] = {
        Color(255, 100, 100, 255),
        Color(255, 150, 50, 255),
        Color(200, 50, 200, 255),
        Color(100, 200, 255, 255)
    };

    static Color botSec[] = {
        Color(190, 100, 100, 255),
        Color(200, 150, 50, 255),
        Color(150, 50, 200, 255),
        Color(100, 130, 255, 255)
    };

    int colorIndex = (id - 1) % 4;
    if (colorIndex < 0) colorIndex = 0;
    return {botColors[colorIndex], botSec[colorIndex]};
}

Pellet::Pellet() : GameEntity(EntityType::PELLET, "Pellet") {

    mass = PELLET_INITIAL_MASS_MIN + (rand() % 100) / 500.0f;
    baseRadius = PELLET_BASE_RADIUS_MIN + rand() % 2;

    position.x = rand() % (int)WORLD_WIDTH;
    position.y = rand() % (int)WORLD_HEIGHT;

}

void Pellet::update(float deltatime) {

    GameEntity::update(deltatime);
}

std::pair<Color, Color> Pellet::getColor() const {

    if (mass < 0.25f) return {Color(255, 255, 100,   255),
                                    Color(210, 210, 100,   255)};
    if (mass < 0.35f) return {Color(255, 200,   50, 255),
                                        Color(200, 150,   50, 255)};
    return {Color(255, 150, 0, 255),
                Color(200, 150, 0, 255)};
}
