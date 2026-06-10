#include "Player.hpp"
#include <cmath>

int health = 100;
int ammo = 50;
int score = 0;
int lives = 3;
bool blueKey = false;
bool goldKey = false;

Player::Player(sf::Vector2f startPos)
{
    pos = startPos;
    angle = 0.0f;

    health = 100;
    ammo = 50;
    score = 0;
    lives = 3;
}

void Player::update(float deltaTime, MapManager& mapManager) {
    float baseSpeed = 180.0f;

if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
{
    baseSpeed = 300.0f;
}

float moveSpeed = baseSpeed * deltaTime;

    sf::Vector2f oldPos = pos;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        pos.x += std::cos(angle) * moveSpeed;
        pos.y += std::sin(angle) * moveSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        pos.x -= std::cos(angle) * moveSpeed;
        pos.y -= std::sin(angle) * moveSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        pos.x += std::cos(angle - 1.570796f) * moveSpeed;
        pos.y += std::sin(angle - 1.570796f) * moveSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        pos.x += std::cos(angle + 1.570796f) * moveSpeed;
        pos.y += std::sin(angle + 1.570796f) * moveSpeed;
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