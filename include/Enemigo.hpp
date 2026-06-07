#pragma once
#include <SFML/Graphics.hpp>

class Enemy {
public:
    sf::Vector2f pos;
    float health;
    bool isDead;
    
    sf::Texture texture;
    sf::Sprite sprite;

    Enemy(sf::Vector2f startPos);
    void update(float deltaTime, sf::Vector2f playerPos);
    void draw(sf::RenderWindow& window, sf::Vector2f playerPos, float playerAngle);
};