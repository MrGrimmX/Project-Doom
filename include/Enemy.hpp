#pragma once
#include <SFML/Graphics.hpp>
#include "Map.hpp"
enum class EnemyType
{
    Grunt,
    Merc,
    Recruit,
    Trooper,
    Heavy,
    Cobra,
    Insect,
    Mutant,
    Robot
};

class Enemy
{
public:

    EnemyType type;

    sf::Texture texture;
    sf::Sprite sprite;
    sf::Vector2f pos;
    int health;
    int damage;
    int frameWidth;
    int frameHeight;
    int totalFrames;
    int currentFrame;
    float animationSpeed;
    float animationTimer;
    float speed;
    bool alive;

    Enemy();
    void load(EnemyType t);
    void update(float dt, sf::Vector2f playerPos, MapManager& mapManager);
    void draw(sf::RenderWindow& window);
    void takeDamage(int dmg);
};