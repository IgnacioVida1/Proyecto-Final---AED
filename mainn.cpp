#ifdef _WIN32
#undef main
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "Game/Game.h"
#include "Renderer/Renderer.h"
#include "Game/Constants.h"

int main(int argc, char* argv[]) {
    srand(time(nullptr));

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Error SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    if (TTF_Init() < 0) {
        std::cerr << "Error SDL_ttf: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    Game game;
    game.init();

    Renderer renderer(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    if (!renderer.init()) {
        std::cerr << "Error al inicializar renderer" << std::endl;
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    renderer.setCamera(game.getCamera());

    bool running = true;
    SDL_Event event;
    Uint32 lastTime = SDL_GetTicks();
    int frameCount = 0;
    float fps = 0;

    int selectedMainMenu = 0;
    int selectedOptions = 0;
    int selectedPause = 0;
    int selectedGameOver = 0;
    int selectedVictory = 0;
    int selectedLosing = 0;

    while (running) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        if (deltaTime > 0.1f) deltaTime = 0.016f;
        lastTime = currentTime;

        frameCount++;
        if (currentTime - lastTime >= 1000) {
            fps = frameCount;
            frameCount = 0;
        }

        float dirX = 0, dirY = 0;
        bool spacePressed = false;
        bool enterPressed = false;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    if (game.getState() == GameState::PLAYING) {
                        game.setState(GameState::MENU_PAUSE);
                    } else if (game.getState() == GameState::MENU_PAUSE) {
                        game.setState(GameState::PLAYING);
                    } else if (game.getState() != GameState::PLAYING &&
                               game.getState() != GameState::MENU_MAIN) {
                        game.setState(GameState::MENU_MAIN);
                    }
                }

                switch (game.getState()) {
                    case GameState::MENU_MAIN:
                        if (event.key.keysym.sym == SDLK_UP) {
                            selectedMainMenu = (selectedMainMenu - 1 + 3) % 3;
                        }
                        else if (event.key.keysym.sym == SDLK_DOWN) {
                            selectedMainMenu = (selectedMainMenu + 1) % 3;
                        }
                        else if (event.key.keysym.sym == SDLK_RETURN ||
                                 event.key.keysym.sym == SDLK_SPACE) {
                            switch (selectedMainMenu) {
                                case 0:
                                    game.setState(GameState::PLAYING);
                                    game.startGame();
                                    break;
                                case 1:
                                    game.setState(GameState::MENU_OPTIONS);
                                    break;
                                case 2:
                                    running = false;
                                    break;
                            }
                        }
                        break;

                    case GameState::MENU_OPTIONS:
                        if (event.key.keysym.sym == SDLK_UP) {
                            selectedOptions = (selectedOptions - 1 + 2) % 2;
                        }
                        else if (event.key.keysym.sym == SDLK_DOWN) {
                            selectedOptions = (selectedOptions + 1) % 2;
                        }
                        else if (event.key.keysym.sym == SDLK_LEFT) {
                            auto& settings = game.getSettings();
                            switch (selectedOptions) {
                                case 0:
                                    if (settings.playerSpeed > 1.0f) {
                                        settings.playerSpeed -= 0.1f;
                                        if (settings.playerSpeed < 1.0f)
                                            settings.playerSpeed = 1.0f;
                                    }
                                break;
                                case 1:
                                    if (settings.botSpeed > 1.0f) {
                                        settings.botSpeed -= 0.1f;
                                        if (settings.botSpeed < 1.0f)
                                            settings.botSpeed = 1.0f;
                                    }
                                    break;
                            }
                        }
                        else if (event.key.keysym.sym == SDLK_RIGHT) {
                            auto& settings = game.getSettings();
                            switch (selectedOptions) {
                                case 0:
                                    if (settings.playerSpeed < 5.0f) {
                                        settings.playerSpeed += 0.1f;
                                        if (settings.playerSpeed > 5.0f)
                                            settings.playerSpeed = 5.0f;
                                    }
                                break;
                                case 1:
                                    if (settings.botSpeed < 5.0f) {
                                        settings.botSpeed += 0.1f;
                                        if (settings.botSpeed > 5.0f)
                                            settings.botSpeed = 5.0f;
                                    }
                                break;
                            }
                        }
                        else if (event.key.keysym.sym == SDLK_RETURN ||
                                 event.key.keysym.sym == SDLK_SPACE) {
                            game.applySettings();
                            game.setState(GameState::MENU_MAIN);
                        }
                        break;

                    case GameState::PLAYING:
                        if (event.key.keysym.sym == SDLK_q) {
                            auto& config = game.getConfig();
                            config.showQuad = !config.showQuad;
                        }
                        break;

                    case GameState::MENU_PAUSE:
                        if (event.key.keysym.sym == SDLK_UP) {
                            selectedPause = (selectedPause - 1 + 4) % 4;
                        }
                        else if (event.key.keysym.sym == SDLK_DOWN) {
                            selectedPause = (selectedPause + 1) % 4;
                        }
                        else if (event.key.keysym.sym == SDLK_RETURN ||
                                 event.key.keysym.sym == SDLK_SPACE) {
                            switch (selectedPause) {
                                case 0: // CONTINUAR
                                    game.setState(GameState::PLAYING);
                                    break;
                                case 1: // OPCIONES
                                    game.setState(GameState::MENU_OPTIONS);
                                    break;
                                case 2: // REINICIAR
                                    game.startGame();
                                    game.setState(GameState::PLAYING);
                                    break;
                                case 3: // MENÚ PRINCIPAL
                                    game.returnToMenu();
                                    break;
                            }
                        }
                        break;

                    case GameState::GAME_OVER:
                        if (event.key.keysym.sym == SDLK_UP) {
                            selectedGameOver = (selectedGameOver - 1 + 2) % 2;
                        }
                        else if (event.key.keysym.sym == SDLK_DOWN) {
                            selectedGameOver = (selectedGameOver + 1) % 2;
                        }
                        else if ((event.key.keysym.sym == SDLK_RETURN ||
                                  event.key.keysym.sym == SDLK_SPACE) &&
                                  game.getGameOverTimer() >= 1.0f) {
                            if (selectedGameOver == 0) {
                                game.respawnPlayer();
                                game.setState(GameState::PLAYING);
                            } else {
                                game.returnToMenu();
                            }
                        }
                        break;
                    case GameState::VICTORY:
                        if (event.key.keysym.sym == SDLK_UP) {
                            selectedVictory = (selectedVictory - 1 + 2) % 2;
                        }
                        else if (event.key.keysym.sym == SDLK_DOWN) {
                            selectedVictory = (selectedVictory + 1) % 2;
                        }
                        else if (event.key.keysym.sym == SDLK_RETURN ||
                                 event.key.keysym.sym == SDLK_SPACE) {
                            if (selectedVictory == 0) {
                                game.startGame();
                                game.setState(GameState::PLAYING);
                            } else {
                                game.returnToMenu();
                            }
                        }
                    break;
                    case GameState::LOSING:
                        if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE) {
                            game.returnToMenu();
                        }
                    break;
                }
            }

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_SPACE) {
                    spacePressed = true;
                }
                if (event.key.keysym.sym == SDLK_RETURN ||
                    event.key.keysym.sym == SDLK_KP_ENTER) {
                    enterPressed = true;
                }
            }
        }

        if (game.getState() == GameState::PLAYING) {
            const Uint8* keystate = SDL_GetKeyboardState(nullptr);
            if (keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_UP]) dirY = -1;
            if (keystate[SDL_SCANCODE_S] || keystate[SDL_SCANCODE_DOWN]) dirY = 1;
            if (keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_LEFT]) dirX = -1;
            if (keystate[SDL_SCANCODE_D] || keystate[SDL_SCANCODE_RIGHT]) dirX = 1;

            if (dirX != 0 && dirY != 0) {
                float len = sqrt(dirX*dirX + dirY*dirY);
                dirX /= len;
                dirY /= len;
            }
        }

        game.handleInput(dirX, dirY, spacePressed, enterPressed);

        game.update(deltaTime);

        switch (game.getState()) {
            case GameState::MENU_MAIN:
                renderer.renderMainMenu(&game, selectedMainMenu);
                break;

            case GameState::MENU_OPTIONS:
                renderer.renderOptionsMenu(&game, selectedOptions);
                break;

            case GameState::PLAYING:
                renderer.renderGame(&game, fps);
                break;

            case GameState::MENU_PAUSE:
                renderer.renderPauseMenu(&game, selectedPause);
                break;

            case GameState::GAME_OVER:
                renderer.renderGameOver(&game, selectedGameOver);
                break;
            case GameState::VICTORY:
                renderer.renderVictoryScreen(&game, selectedVictory);
                break;
            case GameState::LOSING:
                renderer.renderLosing(&game, selectedLosing);
                break;
        }

        SDL_Delay(16);
    }

    renderer.cleanup();
    TTF_Quit();
    SDL_Quit();
    
    return 0;
}