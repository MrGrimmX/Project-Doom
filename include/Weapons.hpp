#pragma once
#include <SFML/Graphics.hpp>

enum class WeaponType
{
    Pistol,
    AK,
    Shotgun,
    SKS,
    Uzi
};

class Weapon
{
public:
    WeaponType type;
    int damage;
    int ammoCost;
    sf::Texture texture;
    sf::Sprite sprite;
    int frameWidth;
    int frameHeight;
    int totalFrames;
    int currentFrame;
    float animationSpeed;
    float animationTimer;
    float fireRate;
    float fireTimer;
    float fireCooldown;
    bool automatic;
    bool shooting;
    void update(float deltaTime);
    void shoot();
    bool canShoot() const;
    Weapon();
};