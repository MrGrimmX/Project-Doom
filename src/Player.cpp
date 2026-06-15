#include "Player.hpp"
#include <cmath>
sf::Vector2f Player::getPosition() const {
    return body.getPosition(); // Aquí usamos el nombre que definimos en el .hpp
}

Player::Player(sf::Vector2f startPos)
{
    pos = startPos;
    angle = 0.0f;

    health = 100;
    armor = 0;
    pistolAmmo = 50;
    akAmmo = 120;
    shotgunAmmo = 30;
    sksAmmo = 80;
    uziAmmo = 200;
    score = 0;
    lives = 3;

    currentWeapon = WeaponType::Pistol;

    pistol.texture.loadFromFile("assets/weapons/Pistol.png");
    pistol.sprite.setTexture(pistol.texture);
    pistol.frameWidth = 320;
    pistol.frameHeight = 180;
    pistol.totalFrames = 4;
    pistol.fireRate = 0.30f;
    pistol.automatic = false;
    pistol.animationSpeed = 0.06f;
    pistol.damage = 15;
    pistol.sprite.setTextureRect(sf::IntRect(0,0,320,180));

    // AK
    ak.texture.loadFromFile("assets/weapons/AK.png");
    ak.sprite.setTexture(ak.texture);
    ak.frameWidth = 320;
    ak.frameHeight = 180;
    ak.totalFrames = 4;
    ak.fireRate = 0.10f;
    ak.automatic = true;
    ak.animationSpeed = 0.05f;
    ak.damage = 30;
    ak.sprite.setTextureRect(sf::IntRect(0,0,320,180));

    // SHOTGUN
    shotgun.texture.loadFromFile("assets/weapons/Shotgun.png");
    shotgun.sprite.setTexture(shotgun.texture);
    shotgun.frameWidth = 320;
    shotgun.frameHeight = 180;
    shotgun.totalFrames = 6;
    shotgun.fireRate = 0.90f;
    shotgun.automatic = false;
    shotgun.animationSpeed = 0.10f;
    shotgun.damage = 50;
    shotgun.sprite.setTextureRect(sf::IntRect(0,0,320,180));

    // SKS
    sks.texture.loadFromFile("assets/weapons/SKS.png");
    sks.sprite.setTexture(sks.texture);
    sks.frameWidth = 320;
    sks.frameHeight = 180;
    sks.totalFrames = 12;
    sks.fireRate = 0.70f;
    sks.automatic = false;
    sks.animationSpeed = 0.09f;
    sks.damage = 60;
    sks.sprite.setTextureRect(sf::IntRect(0,0,320,180));

    // UZI
    uzi.texture.loadFromFile("assets/weapons/Uzi.png");
    uzi.sprite.setTexture(uzi.texture);
    uzi.frameWidth = 320;
    uzi.frameHeight = 180;
    uzi.totalFrames = 4;
    uzi.fireRate = 0.06f;
    uzi.automatic = true;
    uzi.animationSpeed = 0.04f;
    uzi.damage = 20;
    uzi.sprite.setTextureRect(sf::IntRect(0,0,320,180));
}

int Player::getCurrentAmmo() const
{
    switch(currentWeapon)
    {
        case WeaponType::Pistol:
            return pistolAmmo;

        case WeaponType::AK:
            return akAmmo;

        case WeaponType::Shotgun:
            return shotgunAmmo;

        case WeaponType::SKS:
            return sksAmmo;

        case WeaponType::Uzi:
            return uziAmmo;
    }

    return 0;
}

Weapon* Player::getCurrentWeapon()
{
    switch(currentWeapon)
    {
        case WeaponType::Pistol:
            return &pistol;

        case WeaponType::AK:
            return &ak;

        case WeaponType::Shotgun:
            return &shotgun;

        case WeaponType::SKS:
            return &sks;

        case WeaponType::Uzi:
            return &uzi;
    }

    return &pistol;
}
bool Player::consumeAmmo()
{
    switch(currentWeapon)
    {
        case WeaponType::Pistol:
            if(pistolAmmo > 0)
            {
                pistolAmmo--;
                return true;
            }
            break;

        case WeaponType::AK:
            if(akAmmo > 0)
            {
                akAmmo--;
                return true;
            }
            break;

        case WeaponType::Shotgun:
            if(shotgunAmmo > 0)
            {
                shotgunAmmo--;
                return true;
            }
            break;

        case WeaponType::SKS:
            if(sksAmmo > 0)
            {
                sksAmmo--;
                return true;
            }
            break;

        case WeaponType::Uzi:
            if(uziAmmo > 0)
            {
                uziAmmo--;
                return true;
            }
            break;
    }

    return false;
}
void Player::switchWeapon(WeaponType newWeapon)
{
    currentWeapon = newWeapon;
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
}