//
// Created by ignac on 23/02/2026.
//

#ifndef CONSTANTS_H
#define CONSTANTS_H

// Tamaño de mundo
constexpr float WORLD_WIDTH = 2000.0f;
constexpr float WORLD_HEIGHT = 2000.0f;

// Límites de spawn y movimiento
constexpr float WORLD_SPAWN_MARGIN = 50.0f;
constexpr float WORLD_BOT_SPAWN_MARGIN = 200.0f;
constexpr float WORLD_PELLET_SPAWN_MARGIN = 50.0f;

// Tamaño de la ventana
constexpr int VIEWPORT_WIDTH = 1024;
constexpr int VIEWPORT_HEIGHT = 768;

// Parámetros de zoom
constexpr float CAMERA_MIN_ZOOM = 0.5f;
constexpr float CAMERA_MAX_ZOOM = 100.0f;
constexpr float CAMERA_ZOOM_SPEED = 0.05f;
constexpr float CAMERA_FOLLOW_SPEED = 0.07f;
constexpr float CAMERA_IDEAL_PLAYER_SIZE_RATIO = 0.15f;

// Jugador
constexpr float PLAYER_INITIAL_MASS = 2.0f;
constexpr float PLAYER_BASE_RADIUS = 15.0f;
constexpr float PLAYER_MAX_SPEED = 2.0f;
constexpr int PLAYER_ID = 1;
constexpr float SAFE_SPAWN_DISTANCE = 500.0f;

// Bots
constexpr int TOTAL_BOTS = 15;
constexpr float BOT_INITIAL_MASS_MIN = 2.0f;
constexpr float BOT_INITIAL_MASS_MAX = 2.0f;
constexpr float BOT_BASE_RADIUS_MIN = 15.0f;
constexpr float BOT_BASE_RADIUS_MAX = 15.0f;
constexpr float BOT_MAX_SPEED = 2.0f;
constexpr float BOT_DECISION_INTERVAL = 0.5f;
constexpr float BOT_RESPAWN_TIME = 3.0f;

// Radios de comportamiento de bots
constexpr float BOT_VIEW_RADIUS = 300.0f;
constexpr float BOT_FLEE_RADIUS = 200.0f;
constexpr float BOT_CHASE_RADIUS = 250.0f;
constexpr float BOT_SAFE_DISTANCE = 300.0f;
constexpr float BOT_SPAWN_RADIUS = 150.0f;

// Umbrales de prioridad para decisiones
constexpr float BOT_THREAT_PRIORITY_THRESHOLD = 1.5f;
constexpr float BOT_PREY_PRIORITY_THRESHOLD = 1.2f;
constexpr float BOT_FOOD_PRIORITY_MULTIPLIER = 0.5f;
constexpr float BOT_SIZE_COMPARISON_THRESHOLD = 1.2f;

// Pellets
constexpr int TOTAL_PELLETS = 1000;
constexpr float PELLET_INITIAL_MASS_MIN = 0.2f;
constexpr float PELLET_INITIAL_MASS_MAX = 0.4f;
constexpr float PELLET_BASE_RADIUS_MIN = 5.0f;
constexpr float PELLET_BASE_RADIUS_MAX = 7.0f;
constexpr float PELLET_RESPAWN_TIME = 0.5f;

// Factores de movimiento
constexpr float MOVEMENT_FRICTION = 0.9f;
constexpr float MOVEMENT_ACCELERATION = 0.4f;
constexpr float MOVE_TOWARDS_THRESHOLD = 20.0f;
constexpr float MOVE_TOWARDS_STOP_DISTANCE = 0.5f;
constexpr float MOVE_TOWARDS_SLOW_FACTOR = 0.5f;

// Límites de velocidad
constexpr float MAX_SPEED_MULTIPLIER_COLLISION = 2.0f;
constexpr float COLLISION_REBOUND_FORCE = 5.0f;
constexpr float PELLET_MASS_FACTOR = 0.5f;

// Crecimiento al comer
constexpr float EAT_MASS_GAIN_FACTOR = 0.65f;
constexpr float EAT_SCORE_MULTIPLIER = 5.0f;
constexpr float EAT_SIZE_COMPARISON_THRESHOLD = 1.1f;

// Quad
constexpr int QUADTREE_DEFAULT_CAPACITY = 4;
constexpr int QUADTREE_MAX_LEVELS = 8;
constexpr float QUADTREE_COLLISION_PADDING = 5.0f;

// Grid
constexpr float GRID_BASE_SPACING = 50.0f;
constexpr float GRID_ZOOM_LOW_THRESHOLD = 0.7f;
constexpr float GRID_ZOOM_HIGH_THRESHOLD = 1.5f;
constexpr float GRID_SPACING_FAR = 100.0f;
constexpr float GRID_SPACING_CLOSE = 25.0f;

// Texto
constexpr float ENTITY_TEXT_MIN_RADIUS = 15.0f;
constexpr float ENTITY_TEXT_MIN_ZOOM = 0.7f;

// Menu
constexpr float GAME_OVER_DELAY = 3.0f;

// Victoria
constexpr float VICTORY_SIZE_RATIO = 0.90f;
constexpr float VICTORY_RADIUS = (WORLD_WIDTH * VICTORY_SIZE_RATIO) / 2.0f;

#endif //CONSTANTS_H

