#include "Player.hpp"
#include <cmath>

Player::Player(sf::Vector2f spawnPos) {
    pos = spawnPos;
    angle = 0.0f;
    cameraZ = 0.0f;
}

void Player::update(float deltaTime, MapManager& mapManager) {
    float moveSpeed = 150.0f * deltaTime;
    float rotSpeed = 3.0f * deltaTime;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) angle -= rotSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) angle += rotSpeed;

    sf::Vector2f oldPos = pos;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        pos.x += std::cos(angle) * moveSpeed;
        pos.y += std::sin(angle) * moveSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        pos.x -= std::cos(angle) * moveSpeed;
        pos.y -= std::sin(angle) * moveSpeed;
    }

    int cellX = (int)(pos.x) / 64;
    int cellY = (int)(pos.y) / 64;
    Level* cur = mapManager.getCurrentLevel();

    if (cellX >= 0 && cellX < cur->cols && cellY >= 0 && cellY < cur->rows) {
        // Bloqueo de muros y puertas cerradas
        if (cur->grid[cellY][cellX].type == 1 || cur->grid[cellY][cellX].type == 2) {
            pos = oldPos;
            cellX = (int)(pos.x) / 64;
            cellY = (int)(pos.y) / 64;
        }

        // Sistema de altura fluido
        float currentGroundHeight = cur->grid[cellY][cellX].floorHeight;
        float targetCameraZ = currentGroundHeight * 220.0f;
        cameraZ += (targetCameraZ - cameraZ) * 8.0f * deltaTime;
    }

    if (mapManager.isExit(cellX, cellY)) {
        mapManager.nextLevel();
        pos = mapManager.getCurrentLevel()->spawnPoint;
    }
}

void Player::drawOnFullMap(sf::RenderWindow& window, MapManager& mapManager) {
    Level* cur = mapManager.getCurrentLevel();
    int tileSize = 32;
    float startX = (window.getSize().x - (cur->cols * tileSize)) / 2.0f;
    float startY = (window.getSize().y - (cur->rows * tileSize)) / 2.0f;

    sf::CircleShape pDot(5.0f);
    pDot.setFillColor(sf::Color::Red);
    
    float mapX = startX + (pos.x / 64.0f) * tileSize;
    float mapY = startY + (pos.y / 64.0f) * tileSize;
    
    pDot.setPosition(mapX - 2.5f, mapY - 2.5f);
    window.draw(pDot);
}