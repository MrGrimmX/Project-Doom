#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "Map.hpp"
#include "TextureManager.hpp"
#include "Menu.hpp"

class Game {
private:
    sf::RenderWindow window;
    MapManager mapManager;
    Player player;
    sf::Clock gameClock;
    TextureManager textureManager;
    Menu menu;
    GameState gameState;
    sf::Font hudFont;

    void processEvents();
    void update(float deltaTime);
    void render();
    void renderRaycast();
    void renderHUD();
    
public:
    Game();
    ~Game();
    void run();
};

#endif