#include "EnemyManager.hpp"

void EnemyManager::spawnEnemy(EnemyType type, sf::Vector2f position) {
    Enemy newEnemy;
    newEnemy.load(type);
    newEnemy.pos = position; // Establecemos la posición inicial
    newEnemy.sprite.setPosition(position);
    enemies.push_back(newEnemy);
}

void EnemyManager::update(float dt, sf::Vector2f playerPos, MapManager& mapManager) {
    for (auto& enemy : enemies) {
        enemy.update(dt, playerPos, mapManager);
    }
}

void EnemyManager::draw(sf::RenderWindow& window) {
    for (auto& enemy : enemies) {
        enemy.draw(window);
    }
}