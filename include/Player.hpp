#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>
#include "Map.hpp"

class Player {
public:
    sf::Vector2f pos;
    float angle;

    Player(sf::Vector2f startPos);
    void update(float deltaTime, MapManager& mapManager);
};

#endif