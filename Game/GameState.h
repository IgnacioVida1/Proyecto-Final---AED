//
// Created by ignac on 23/02/2026.
//

#ifndef GAMESTATE_H
#define GAMESTATE_H

enum class GameState {
    MENU_MAIN,
    MENU_OPTIONS,
    MENU_PAUSE,
    PLAYING,
    GAME_OVER,
    VICTORY
};

struct MenuOption {
    std::string text;
    GameState targetState;
    void (*action)() = nullptr;
};

#endif //GAMESTATE_H
