#include "Item.hpp"
#include "Player.hpp"

Item::Item(ItemType t, ItemVariant v, AmmoType a, int val, const std::string& texturePath)
{
    type = t;
    variant = v;
    ammoType = a;
    value = val;

    texture.loadFromFile(texturePath);
    sprite.setTexture(texture);
}

void Item::setPosition(const sf::Vector2f& pos) {
    sprite.setPosition(pos);
}

void Item::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

sf::FloatRect Item::getBounds() const {
    return sprite.getGlobalBounds();
}

void Item::apply(Player& player)
{
    if(type == ItemType::Health)
    {
        player.health += value;

        if(player.health > 100)
            player.health = 100;
    }

    else if(type == ItemType::Armor)
    {
        player.armor += value;

        if(player.armor > 100)
            player.armor = 100;
    }

    else if(type == ItemType::Ammo)
    {
        switch(ammoType)
        {
            case AmmoType::Pistol:
                player.pistolAmmo += value;
                break;

            case AmmoType::AK:
                player.akAmmo += value;
                break;

            case AmmoType::Shotgun:
                player.shotgunAmmo += value;
                break;

            case AmmoType::SKS:
                player.sksAmmo += value;
                break;

            case AmmoType::Uzi:
                player.uziAmmo += value;
                break;
        }
    }
}