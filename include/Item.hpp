#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class Player;

enum class ItemType
{
    Health,
    Armor,
    Ammo
};

enum class ItemVariant
{
    H_50,
    H_100,

    A_25,
    A_50,
    A_100,

    AMMO_PISTOL,
    AMMO_AK,
    AMMO_SHOTGUN,
    AMMO_SKS,
    AMMO_UZI
};

enum class AmmoType
{
    Pistol,
    AK,
    Shotgun,
    SKS,
    Uzi
};

class Item
{
private:
    sf::Sprite sprite;
    sf::Texture texture;

    ItemType type;
    ItemVariant variant;
    AmmoType ammoType;

    int value;

public:
    Item(ItemType t, ItemVariant v, AmmoType a, int val, const std::string& texturePath);

    void apply(Player& player);

    void setPosition(const sf::Vector2f& pos);
    void draw(sf::RenderWindow& window);

    sf::FloatRect getBounds() const;
};