//
// Created by ignac on 15/02/2026.
//

#include "Entities.h"
#include "../Renderer/Renderer.h"
#include <cmath>

#include "../Game/CollisionSystem.h"


GameEntity::GameEntity(EntityType t, const std::string &n) : type(t), name(n) {
    radius = baseRadius;
}

// Actualizar por frame
void GameEntity::update(float deltatime) {

    // Se suma la velocidad en el eje X y el eje Y, multiplciado por deltatime y 60.0f para que este generalizado
    position.x += velX * deltatime * 60.0f;
    position.y += velY * deltatime * 60.0f;

    radius = baseRadius * sqrtf(mass); // Calcular radio en base a la masa y un radio base

    float currentSpeed = sqrtf(velX * velX + velY * velY); // Calcular vector de velocidad actual
    if (currentSpeed > maxSpeed) {
        // Si la velocidad actual es mayor a su velocidad maxima, se limita a un valor menor a la velocidad maxima
        velX = (velX / currentSpeed) * maxSpeed;
        velY = (velY / currentSpeed) * maxSpeed;
    }

    // Se aplica friccion para que no patine
    velX *= MOVEMENT_FRICTION;
    velY *= MOVEMENT_FRICTION;

    // Se actualizan animaciones
    updateAnimations(deltatime);
}

// Actualizar animaciones
void GameEntity::updateAnimations(float deltatime) {

    if (justAte) { // Si se acaba de comer, se aplica la funcion f(x) = 1 + (1/x) para un "pulso"

        ateTimer += deltatime;
        pulseScale = 1.0f + (1.0f / (ateTimer));

        if (ateTimer > 10.0f) { // Si el contador llego a los 10 frames, se termina la animacion
            justAte = false;
            ateTimer = 0.0f;
            pulseScale = 1.0f;
        }
    }
}

// Moverse hacia un objetivo
void GameEntity::moveTowards(const Point& target, float speedMultiplier) {

    // Se saca la distancia entre el objetivo y la entidad
    float dx = target.x - position.x;
    float dy = target.y - position.y;
    float distance = sqrtf(dx * dx + dy * dy);

    // Si la distancia es mayor a un umbral, indica que esta demasiado lejos, entonces comienza a acelerar en esa direccion
    if (distance > MOVE_TOWARDS_THRESHOLD) {

        // Se saca la "direccion" y la velocidad a la que debe ir en cada eje
        float desiredVelX = (dx / distance) * maxSpeed * speedMultiplier;
        float desiredVelY = (dy / distance) * maxSpeed * speedMultiplier;

        // Se acelera la velocidad en X y Y
        velX += (desiredVelX - velX) * MOVEMENT_ACCELERATION;
        velY += (desiredVelY - velY) * MOVEMENT_ACCELERATION;

        // Se hace una verificacion para ver que no sobrepase la velocidad maxima
        float currentSpeed = sqrtf(velX*velX + velY*velY);
        float maxAllowedSpeed = maxSpeed * speedMultiplier;

        if (currentSpeed > maxAllowedSpeed) {
            velX = (velX / currentSpeed) * maxAllowedSpeed;
            velY = (velY / currentSpeed) * maxAllowedSpeed;
        }
    }
    else if (distance < MOVE_TOWARDS_STOP_DISTANCE){ // Si el objetivo esta muy cerca, se marca como que ya ha llegado
        velX = 0;
        velY = 0;
        hasTarget = false;
    } else { // Si no esta ni cerca, ni lejos, entonces avanza a velociadad promedio
        velX = (dx / distance) * maxSpeed * speedMultiplier * MOVE_TOWARDS_SLOW_FACTOR;
        velY = (dy / distance) * maxSpeed * speedMultiplier * MOVE_TOWARDS_SLOW_FACTOR;
    }
}

// Se puede comer?
bool GameEntity::canEat(const GameEntity* other) const {

    float myArea = (radius * 2) * (radius * 2);
    float otherArea = (other->radius * 2) * (other->radius * 2);
    return (myArea > otherArea * EAT_SIZE_COMPARISON_THRESHOLD); // Si el area de una entidad es mayor a la otra, la primera se puede comer
}

// Accion de comer
void GameEntity::eat(GameEntity* other) {

    if (canEat(other)) { // Si se puede comer, se añade la masa del comido multiplicado por un factor, y se suma a la puntuacion

        mass += other->mass * EAT_MASS_GAIN_FACTOR;
        score += static_cast<int>(other->mass * EAT_SCORE_MULTIPLIER);
        other->active = false; // Se pone como inactiva la entidad comida

        justAte = true; // Comienza la animacion
        ateTimer = 0.0f;
        pulseScale = 1.2f;
    }
}

// Devuelve colores segun el tipo de entidad
pair<Color, Color> GameEntity::getColor() const {

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

    // Posicion aleatoria en el mapa
    position.x = WORLD_BOT_SPAWN_MARGIN + rand() % (int)(WORLD_WIDTH - WORLD_BOT_SPAWN_MARGIN * 2);
    position.y = WORLD_BOT_SPAWN_MARGIN + rand() % (int)(WORLD_HEIGHT - WORLD_BOT_SPAWN_MARGIN * 2);

    // Settear intervalo de decision para el espacio de frames entre cada decision que se toma
    decisionInterval = BOT_DECISION_INTERVAL;
    id = id_;
    state = BotState::WANDER; // Estado inicial a vagar
    targetEntity = nullptr;

    chooseNewTarget();
}

// ACtualizar decisiones del bot
void Bot::update(float deltatime, const vector<GameEntity*>& allEntities, GameEntity* player) {

    GameEntity::update(deltatime);

    decisionTimer += deltatime;

    // Si ya se completo el intervalo de decision, el bot se pone a analizar el entorno
    if (decisionTimer > decisionInterval) {
        analyzeEnvironment(allEntities, player);
        decisionTimer = 0.0f; // Se reinicia el contado de decisiones
    }

    executeBehavior(deltatime); // Se ejecuta su comportamiento respecto al entorno
}

// Analiza el ambiente
void Bot::analyzeEnvironment(const vector<GameEntity *> &allEntities, GameEntity *player) {

    if (!collisionSystem) { return; }

    // Se crean 3 vectores de prioridad para amenazas, presas y comida
    vector<PriorityTarget> threats;
    vector<PriorityTarget> prey;
    vector<PriorityTarget> food;

    // Area de evaluacion
    Rect viewArea(position.x - VIEW_RADIUS, position.y - VIEW_RADIUS, VIEW_RADIUS * 2, VIEW_RADIUS * 2);

    // Se obtienen todas las entidades en el area de evaluacion
    auto nearby = collisionSystem->queryArea(viewArea);

    for (auto entity : nearby) {

        if (!entity->active || entity == this) continue;

        // Se saca la distancia entre la entidad evaluada y la que esta analizando
        float dx = entity->position.x - position.x;
        float dy = entity->position.y - position.y;
        float distance = sqrt(dx * dx + dy * dy);

        if (distance > VIEW_RADIUS) continue;
        // Las prioridades se fijan en base a la masa de la entidad y la distancia
        // Si es un jugador, se añade al vector de amenazas o presas, segun la diferencia entre masa de la entidad evaluada y la otra
        if (entity->type == EntityType::PLAYER) {
            if (entity->mass > mass * BOT_SIZE_COMPARISON_THRESHOLD) {
                float priority = (entity->mass / mass) * (VIEW_RADIUS / distance);
                threats.emplace_back(entity, priority, distance);
            } else if (mass > entity->mass * BOT_SIZE_COMPARISON_THRESHOLD) {
                float priority = (mass / entity->mass) * (VIEW_RADIUS / distance);
                prey.emplace_back(entity, priority, distance);
            }
        }

        // Si es un BOT, se añade al vector de amenazas o presas, segun la diferencia entre masa de la entidad evaluada y la otra
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
        // Si es un pellet se añade directamente a la lista de comida
        else if (entity->type == EntityType::PELLET) {
            float priority = (VIEW_RADIUS / distance) * BOT_FOOD_PRIORITY_MULTIPLIER;
            food.emplace_back(entity, priority, distance);
        }
    }

    // Ordenamos de forma descendente
    sort(threats.begin(), threats.end(), [](const PriorityTarget& a, const PriorityTarget& b) {
         return a.priority > b.priority;
     });
    sort(prey.begin(), prey.end(), [](const PriorityTarget& a, const PriorityTarget& b) {
         return a.priority > b.priority;
     });
    sort(food.begin(), food.end(), [](const PriorityTarget& a, const PriorityTarget& b) {
         return a.priority > b.priority;
     });

    // Se prioriza en orden las amenazas, las presas y la comida al final
    if (!threats.empty() && threats[0].priority > BOT_THREAT_PRIORITY_THRESHOLD) {
        // si hay amenazaas, y su prioridad es mayor a un umbral, se pone a escapar
        state = BotState::FLEE;
        targetEntity = threats[0].entity;
    }
    else if (!prey.empty() && prey[0].priority > BOT_PREY_PRIORITY_THRESHOLD) {
        // si hay presas, y su prioridad es mayor a un umbral, se pone a cazar
        state = BotState::CHASE;
        targetEntity = prey[0].entity;
    }
    else if (!food.empty()) {
        // Si hay comida, se pone a buscar comida
        state = BotState::SEEK_FOOD;
        targetEntity = food[0].entity;
    }
    else {
        // Si no hay ningun objetivo, se pone a vagar
        state = BotState::WANDER;
        targetEntity = nullptr;
    }
}

// Ejecutar comportamiento
void Bot::executeBehavior(float deltatime) {

    switch (state) {

        // Si esta escapando
        case BotState::FLEE:
            if (targetEntity && targetEntity->active) {

                // Distancia entre entidad y objetivo
                float dx = position.x - targetEntity->position.x;
                float dy = position.y - targetEntity->position.y;
                float distance = sqrt(dx*dx + dy*dy);

                // Si la distancia esta dentro del radio de escape, y es mayor a 0
                if (distance < FLEE_RADIUS) {
                    if (distance > 0) {
                        // Se saca la direccion contraria a donde esta el target anterior y se actualiza el nuevo objetivo
                        targetPosition.x = position.x + (dx / distance) * SAFE_DISTANCE;
                        targetPosition.y = position.y + (dy / distance) * SAFE_DISTANCE;

                        // Se mantiene la posicion del nuevo target dentro de los limites del mundo
                        if (targetPosition.x < WORLD_SPAWN_MARGIN) targetPosition.x = WORLD_SPAWN_MARGIN;
                        if (targetPosition.x > WORLD_WIDTH - WORLD_SPAWN_MARGIN) {
                            targetPosition.x = WORLD_WIDTH - WORLD_SPAWN_MARGIN;
                        }
                        if (targetPosition.y < WORLD_SPAWN_MARGIN) targetPosition.y = WORLD_SPAWN_MARGIN;
                        if (targetPosition.y > WORLD_HEIGHT - WORLD_SPAWN_MARGIN) {
                            targetPosition.y = WORLD_HEIGHT - WORLD_SPAWN_MARGIN;
                        }

                        // Se procede a mover en direccion al nuevo objetivo para escapar
                        moveTowards(targetPosition, 0.85f);
                    }
                } else {
                    state = BotState::WANDER;
                }
            } else {
                state = BotState::WANDER;
            }
            break;

        case BotState::CHASE:
            // Se pone la entidad a perseguir como objetivo
            if (targetEntity && targetEntity->active) {
                targetPosition = targetEntity->position;
                moveTowards(targetPosition, 0.85f);
            } else {
                state = BotState::WANDER;
            }
            break;

        // Igual que el caso anterior, se pone la comida como objetivo
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

// Escoger un nuevo objetivo
void Bot::chooseNewTarget() {

    int attempts = 0;
    float minDistance = 100.0f;

    // En 10 intentos, se escoje una distancia en una parte aleatoria del mapa
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

pair<Color, Color> Bot::getColor() const {
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

pair<Color, Color> Pellet::getColor() const {

    if (mass < 0.25f) return {Color(255, 255, 100,   255),
                                    Color(210, 210, 100,   255)};
    if (mass < 0.35f) return {Color(255, 200,   50, 255),
                                        Color(200, 150,   50, 255)};
    return {Color(255, 150, 0, 255),
                Color(200, 150, 0, 255)};
}
