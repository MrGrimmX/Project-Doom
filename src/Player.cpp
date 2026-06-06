#include "Player.hpp"
#include <cmath>

Player::Player(sf::Vector2f startPos) {
    pos = startPos;
    angle = 0.0f;
}

void Player::update(float deltaTime, MapManager& mapManager) {
    float moveSpeed = 180.0f * deltaTime;
    float rotSpeed = 3.5f * deltaTime;

    // 1. Manejo de la rotación de la cámara
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) angle -= rotSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) angle += rotSpeed;

    // 2. Guardar posición previa antes de aplicar el movimiento
    sf::Vector2f oldPos = pos;

    // 3. Calcular posición tentativa
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        pos.x += std::cos(angle) * moveSpeed;
        pos.y += std::sin(angle) * moveSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        pos.x -= std::cos(angle) * moveSpeed;
        pos.y -= std::sin(angle) * moveSpeed;
    }

    // 4. Mapear las coordenadas flotantes a celdas enteras del mapa
    int cellX = (int)(pos.x) / 64;
    int cellY = (int)(pos.y) / 64;

    // 5. Verificar colisión inteligente (Muros, Puertas cerradas y Secretos en movimiento)
    if (mapManager.isWall(cellX, cellY)) {
        pos = oldPos; // Si la celda es sólida, deshacemos el avance
        
        // Recalcular celdas con la posición restaurada por seguridad
        cellX = (int)(pos.x) / 64;
        cellY = (int)(pos.y) / 64;
    }

    // 6. Cambiar de nivel si se pisa la zona de meta
    if (mapManager.isExit(cellX, cellY)) {
        mapManager.nextLevel();
        pos = mapManager.getCurrentLevel()->spawnPoint;
    }
}