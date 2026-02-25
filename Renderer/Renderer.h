//
// Created by ignac on 9/02/2026.
//

#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <string>
#include "../Quad/Quad.h"
#include "Camera.h"

class Game;
class GameEntity;

struct Color {
    Uint8 r, g, b, a;
    Color(Uint8 red = 255, Uint8 green = 255, Uint8 blue = 255, Uint8 alpha = 255) : r(red), g(green), b(blue), a(alpha) {}

    static Color White() { return Color(255, 255, 255, 255); }
    static Color Black() { return Color(0, 0, 0, 255); }
    static Color Red() { return Color(255, 0, 0, 255); }
    static Color Green() { return Color(0, 255, 0, 255); }
    static Color Blue() { return Color(0, 0, 255, 255); }
    static Color Yellow() { return Color(255, 255, 0, 255); }
    static Color Cyan() { return Color(0, 255, 255, 255); }
    static Color Magenta() { return Color(255, 0, 255, 255); }
    static Color Orange() { return Color(255, 165, 0, 255); }
    static Color Purple() { return Color(128, 0, 128, 255); }
};

class Renderer {

    private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    TTF_Font* fontSmall;
    int width, height;

    Camera* camera;

    SDL_Texture* playerTexture;
    SDL_Texture* botTexture;
    SDL_Texture* pelletTexture;

    const float TEXTURE_BASE_RADIUS = 32.0f;

    public:
    Renderer(int w, int h) : window(nullptr), renderer(nullptr), font(nullptr), fontSmall(nullptr), width(w), height(h) {}

    ~Renderer() { cleanup(); }

    bool init();

    void cleanup();

    void clear();

    void present();

    void drawCircle(int cx, int cy, int r, Color c);

    void fillCircle(int cx, int cy, int r, Color c);

    void drawRect(int x, int y, int w, int h, Color c);

    void fillRect(int x, int y, int w, int h, Color c);

    void drawLine(int x1, int y1, int x2, int y2, Color c);

    void drawText(const std::string& text, int x, int y, Color c, bool small = false);

    void drawEntityTextured(GameEntity* entity);
    SDL_Texture* loadTexture(const std::string& path);

    void drawWorldRect(float worldX, float worldY, float w, float h, Color c);
    void fillWorldRect(float worldX, float worldY, float w, float h, Color c);
    void drawWorldLine(float worldX1, float worldY1, float worldX2, float worldY2, Color c);
    void drawWorldText(const std::string& text, float worldX, float worldY, Color c, bool small = false);

    void drawButton(const std::string& text, int x, int y, bool selected, Color baseColor = Color::White());
    void drawPanel(int x, int y, int w, int h, Color bgColor, Color borderColor);
    void drawSlider(int x, int y, int w, float value, const std::string& label);

    void setCamera(Camera* camera_) { camera = camera_; }

    void drawQuad(Quad* quad);

    void renderMainMenu(Game* game, int selectedOption);
    void renderOptionsMenu(Game* game, int selectedOption);
    void renderHowToMenu(Game* game);
    void renderStatsMenu(Game* game);
    void renderPauseMenu(Game* game, int selectedOption);
    void renderVictoryScreen(Game* game, int selectedOption);

    void renderMenu(Game* game);

    void renderGameOver(Game* game, int selectedOption);

    void renderLosing(Game* game, int selectedOption);

    void renderGame(Game* game, float fps);

    void renderEntity(GameEntity* entity);

    void drawGrid();

    void renderHUD(Game* game, float fps);

    SDL_Renderer* getRenderer() { return renderer; }
};



#endif //RENDERER_H
