//
// Created by ignac on 16/02/2026.
//

#include "Game.h"
#include <cmath>

void Game::cleanupEntities() {

    for (auto entity : allEntities) {
        delete entity;
    }

    allEntities.clear();
    bots.clear();
    pellets.clear();
}

Game::Game() : isRunning(false), frameCount(0), gameTime(0.0f), currentState(GameState::MENU_MAIN) {

    settings.totalBots = TOTAL_BOTS;
    settings.totalPellets = TOTAL_PELLETS;
    settings.playerSpeed = PLAYER_MAX_SPEED;
    settings.botSpeed = BOT_MAX_SPEED;

    config.WorldWidth = WORLD_WIDTH;
    config.WorldHeight = WORLD_HEIGHT;
    config.quadCapacity = QUADTREE_DEFAULT_CAPACITY;
    config.maxQuadLevels = QUADTREE_MAX_LEVELS;
    config.showQuad = false;

    collisionSystem = new CollisionSystem(config);

    camera = new Camera(VIEWPORT_WIDTH, VIEWPORT_HEIGHT, WORLD_WIDTH, WORLD_HEIGHT);

    player = new Player();
    allEntities.push_back(player);

    for (int i = 0; i < TOTAL_BOTS; i++) {
        Bot* bot = new Bot(i + 1);
        bots.push_back(bot);
        allEntities.push_back(bot);
    }

    for (int i = 0; i < TOTAL_PELLETS; i++) {
        Pellet* pellet = new Pellet();
        pellets.push_back(pellet);
        allEntities.push_back(pellet);
    }

    collisionSystem->setActiveEntities(allEntities);
}

Game::~Game() {

    delete collisionSystem;
    cleanupEntities();
}

void Game::init() {

    isRunning = true;
    frameCount = 0;
    gameTime = 0.0f;

    std::cout << "Juego inicializado" << std::endl;
    std::cout << "Mundo: " << config.WorldWidth << "x" << config.WorldHeight << std::endl;
    std::cout << "QuadTree: capacidad=" << config.quadCapacity
              << ", niveles=" << config.maxQuadLevels << std::endl;

    for (auto bot : bots) {
        bot->setCollisionSystem(collisionSystem);
        bot->active = false;
    }

    player->active = false;
}

void Game::startGame() {

    currentState = GameState::PLAYING;
    gameOverTimer = 0.0f;

    player->active = true;
    player->position = Point(WORLD_WIDTH / 2.0f, WORLD_HEIGHT / 2.0f);
    player->mass = PLAYER_INITIAL_MASS;
    player->speed = settings.playerSpeed;
    player->velX = 0;
    player->velY = 0;

    for (auto bot : bots) {
        bot->active = true;
        bot->position.x = WORLD_BOT_SPAWN_MARGIN + rand() % (int)(WORLD_WIDTH - WORLD_BOT_SPAWN_MARGIN * 2);
        bot->position.y = WORLD_BOT_SPAWN_MARGIN + rand() % (int)(WORLD_HEIGHT - WORLD_BOT_SPAWN_MARGIN * 2);
        bot->mass = BOT_INITIAL_MASS_MIN + (rand() % 100) / 100.0f;
        bot->speed = settings.botSpeed;
        bot->velX = 0;
        bot->velY = 0;
        bot->setState(BotState::WANDER);
        bot->chooseNewTarget();
    }

    for (auto pellet : pellets) {
        pellet->active = true;
        pellet->position.x = WORLD_SPAWN_MARGIN + rand() % (int)(WORLD_WIDTH - WORLD_SPAWN_MARGIN * 2);
        pellet->position.y = WORLD_SPAWN_MARGIN + rand() % (int)(WORLD_HEIGHT - WORLD_SPAWN_MARGIN * 2);
    }
}

void Game::respawnPlayer() {

    int attempts = 0;
    bool safePosition = false;
    Point spawnPos(0, 0);

    while (!safePosition && attempts < MAX_SPAWN_ATTEMPTS) {
        spawnPos.x = WORLD_SPAWN_MARGIN + rand() % (int)(WORLD_WIDTH - WORLD_SPAWN_MARGIN * 2);
        spawnPos.y = WORLD_SPAWN_MARGIN + rand() % (int)(WORLD_HEIGHT - WORLD_SPAWN_MARGIN * 2);

        safePosition = true;

        for (auto bot : bots) {
            if (!bot->active) continue;

            float dx = bot->position.x - spawnPos.x;
            float dy = bot->position.y - spawnPos.y;
            float dist = sqrt(dx*dx + dy*dy);
            float minDist = bot->radius + PLAYER_BASE_RADIUS;

            // No spawnear dentro de bots
            if (dist < minDist) {
                safePosition = false;
                break;
            }

            if (dist < SAFE_SPAWN_DISTANCE && bot->mass > player->mass * EAT_SIZE_COMPARISON_THRESHOLD) {
                safePosition = false;
                break;
            }
        }
        attempts++;
    }

    player->active = true;
    player->position = spawnPos;
    player->mass = PLAYER_INITIAL_MASS;
    player->velX = 0;
    player->velY = 0;
    player->score = 0;

    currentState = GameState::PLAYING;
    gameOverTimer = 0.0f;
}

void Game::returnToMenu() {

    currentState = GameState::MENU_MAIN;

    player->active = false;
    for (auto bot : bots) { bot->active = false; }
    for (auto pellet : pellets) { pellet->active = false; }

    gameOverTimer = 0.0f;
}

GameEntity* Game::checkVictoryCondition() {

    for (auto e : allEntities) {
        if (!e->active) continue;

        float eDiameter = e->radius * 2;

        if (eDiameter >= WORLD_WIDTH * VICTORY_SIZE_RATIO) {
            return e;
        }
    }

    return nullptr;
}

void Game::update(float deltatime) {

    if (!isRunning) { return; }

    gameTime += deltatime;

    switch (currentState) {

        case GameState::MENU_MAIN:
            break;
        case GameState::PLAYING: {
            updateEntities(deltatime);

            collisionSystem->update();

            GameEntity* winner = checkVictoryCondition();
            if (winner) {
                if (winner->getType() == EntityType::PLAYER) {
                    currentState = GameState::VICTORY;
                    std::cout << "¡VICTORIA! El jugador ganó" << std::endl;
                } else if (winner->getType() == EntityType::BOT) {
                    currentState = GameState::LOSING;
                    std::cout << "Derrota: Un bot ganó" << std::endl;
                }
                break;
            }

            applyGameRules();

            if (player->active) {
                camera->update(player->position, player->radius);
            }

            updateRespawnTimers(deltatime);
            checkAndRespawn();
            break;
        }
        case GameState::GAME_OVER: {
            gameOverTimer += deltatime;
            break;
        }
    }
//    if (frameCount % 300 == 0) { printGameStats(); }
}

void Game::updateEntities(float deltatime) {

    if (player->active) {
        player->update(deltatime);
        clampToWorld(player);
    }

    for (auto bot : bots) {
        if (bot->active) {
            bot->update(deltatime, allEntities, player);
            clampToWorld(bot);
        }
    }

    for (auto pellet : pellets) {
        if (pellet->active) {
            pellet->update(deltatime);
            clampToWorld(pellet);
        }
    }
}

void Game::clampToWorld(GameEntity* entity) {

    float margin = entity->radius;

    if (entity->position.x < margin) {
        entity->position.x = margin;
        entity->velX = 0;
    }
    if (entity->position.x > config.WorldWidth - margin) {
        entity->position.x = config.WorldWidth - margin;
        entity->velX = 0;
    }
    if (entity->position.y < margin) {
        entity->position.y = margin;
        entity->velY = 0;
    }
    if (entity->position.y > config.WorldHeight - margin) {
        entity->position.y = config.WorldHeight - margin;
        entity->velY = 0;
    }
}

void Game::updateRespawnTimers(float deltatime) {
    for (auto it = pelletsTimer.begin(); it != pelletsTimer.end();) {

        it->timeRemaining -= deltatime;

        if (it->timeRemaining <= 0) {

            Pellet* pellet = dynamic_cast<Pellet*>(it->entity);

            Point pos(0, 0);
            bool safeFound = false;

            for (int attempts = 0; attempts < 10 && !safeFound; attempts++) {
                pos.x = WORLD_PELLET_SPAWN_MARGIN + rand() % (int)(WORLD_WIDTH - WORLD_PELLET_SPAWN_MARGIN * 2);
                pos.y = WORLD_PELLET_SPAWN_MARGIN + rand() % (int)(WORLD_HEIGHT - WORLD_PELLET_SPAWN_MARGIN * 2);

                safeFound = true;
                auto nearby = collisionSystem->queryNearby(pos, 50.0f);

                for (auto entity : nearby) {
                    if (entity->getBoundingBox().contains(pos)) {
                        safeFound = false;
                        break;
                    }
                }
            }

            pellet->position = pos;
            pellet->active = true;
            pellet->mass = PELLET_INITIAL_MASS_MIN + (rand() % 100) / 500.0f;
            pellet->velX = 0;
            pellet->velY = 0;

            it = pelletsTimer.erase(it);
        }
        else {
            ++it;
        }
    }

    for (auto it = botsTimer.begin(); it != botsTimer.end();) {
        it->timeRemaining -= deltatime;
        if (it->timeRemaining <= 0) {
            Bot* bot = dynamic_cast<Bot*>(it->entity);

            Point pos(0, 0);
            bool safeFound = false;

            for (int attempts = 0; attempts < 10 && !safeFound; attempts++) {
                pos.x = WORLD_BOT_SPAWN_MARGIN + rand() % (int)(WORLD_WIDTH - WORLD_BOT_SPAWN_MARGIN * 2);
                pos.y = WORLD_BOT_SPAWN_MARGIN + rand() % (int)(WORLD_HEIGHT - WORLD_BOT_SPAWN_MARGIN * 2);

                safeFound = true;
                auto nearby = collisionSystem->queryNearby(pos, 50.0f);

                for (auto entity : nearby) {
                    if (entity->getBoundingBox().contains(pos)) {
                        safeFound = false;
                        break;
                    }

                    float dx = entity->position.x - pos.x;
                    float dy = entity->position.y - pos.y;
                    float dist = sqrt(dx*dx + dy*dy);
                    if (dist < entity->radius + bot->radius) {
                        safeFound = false;
                        break;
                    }
                }
            }

            bot->position = pos;
            bot->active = true;
            bot->mass = BOT_INITIAL_MASS_MIN + (rand() % 100) / 100.0f;
            bot->velX = 0;
            bot->velY = 0;
            bot->setState(BotState::WANDER);
            bot->chooseNewTarget();

            it = botsTimer.erase(it);
        } else {
            ++it;
        }
    }
}

void Game::checkAndRespawn() {

    for (auto pellet : pellets) {
        if (!pellet->active) {
            bool inQueue = false;
            for (const auto& timer : pelletsTimer) {
                if (timer.entity == pellet) {
                    inQueue = true;
                    break;
                }
            }
            if (!inQueue) {
                pelletsTimer.emplace_back(pellet, PELLET_RESPAWN_TIME);
                pelletsEaten++;
            }
        }
    }

    for (auto bot : bots) {
        if (!bot->active) {
            bool inQueue = false;
            for (const auto& timer : botsTimer) {
                if (timer.entity == bot) {
                    inQueue = true;
                    break;
                }
            }
            if (!inQueue) {
                botsTimer.emplace_back(bot, BOT_RESPAWN_TIME);
                botsEaten++;
            }
        }
    }
}

void Game::applyGameRules() {
    if (!player->active && currentState == GameState::PLAYING) {
        playerDeaths++;
        currentState = GameState::GAME_OVER;
        gameOverTimer = 0.0f;
    }
}



void Game::handleInput(float dirX, float dirY, bool spacePressed, bool enterPressed) {

    switch (currentState) {
        case GameState::MENU_MAIN:
            if (spacePressed) {
                startGame();
            }
            break;

        case GameState::PLAYING:
            if (player && player->active) {
                float targetVelX = dirX * player->maxSpeed;
                float targetVelY = dirY * player->maxSpeed;
                player->velX += (targetVelX - player->velX) * 0.3f;
                player->velY += (targetVelY - player->velY) * 0.3f;
            }
            break;

        case GameState::GAME_OVER:
            if (spacePressed && gameOverTimer >= GAME_OVER_DELAY) {
                respawnPlayer();
            } else if (enterPressed) {
                returnToMenu();
            }
            break;
    }
}

void Game::applySettings() {
    config.quadCapacity = QUADTREE_DEFAULT_CAPACITY;
    config.maxQuadLevels = QUADTREE_MAX_LEVELS;
    config.showQuad = settings.showQuad;

    player->maxSpeed = settings.playerSpeed;
    for (auto bot : bots) {
        bot->maxSpeed = settings.botSpeed;
    }

    std::cout << "Configuración aplicada" << std::endl;
}

void Game::printGameStats() {

    std::cout << "\n=== ESTADÍSTICAS DEL JUEGO ===" << std::endl;
    std::cout << "Tiempo: " << (int)gameTime << "s" << std::endl;
    std::cout << "Frames: " << frameCount << std::endl;
    std::cout << "\nENTIDADES:" << std::endl;
    std::cout << "  Jugador: " << (player->active ? "Vivo" : "Muerto") << std::endl;
    std::cout << "  Puntaje: " << player->score << std::endl;

    int activeBots = 0;
    for (auto bot : bots) if (bot->active) activeBots++;
    int activePellets = 0;
    for (auto pellet : pellets) if (pellet->active) activePellets++;

    std::cout << "  Bots: " << activeBots << "/" << bots.size() << std::endl;
    std::cout << "  Pellets: " << activePellets << "/" << pellets.size() << std::endl;

    std::cout << "\nESTADÍSTICAS:" << std::endl;
    std::cout << "  Pellets comidos: " << pelletsEaten << std::endl;
    std::cout << "  Bots comidos: " << botsEaten << std::endl;
    std::cout << "  Muertes: " << playerDeaths << std::endl;
    std::cout << "  Pellets spawn: " << pelletsSpawned << std::endl;
    std::cout << "===============================" << std::endl;

    collisionSystem->printPerformanceStats();
}