#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>
#include "Map.hpp"

class Player {
public:
    sf::Vector2f pos;
    float angle;
    float cameraZ; // Altura real de los ojos del jugador (coincide con el suelo donde pisa)

    Player(sf::Vector2f spawnPos);
    void update(float deltaTime, MapManager& mapManager);
    void drawOnFullMap(sf::RenderWindow& window, MapManager& mapManager);
};

#endif