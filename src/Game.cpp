#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include "Player.hpp"
#include "Map.hpp"
#include "TextureManager.hpp"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int TILE_SIZE = 64;

class Game {
private:
    sf::RenderWindow window;
    MapManager mapManager;
    Player player;
    sf::Clock gameClock;
    TextureManager textureManager;

void processEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                float lookX = player.pos.x + std::cos(player.angle) * 45.0f;
                float lookY = player.pos.y + std::sin(player.angle) * 45.0f;
                int targetX = (int)(lookX) / TILE_SIZE;
                int targetY = (int)(lookY) / TILE_SIZE;

                if (mapManager.isDoor(targetX, targetY)) {
                    mapManager.openDoor(targetX, targetY);
                }
                else if (mapManager.isSecret(targetX, targetY)) {
                    mapManager.triggerSecret(targetX, targetY, player.angle);
                }
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Tab) {
                mapManager.showFullMap = !mapManager.showFullMap;
            }
        }
    }

    void update(float deltaTime) {
        player.update(deltaTime, mapManager);
        mapManager.updateSecret(deltaTime);
        mapManager.updateDoors(deltaTime, player.pos);
    }

    void drawColumn(int screenX, float distance, float rayAngle, sf::Color color) {
        // Corrección del efecto ojo de pez clásico
        float correctedDistance = distance * std::cos(rayAngle - player.angle);
        if (correctedDistance < 1.0f) correctedDistance = 1.0f;

        // Proyección clásica de Wolfenstein 3D
        int wallHeight = (int)(SCREEN_HEIGHT * TILE_SIZE / correctedDistance);

        int finalTop = (SCREEN_HEIGHT / 2) - (wallHeight / 2);
        int finalBottom = (SCREEN_HEIGHT / 2) + (wallHeight / 2);

        if (finalTop < 0) finalTop = 0;
        if (finalBottom >= SCREEN_HEIGHT) finalBottom = SCREEN_HEIGHT - 1;

        // Sombreado realista por distancia
        float shadow = 1.0f - (distance / 750.0f);
        if (shadow < 0.0f) shadow = 0.0f;
        color.r = (sf::Uint8)(color.r * shadow);
        color.g = (sf::Uint8)(color.g * shadow);
        color.b = (sf::Uint8)(color.b * shadow);

        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f((float)screenX, (float)finalTop), color),
            sf::Vertex(sf::Vector2f((float)screenX, (float)finalBottom), color)
        };
        window.draw(line, 2, sf::Lines);
    }

    void renderRaycast() {
        Level* curLevel = mapManager.getCurrentLevel();
        float fov = 60.0f * 3.14159265f / 180.0f;
        float startAngle = player.angle - fov / 2.0f;
        float angleStep = fov / (float)SCREEN_WIDTH;

        // Render de cielo y suelo fijos
        sf::RectangleShape sky(sf::Vector2f((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT / 2));
        sky.setFillColor(sf::Color(40, 40, 40)); // Techo oscuro estilo mazmorra
        sf::RectangleShape floorRect(sf::Vector2f((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT / 2));
        floorRect.setPosition(0, (float)SCREEN_HEIGHT / 2);
        floorRect.setFillColor(sf::Color(70, 70, 70)); // Suelo cemento
        window.draw(sky);
        window.draw(floorRect);

        for (int i = 0; i < SCREEN_WIDTH; i++) {
            float rayAngle = startAngle + (float)i * angleStep;
            float cosRay = std::cos(rayAngle);
            float sinRay = std::sin(rayAngle);

            float distance = 0.0f;
            float maxDistance = 800.0f;
            float stepSize = 1.5f; // Mayor precisión de rayo para Wolfenstein
            bool hitWall = false;
            sf::Color wallColor;

while (!hitWall && distance < maxDistance) {
    distance += stepSize;
    float rayX = player.pos.x + cosRay * distance;
    float rayY = player.pos.y + sinRay * distance;

    int testX = (int)(rayX) / TILE_SIZE;
    int testY = (int)(rayY) / TILE_SIZE;

    // 🌟 COMPROBACIÓN DINÁMICA DEL PUSHWALL
    if (mapManager.activeSecret.isActive) {
    float wallMinX = mapManager.activeSecret.originalX * 64 + mapManager.activeSecret.moveX;
    float wallMaxX = wallMinX + 64;
    float wallMinY = mapManager.activeSecret.originalY * 64 + mapManager.activeSecret.moveY;
    float wallMaxY = wallMinY + 64;

    if (rayX >= wallMinX && rayX < wallMaxX && rayY >= wallMinY && rayY < wallMaxY) {
        wallColor = sf::Color(110, 115, 120); // 🌟 IGUALADO al color del muro normal (Gris)
        hitWall = true;
        break;
    }
}

if (testX >= 0 && testX < curLevel->cols && testY >= 0 && testY < curLevel->rows) {
    int type = curLevel->grid[testY][testX].type;
    
    if (type == 1) {
        wallColor = sf::Color(110, 115, 120); // Muro normal
        hitWall = true;
    }
    else if (type == 2) {
        if (!curLevel->grid[testY][testX].isOpen) {
            wallColor = sf::Color(140, 45, 140); // Puerta violeta
            hitWall = true;
        }
    }
    else if (type == 3) { 
        wallColor = sf::Color(110, 115, 120); // 🌟 IGUALADO al color del muro normal (Gris)
        hitWall = true;
    }
    else if (type == 9) {
        wallColor = sf::Color(40, 190, 80); // Salida verde
        hitWall = true;
    }
} else {
    hitWall = true;
}
}

            if (hitWall && distance < maxDistance) {
                drawColumn(i, distance, rayAngle, wallColor);
            }
        }
    }

    void render() {
        window.clear();
        if (mapManager.showFullMap) {
            mapManager.drawFullMap(window);
            
            // Dibujar jugador en el mapa
            sf::CircleShape pDot(5);
            pDot.setFillColor(sf::Color::Red);
            pDot.setOrigin(5, 5);
            Level* cur = mapManager.getCurrentLevel();
            float startX = (window.getSize().x - (cur->cols * 32)) / 2.0f;
            float startY = (window.getSize().y - (cur->rows * 32)) / 2.0f;
            pDot.setPosition(startX + (player.pos.x / 64.0f) * 32.0f, startY + (player.pos.y / 64.0f) * 32.0f);
            window.draw(pDot);
        } else {
            renderRaycast();
        }
        window.display();
    }

public:
    Game() : 
    window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Wolfenstein 3D Engine"),
    player(sf::Vector2f(96.0f, 96.0f)),
    textureManager(64)
{
    window.setFramerateLimit(60);
    player.pos = mapManager.getCurrentLevel()->spawnPoint;
    
    // 🌟 Mensaje de diagnóstico para comprobar la ruta
    if (!textureManager.loadTextures("CASTLEBRICKS.png")) {
        std::cerr << "🛑 ERROR: No se encontro 'CASTLEBRICKS.png'. Verifica que este en la raiz del proyecto." << std::endl;
    } else {
        std::cout << "✅ EXITO: ¡'CASTLEBRICKS.png' se cargo correctamente desde la raiz!" << std::endl;
    }
}

    void run() {
        while (window.isOpen()) {
            float deltaTime = gameClock.restart().asSeconds();
            processEvents();
            update(deltaTime);
            render();
        }
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}