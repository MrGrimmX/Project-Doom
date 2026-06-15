#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>
#include "Map.hpp"
#include "Weapons.hpp"

class Player {
public:
    sf::Vector2f pos;
    sf::Vector2f getPosition() const;
    float angle;

    // Estadísticas del jugador
    int health = 100;
    int armor = 0;
    int pistolAmmo = 50;
    int akAmmo = 60;
    int shotgunAmmo = 30;
    int sksAmmo = 20;
    int uziAmmo = 120;
    int score;
    int lives;
    bool consumeAmmo();
    int getCurrentAmmo() const;
    
    Weapon pistol;
    Weapon ak;
    Weapon shotgun;
    Weapon sks;
    Weapon uzi;

    WeaponType currentWeapon;

    Weapon* getCurrentWeapon();

    Player(sf::Vector2f startPos);
    void update(float deltaTime, MapManager& mapManager);
    void switchWeapon(WeaponType newWeapon);
private:
    sf::RectangleShape body;
};

#endif