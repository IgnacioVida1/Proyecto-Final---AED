//
// Created by ignac on 9/02/2026.
//

#include "Renderer.h"
#include "../Game/Game.h"
#include "../Quad/Quad.h"
#include "../Entity/Entities.h"
#include <iostream>
#include <cmath>
#include <functional>

bool Renderer::init() {

    window = SDL_CreateWindow(
        "Agar.io C++ - VERSION CUADRADOS",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_SHOWN
    );
    if (!window) { return false; }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window);
        return false;
    }

    font = TTF_OpenFont("C:/Windows/Fonts/Arial.ttf", 20);
    if (!font) { font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 16); }
    if (!font) { font = TTF_OpenFont("/System/Library/Fonts/Helvetica.ttf", 20); }

    if (!font) {
        std::cout << "ADVERTENCIA: No se pudo cargar fuente grande" << std::endl;
    } else {
        std::cout << "Fuente grande cargada correctamente" << std::endl;
    }

    fontSmall = TTF_OpenFont("C:/Windows/Fonts/Arial.ttf", 14);
    if (!fontSmall) fontSmall = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 14);

    if (!fontSmall) {
        std::cout << "ADVERTENCIA: No se pudo cargar fuente pequeña" << std::endl;
    }

    return true;
}

void Renderer::cleanup() {

    if (font) { TTF_CloseFont(font); }
    if (fontSmall) { TTF_CloseFont(fontSmall); }
    if (renderer) { SDL_DestroyRenderer(renderer); }
    if (window) { SDL_DestroyWindow(window); }
}

void Renderer::clear() {

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
}

void Renderer::present() {

    SDL_RenderPresent(renderer);
}

void Renderer::drawCircle(int cx, int cy, int r, Color c) {
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    for (int y = -r; y <= r; ++y) {
        for (int x = -r; x <= r; ++x) {
            if (x*x + y*y <= r*r) {
                SDL_RenderDrawPoint(renderer, x+cx, y+cy);
            }
        }
    }
}

void Renderer::fillCircle(int cx, int cy, int r, Color c) {
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    for (int y = -r; y <= r; ++y) {
        for (int x = -r; x <= r; ++x) {
            if (x*x + y*y <= r*r) {
                SDL_RenderDrawPoint(renderer, x+cx, y+cy);
            }
        }
    }
}

void Renderer::drawRect(int x, int y, int w, int h, Color c) {

    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderDrawRect(renderer, &rect);
}

void Renderer::fillRect(int x, int y, int w, int h, Color c) {
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer, &rect);
}

void Renderer::drawLine(int x1, int y1, int x2, int y2, Color c) {

    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void Renderer::drawText(const std::string& text, int x, int y, Color c, bool small) {

    TTF_Font* f = small ? fontSmall : font;
    if (!f) {
        f = fontSmall;
        if (!f) { return; }
    }


    SDL_Surface* surface = TTF_RenderText_Solid(f, text.c_str(),
        SDL_Color{c.r, c.g, c.b, c.a});

    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect rect = {x, y, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
}

void Renderer::drawWorldRect(float worldX, float worldY, float w, float h, Color c) {
    if (!camera) {
        drawRect(worldX, worldY, w, h, c);
        return;
    }

    Point screenPos = camera->worldToScreen(Point(worldX, worldY));
    drawRect(screenPos.x, screenPos.y, w, h, c);
}

void Renderer::fillWorldRect(float worldX, float worldY, float w, float h, Color c) {
    if (!camera) {
        fillRect(worldX, worldY, w, h, c);
        return;
    }

    Point screenPos = camera->worldToScreen(Point(worldX, worldY));
    fillRect(screenPos.x, screenPos.y, w, h, c);
}

void Renderer::drawWorldLine(float worldX1, float worldY1, float worldX2, float worldY2, Color c) {
    if (!camera) {
        drawLine(worldX1, worldY1, worldX2, worldY2, c);
        return;
    }

    Point screenP1 = camera->worldToScreen(Point(worldX1, worldY1));
    Point screenP2 = camera->worldToScreen(Point(worldX2, worldY2));
    drawLine(screenP1.x, screenP1.y, screenP2.x, screenP2.y, c);
}

void Renderer::drawWorldText(const std::string& text, float worldX, float worldY, Color c, bool small) {
    if (!camera) {
        drawText(text, worldX, worldY, c, small);
        return;
    }

    Point screenPos = camera->worldToScreen(Point(worldX, worldY));
    drawText(text, screenPos.x, screenPos.y, c, small);
}

void Renderer::drawButton(const std::string& text, int x, int y, bool selected, Color baseColor) {
    int textW, textH;
    TTF_SizeText(font, text.c_str(), &textW, &textH);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    if (selected) {
        SDL_SetRenderDrawColor(renderer, 100, 100, 200, 200);
        SDL_Rect bg = {x - 20, y - 5, textW + 40, textH + 10};
        SDL_RenderFillRect(renderer, &bg);

        SDL_SetRenderDrawColor(renderer, 200, 200, 255, 255);
        SDL_RenderDrawRect(renderer, &bg);

        drawText(text, x, y, Color::Yellow(), false);
    } else {
        SDL_SetRenderDrawColor(renderer, 50, 50, 100, 150);
        SDL_Rect bg = {x - 15, y - 3, textW + 30, textH + 6};
        SDL_RenderFillRect(renderer, &bg);

        SDL_SetRenderDrawColor(renderer, 100, 100, 150, 255);
        SDL_RenderDrawRect(renderer, &bg);

        drawText(text, x, y, baseColor, false);
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void Renderer::drawPanel(int x, int y, int w, int h, Color bgColor, Color borderColor) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_Rect bg = {x, y, w, h};
    SDL_RenderFillRect(renderer, &bg);

    SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    SDL_RenderDrawRect(renderer, &bg);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void Renderer::drawSlider(int x, int y, int w, float value, const std::string& label) {

    if (value < 0) value = 0;
    if (value > 1) value = 1;

    drawText(label, x - 150, y - 5, Color::White(), false);

    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_Rect bar = {x, y, w, 10};
    SDL_RenderFillRect(renderer, &bar);

    SDL_SetRenderDrawColor(renderer, 100, 200, 100, 255);
    SDL_Rect valueBar = {x, y, (int)(w * value), 10};
    SDL_RenderFillRect(renderer, &valueBar);

    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(renderer, &bar);

    drawText(std::to_string((int)(value * 100)) + "%", x + w + 10, y - 5, Color::Yellow(), true);
}

void Renderer::drawQuad(Quad* node) {

    if (!node) return;

    Rect worldBounds = node->bounds;
    drawWorldRect(worldBounds.x, worldBounds.y,
                  worldBounds.width, worldBounds.height, Color::Red());

    std::function<void(Quad*)> drawDivisions = [&](Quad* n) {
        if (!n || !n->subdivided) return;

        float midX = n->bounds.x + n->bounds.width / 2.0f;
        float midY = n->bounds.y + n->bounds.height / 2.0f;

        drawWorldLine(midX, n->bounds.y, midX, n->bounds.y + n->bounds.height, Color::Red());
        drawWorldLine(n->bounds.x, midY, n->bounds.x + n->bounds.width, midY, Color::Red());

        drawDivisions(n->TopLeft);
        drawDivisions(n->TopRight);
        drawDivisions(n->BotLeft);
        drawDivisions(n->BotRight);
    };

    drawDivisions(node);
}

void Renderer::renderMainMenu(Game* game, int selectedOption) {
    clear();

    for (int i = 0; i < height; i++) {
        float ratio = (float)i / height;
        SDL_SetRenderDrawColor(renderer,
                               20 + 30 * ratio,   // R
                               20 + 20 * ratio,   // G
                               40 + 40 * ratio,   // B
                               255);
        SDL_RenderDrawLine(renderer, 0, i, width, i);
    }

    std::string title = "AGAR.IO";
    int titleX = width/2 - 150;
    int titleY = height/4;

    drawText(title, titleX + 5, titleY + 5, Color(0, 0, 0, 128), false);

    for (int i = 0; i < 5; i++) {
        float intensity = 1.0f - i * 0.1f;
        drawText(title, titleX - i, titleY - i,
                 Color(100 * intensity, 200 * intensity, 255 * intensity, 255), false);
    }

    std::vector<std::string> options = {"JUGAR", "OPCIONES", "SALIR"};
    int startY = height/2;

    for (size_t i = 0; i < options.size(); i++) {
        drawButton(options[i], width/2 - 50, startY + i * 50, (i == selectedOption));
    }

    drawText("Usa UP/DOWN para navegar, ENTER para seleccionar",
             10, height - 60, Color(200, 200, 200, 255), true);

    present();
}


void Renderer::renderOptionsMenu(Game* game, int selectedOption) {
    clear();

    SDL_SetRenderDrawColor(renderer, 30, 30, 40, 255);
    SDL_RenderClear(renderer);

    drawText("OPCIONES", width/2 - 80, 60, Color::Cyan(), false);

    auto& settings = game->getSettings();
    int startY = 150;
    int spacing = 70;
    drawPanel(150, startY - 30, 700, spacing * 6 + 30, Color(20, 20, 30, 200), Color::White());

    Color optColor = (selectedOption == 0) ? Color::Yellow() : Color::White();
    drawText("VEL. JUGADOR: " + std::to_string(settings.playerSpeed),
             200, startY, optColor, false);
    drawSlider(500, startY, 200, settings.playerSpeed / 5.0f, "");

    optColor = (selectedOption == 1) ? Color::Yellow() : Color::White();
    drawText("VEL. BOTS: " + std::to_string(settings.botSpeed),
             200, startY + spacing, optColor, false);
    drawSlider(500, startY + spacing, 200, settings.botSpeed / 5.0f, "");


    drawText("ESC para volver sin guardar || ENTER para guardar", 10, height - 30, Color::White(), true);

    present();
}

void Renderer::renderPauseMenu(Game* game, int selectedOption) {
    clear();
    drawGrid();

    if (game->getConfig().showQuad) {
        drawQuad(game->getCollisionSystem()->getQuad());
    }

    const auto& entities = game->getEntities();
    for (auto entity : entities) {
        if (entity->active && camera->isVisible(entity->position, entity->radius)) {
            renderEntity(entity);
        }
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_Rect overlay = {0, 0, width, height};
    SDL_RenderFillRect(renderer, &overlay);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    int panelW = 400;
    int panelH = 350;
    int panelX = width/2 - panelW/2;
    int panelY = height/2 - panelH/2;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 30, 30, 40, 230);
    SDL_Rect panel = {panelX, panelY, panelW, panelH};
    SDL_RenderFillRect(renderer, &panel);

    SDL_SetRenderDrawColor(renderer, 100, 100, 150, 255);
    SDL_RenderDrawRect(renderer, &panel);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    drawText("PAUSA", panelX + panelW/2 - 60, panelY + 30, Color::Cyan(), false);

    std::vector<std::string> options = {"CONTINUAR", "OPCIONES", "REINICIAR", "MENU PRINCIPAL"};
    int startY = panelY + 100;

    for (size_t i = 0; i < options.size(); i++) {
        int textX = panelX + panelW/2 - 70;
        int textY = startY + i * 50;

        if (i == selectedOption) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 100, 100, 50, 200);
            SDL_Rect selBg = {textX - 10, textY - 5, 160, 30};
            SDL_RenderFillRect(renderer, &selBg);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

            drawText(options[i], textX, textY, Color::Yellow(), false);
        } else {
            drawText(options[i], textX, textY, Color::White(), false);
        }
    }

    drawText("UP/DOWN para navegar, ENTER para seleccionar",
             panelX + 20, panelY + panelH - 40, Color(200, 200, 200, 255), true);

    present();
}

void Renderer::renderGameOver(Game* game, int selectedOption) {

    clear();
    drawGrid();

    if (game->getConfig().showQuad) {
        drawQuad(game->getCollisionSystem()->getQuad());
    }

    const auto& entities = game->getEntities();
    for (auto entity : entities) {
        if (entity->active && camera->isVisible(entity->position, entity->radius)) {
            renderEntity(entity);
        }
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_Rect overlay = {0, 0, width, height};
    SDL_RenderFillRect(renderer, &overlay);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    Player* player = game->getPlayer();
    float timer = game->getGameOverTimer();
    float delay = 1.0f;

    // Panel principal
    int panelW = 500;
    int panelH = 350;
    int panelX = width/2 - panelW/2;
    int panelY = height/2 - panelH/2;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 40, 20, 20, 230);
    SDL_Rect panel = {panelX, panelY, panelW, panelH};
    SDL_RenderFillRect(renderer, &panel);

    SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
    SDL_RenderDrawRect(renderer, &panel);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    drawText("GAME OVER", panelX + panelW/2 - 120, panelY + 40, Color::Red(), false);

    drawText("Puntaje: " + std::to_string(player->score),
             panelX + 150, panelY + 100, Color::White(), false);
    drawText("Masa final: " + std::to_string((int)player->mass),
             panelX + 150, panelY + 135, Color::White(), false);

    if (timer < delay) {
        int remaining = (int)(delay - timer) + 1;
        drawText("Espera " + std::to_string(remaining) + "...",
                 panelX + panelW/2 - 70, panelY + 200, Color::Yellow(), false);
    } else {
        std::vector<std::string> options = {"REAPARECER", "MENU PRINCIPAL"};
        int startY = panelY + 200;

        for (size_t i = 0; i < options.size(); i++) {
            int textX = panelX + panelW/2 - 70;
            int textY = startY + i * 50;

            if (i == selectedOption) {
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(renderer, 100, 100, 50, 200);
                SDL_Rect selBg = {textX - 10, textY - 5, 160, 30};
                SDL_RenderFillRect(renderer, &selBg);
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

                drawText(options[i], textX, textY, Color::Yellow(), false);
            } else {
                drawText(options[i], textX, textY, Color::White(), false);
            }
        }
    }

    drawText("UP/DOWN para navegar, ENTER para seleccionar",
             panelX + 70, panelY + panelH - 40, Color(200, 200, 200, 255), true);

    present();
}

void Renderer::renderVictoryScreen(Game* game, int selectedOption) {
    clear();
    drawGrid();

    if (game->getConfig().showQuad) {
        drawQuad(game->getCollisionSystem()->getQuad());
    }

    const auto& entities = game->getEntities();
    for (auto entity : entities) {
        if (entity->active && camera->isVisible(entity->position, entity->radius)) {
            renderEntity(entity);
        }
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 180);  // Dorado
    SDL_Rect overlay = {0, 0, width, height};
    SDL_RenderFillRect(renderer, &overlay);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    Player* player = game->getPlayer();

    int panelW = 600;
    int panelH = 400;
    int panelX = width/2 - panelW/2;
    int panelY = height/2 - panelH/2;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_Rect panel = {panelX, panelY, panelW, panelH};
    SDL_RenderFillRect(renderer, &panel);

    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
    SDL_RenderDrawRect(renderer, &panel);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    drawText("! VICTORIA !", panelX + panelW/2 - 140, panelY + 50, Color::Yellow(), false);
    drawText("! VICTORIA !", panelX + panelW/2 - 142, panelY + 48, Color::Orange(), false);

    drawText("¡Has dominado el mundo!", panelX + 150, panelY + 120, Color::White(), false);
    drawText("Tamano final: " + std::to_string((int)(player->radius * 2)),
             panelX + 200, panelY + 160, Color::Cyan(), false);
    drawText("Masa final: " + std::to_string((int)player->mass),
             panelX + 200, panelY + 195, Color::Green(), false);
    drawText("Puntaje: " + std::to_string(player->score),
             panelX + 200, panelY + 230, Color::Yellow(), false);

    std::vector<std::string> options = {"JUGAR DE NUEVO", "MENU PRINCIPAL"};
    int startY = panelY + 280;

    for (size_t i = 0; i < options.size(); i++) {
        int textX = panelX + panelW/2 - 80;
        int textY = startY + i * 50;

        if (i == selectedOption) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 255, 215, 0, 100);
            SDL_Rect selBg = {textX - 10, textY - 5, 180, 30};
            SDL_RenderFillRect(renderer, &selBg);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

            drawText(options[i], textX, textY, Color::Yellow(), false);
        } else {
            drawText(options[i], textX, textY, Color::White(), false);
        }
    }

    drawText("UP/DOWN para navegar, ENTER para seleccionar",
             panelX + 120, panelY + panelH - 40, Color(200, 200, 200, 255), true);

    present();
}

void Renderer::renderGame(Game* game, float fps) {

    if (!game) return;

    if (!camera) {
        camera = game->getCamera();
    }

    switch (game->getState()) {
        case GameState::MENU_MAIN:
            renderMainMenu(game, 0);
            break;

        case GameState::PLAYING: {
            clear();
            drawGrid();

            if (game->getConfig().showQuad) {
                drawQuad(game->getCollisionSystem()->getQuad());
            }

            const auto& entities = game->getEntities();
            for (auto entity : entities) {
                if (entity->active && camera->isVisible(entity->position, entity->radius)) {
                    renderEntity(entity);
                }
            }

            renderHUD(game, fps);
            present();
            break;
        }

        case GameState::GAME_OVER: {
            clear();
            drawGrid();

            const auto& entitiesBg = game->getEntities();
            for (auto entity : entitiesBg) {
                if (entity->active && camera->isVisible(entity->position, entity->radius)) {
                    renderEntity(entity);
                }
            }

            renderGameOver(game, 1);
            break;
        }
    }
}

void Renderer::renderEntity(GameEntity* entity) {

    if (!entity->active) return;

    auto [color1, color2] = entity->getColor();

    float worldX = entity->position.x - entity->radius;
    float worldY = entity->position.y - entity->radius;
    float size = entity->radius * 2;

    if (entity->justAte) {
        size += entity->pulseScale;
        worldX -= (size - entity->radius * 2) / 2;
        worldY -= (size - entity->radius * 2) / 2;
    }

    Point screenPos = camera->worldToScreen(Point(worldX, worldY));
    float screenSize = camera->worldToScreenSize(size);

    fillRect(screenPos.x, screenPos.y, screenSize, screenSize, color1);
    drawRect(screenPos.x, screenPos.y, screenSize, screenSize, color2);

    if (entity->type == EntityType::PLAYER) {
        drawRect(screenPos.x - 1, screenPos.y - 1,
                screenSize + 2, screenSize + 2, Color::Green());
    }

    if (entity->radius > ENTITY_TEXT_MIN_RADIUS && camera->getZoomLevel() > ENTITY_TEXT_MIN_ZOOM) {
        std::string massText = std::to_string((int)entity->mass);
        Point textPos = camera->worldToScreen(Point(
            entity->position.x - 10,
            entity->position.y - 10
        ));

        Color textColor = Color::Black();
        drawText(massText, textPos.x, textPos.y, textColor, true);
    }
}

void Renderer::drawGrid() {

    if (!camera) return;

    Rect viewRect = camera->getViewRect();
    float zoom = camera->getZoomLevel();

    float gridSpacing = GRID_BASE_SPACING;
    if (zoom < GRID_ZOOM_LOW_THRESHOLD) gridSpacing = GRID_SPACING_FAR;
    if (zoom > GRID_ZOOM_HIGH_THRESHOLD) gridSpacing = GRID_SPACING_CLOSE;

    float startX = floor(viewRect.x / gridSpacing) * gridSpacing;
    float startY = floor(viewRect.y / gridSpacing) * gridSpacing;
    float endX = viewRect.x + viewRect.width;
    float endY = viewRect.y + viewRect.height;

    SDL_SetRenderDrawColor(renderer, 40, 40, 50, 255);

    for (float x = startX; x <= endX; x += gridSpacing) {
        if (x >= viewRect.x && x <= viewRect.x + viewRect.width) {
            Point screenStart = camera->worldToScreen(Point(x, viewRect.y));
            Point screenEnd = camera->worldToScreen(Point(x, viewRect.y + viewRect.height));
            drawLine(screenStart.x, screenStart.y, screenEnd.x, screenEnd.y, Color(70, 70, 80));
        }
    }

    for (float y = startY; y <= endY; y += gridSpacing) {
        if (y >= viewRect.y && y <= viewRect.y + viewRect.height) {
            Point screenStart = camera->worldToScreen(Point(viewRect.x, y));
            Point screenEnd = camera->worldToScreen(Point(viewRect.x + viewRect.width, y));
            drawLine(screenStart.x, screenStart.y, screenEnd.x, screenEnd.y, Color(70, 70, 80));
        }
    }
}

void Renderer::renderHUD(Game* game, float fps) {

    Player* player = game->getPlayer();
    auto& stats = game->getCollisionSystem()->getStats();

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 120);

    if (game->getConfig().showQuad) {

        SDL_Rect bg = {5, 5, 250, 180};
        SDL_RenderFillRect(renderer, &bg);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

        if (camera) {

            Point camPos = camera->getPosition();
            drawText("Cam: (" + std::to_string((int)camPos.x) + "," +
                                std::to_string((int)camPos.y) + ")", 20, 145, Color::Magenta(), true);
            drawText("Zoom: " + std::to_string((int)(camera->getZoomLevel() * 100)) + "%",
                     20, 160, Color::Cyan(), true);
        }

        drawText("Puntaje: " + std::to_string(player->score), 20, 20, Color::White());
        drawText("Masa: " + std::to_string((int)player->mass), 20, 45, Color::Green());
        drawText("Tamano: " + std::to_string((int)player->radius), 20, 70, Color::Green());
        drawText("Entidades: " + std::to_string(stats.activeEntities), 20, 95, Color::Cyan());
        drawText("Nodos QT: " + std::to_string(stats.quadNodes), 20, 120, Color::Yellow());

        drawText("[DEBUG] QuadTree ON", 10, height - 60, Color::Magenta());
    }

    drawText("WASD: Mover | Q+Shift: DT | ESC: Salir", 10, height - 30, Color::Black(), true);
}