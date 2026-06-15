#include "Enemy.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>

Enemy::Enemy()
{
    health = 100;
    damage = 10;
    speed = 30.f;
    alive = true;
    pos = sf::Vector2f(0.f, 0.f);
    currentFrame = 0;
    animationTimer = 0.f;
    frameWidth = 0;
    frameHeight = 0;
    totalFrames = 0;
}

void Enemy::load(EnemyType t)
{
    type = t;

    switch(type)
    {
        case EnemyType::Grunt:
if(!texture.loadFromFile("assets/Enemies/Grunt.png"))
{
    std::cout << "ERROR cargando Grunt.png" << std::endl;
}
else
{
    std::cout
        << "Grunt cargado: "
        << texture.getSize().x
        << "x"
        << texture.getSize().y
        << std::endl;
}
            sprite.setTexture(texture);
            frameWidth = 64;
            frameHeight = 80;
            totalFrames = 30;
            animationSpeed = 0.06f;
            health = 100;
            speed = 30.f;
            damage = 20;
            break;

        case EnemyType::Merc:
            texture.loadFromFile("assets/Enemies/Merc.png");
            sprite.setTexture(texture);
            frameWidth = 64;
            frameHeight = 80;
            totalFrames = 30;
            animationSpeed = 0.06f;
            health = 150;
            speed = 15.f;
            damage = 30;
            break;

        case EnemyType::Recruit:
            texture.loadFromFile("assets/Enemies/Recruit.png");
            sprite.setTexture(texture);
            frameWidth = 64;
            frameHeight = 80;
            totalFrames = 30;
            animationSpeed = 0.06f;
            health = 60;
            speed = 50.f;
            damage = 10;
            break;
        
        case EnemyType::Trooper:
            texture.loadFromFile("assets/Enemies/Trooper.png");
            sprite.setTexture(texture);
            frameWidth = 64;
            frameHeight = 80;
            totalFrames = 30;
            animationSpeed = 0.06f;
            health = 60;
            speed = 50.f;
            damage = 10;
            break;

        case EnemyType::Heavy:
            texture.loadFromFile("assets/Enemies/Heavy.png");
            sprite.setTexture(texture);
            frameWidth = 64;
            frameHeight = 64;
            totalFrames = 9;
            animationSpeed = 0.06f;
            health = 200;
            speed = 40.f;
            damage = 50;
            break;

        case EnemyType::Cobra:
            texture.loadFromFile("assets/Enemies/Cobra.png");
            sprite.setTexture(texture);
            frameWidth = 64;
            frameHeight = 64;
            totalFrames = 11;
            animationSpeed = 0.06f;
            health = 175;
            speed = 40.f;
            damage = 50;
            break;

        case EnemyType::Insect:
            texture.loadFromFile("assets/Enemies/Insect.png");
            sprite.setTexture(texture);
            frameWidth = 64;
            frameHeight = 64;
            totalFrames = 9;
            animationSpeed = 0.06f;
            health = 150;
            speed = 40.f;
            damage = 50;
            break;

        case EnemyType::Mutant:
            texture.loadFromFile("assets/Enemies/Mutant.png");
            sprite.setTexture(texture);
            frameWidth = 64;
            frameHeight = 64;
            totalFrames = 9;
            animationSpeed = 0.06f;
            health = 125;
            speed = 40.f;
            damage = 25;
            break;

        case EnemyType::Robot:
            texture.loadFromFile("assets/Enemies/Robot.png");
            sprite.setTexture(texture);
            frameWidth = 64;
            frameHeight = 64;
            totalFrames = 7;
            animationSpeed = 0.06f;
            health = 400;
            speed = 50.f;
            damage = 75;
            break;
    }

    currentFrame = 0;
    animationTimer = 0.f;

    sprite.setTextureRect(
        sf::IntRect(0, 0, frameWidth, frameHeight)
    );

    sprite.setOrigin(
        frameWidth / 2.f,
        frameHeight / 2.f
    );
}

void Enemy::update(float dt,
                   sf::Vector2f playerPos,
                   MapManager& mapManager)
{
    if(!alive)
        return;

    sf::Vector2f dir = playerPos - pos;

    float length =
        std::sqrt(dir.x * dir.x +
                  dir.y * dir.y);

    if(length < 1.f)
        return;

    dir /= length;

    sf::Vector2f oldPos = pos;

    pos += dir * speed * dt;

    int cellX = (int)(pos.x) / 64;
    int cellY = (int)(pos.y) / 64;

    if(mapManager.isWall(cellX, cellY))
    {
        pos = oldPos;
    }

    sprite.setPosition(pos);

    animationTimer += dt;

    if(animationTimer >= animationSpeed)
    {
        animationTimer = 0.f;

        currentFrame++;

        if(currentFrame >= totalFrames)
            currentFrame = 0;

        int columns =
            texture.getSize().x / frameWidth;

        int x =
            (currentFrame % columns) *
            frameWidth;

        sprite.setTextureRect(
            sf::IntRect(
                x,
                0,
                frameWidth,
                frameHeight
            )
        );
    }
}

void Enemy::draw(sf::RenderWindow& window)
{
    if(alive)
        window.draw(sprite);
}

void Enemy::takeDamage(int dmg)
{
    health -= dmg;

    if(health <= 0)
        alive = false;
}