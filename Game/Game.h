//
// Created by ignac on 16/02/2026.
//

#ifndef GAME_H
#define GAME_H

#include "CollisionSystem.h"
#include "../Entity/entities.h"
#include "../Renderer/Camera.h"
#include "GameState.h"
#include <vector>

struct GameSettings {
    int totalBots = 31;
    int totalPellets = 7500;
    float playerSpeed = 2.0f;
    float botSpeed = 2.0f;
    bool showData = false;
    bool showQuad = false;
};

struct RespawnTimer {

    GameEntity* entity;
    float timeRemaining;

    RespawnTimer(GameEntity* entity, float t) : entity(entity), timeRemaining(t) {}
};

class Game {
    GameConfig config;
    CollisionSystem* collisionSystem;
    Camera* camera;
    GameState currentState;

    Player* player;
    vector<Bot*> bots;
    vector<Pellet*> pellets;
    vector<GameEntity*> allEntities;

    vector<RespawnTimer> pelletsTimer;
    vector<RespawnTimer> botsTimer;

    bool isRunning;
    int frameCount;
    float gameTime;

    int pelletsSpawned = 0;
    int pelletsEaten = 0;
    int botsEaten = 0;
    int playerDeaths = 0;

    float gameOverTimer = 0.0f;
    float gameOverDelay = GAME_OVER_DELAY;

    GameSettings settings;
    int selectedMenuOption = 0;
    int selectedOptionsOption = 0;

    void cleanupEntities();

public:
    Game();

    ~Game();

    void init();

    void update(float deltatime);

    void updateEntities(float deltatime);

    void clampToWorld(GameEntity* entity);

    void applyGameRules();

    void updateRespawnTimers(float deltatime);

    void checkAndRespawn();

    void handleInput(float dirX, float dirY, bool spacePressed, bool enterPressed);

    bool isGameRunning() const { return isRunning; }

    GameState getState() const { return currentState; }

    Player* getPlayer() const { return player; }

    Camera* getCamera() const { return camera; }

    const vector<GameEntity*>& getEntities() const { return allEntities; }

    CollisionSystem* getCollisionSystem() { return collisionSystem; }

    const GameConfig& getConfig() const { return config; }

    float getWorldWidth() const { return WORLD_WIDTH; }

    float getWorldHeight() const { return WORLD_HEIGHT; }

    float getGameOverTimer() const { return gameOverTimer; }

    void startGame();

    void respawnPlayer();

    void returnToMenu();

    void setState(GameState newState) { currentState = newState; }

    GameSettings& getSettings() { return settings; }
    int getSelectedMenuOption() const { return selectedMenuOption; }
    void setSelectedMenuOption(int opt) { selectedMenuOption = opt; }
    int getSelectedOptionsOption() const { return selectedOptionsOption; }
    void setSelectedOptionsOption(int opt) { selectedOptionsOption = opt; }

    void applySettings();

    GameEntity* checkVictoryCondition();

    void printGameStats();
};



#endif //GAME_H
