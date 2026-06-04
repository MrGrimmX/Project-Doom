#pragma once

#include <sfml/Graphics.hpp>
#include <memory>
#include <array>
#include <cmath>

class Raycaster{
     
    static constexpr int mapWidth = 16, mapHeight = 16;
    std::array<std::string, mapHeight> map;

    float player_x, player_y, player_angle;

    sf::RenderWindow window;
    sf::Clock clock;   

    void events(), draw(), draw_minimap(),
        keys(), rays();

    public:
        Raycaster();
        void run();
};