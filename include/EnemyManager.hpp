#pragma once
#include <vector>
#include <memory>
#include "Enemy.hpp"

class EnemyManager {
private:
    std::vector<Enemy> enemies;

public:
    void spawnEnemy(EnemyType type, sf::Vector2f position);
    void update(float dt, sf::Vector2f playerPos, MapManager& mapManager);
    void draw(sf::RenderWindow& window);
    
    // Getter para acceder a la lista si necesitas verificar colisiones con balas
    std::vector<Enemy>& getEnemies() { return enemies; }
};