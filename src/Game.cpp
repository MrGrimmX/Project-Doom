#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
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

    void renderRaycast() {
        float fov = 60.0f * (3.14159265f / 180.0f);
        float halfFov = fov / 2.0f;
        int numRays = SCREEN_WIDTH;
        float deltaAngle = fov / (float)numRays;

        Level* curLevel = mapManager.getCurrentLevel();

        for (int i = 0; i < numRays; i++) {
            float rayAngle = (player.angle - halfFov) + (float)i * deltaAngle;
            float cosRay = std::cos(rayAngle);
            float sinRay = std::sin(rayAngle);

            float distance = 0.0f;
            float stepSize = 1.0f; 
            bool hitWall = false;
            int wallType = 1;      
            
            float rayX = player.pos.x;
            float rayY = player.pos.y;

            while (!hitWall && distance < 800.0f) {
                distance += stepSize;
                rayX = player.pos.x + cosRay * distance;
                rayY = player.pos.y + sinRay * distance;

                int testX = (int)(rayX) / TILE_SIZE;
                int testY = (int)(rayY) / TILE_SIZE;

                if (mapManager.activeSecret.isActive) {
                    float wallMinX = mapManager.activeSecret.originalX * 64 + mapManager.activeSecret.moveX;
                    float wallMaxX = wallMinX + 64;
                    float wallMinY = mapManager.activeSecret.originalY * 64 + mapManager.activeSecret.moveY;
                    float wallMaxY = wallMinY + 64;

                    if (rayX >= wallMinX && rayX < wallMaxX && rayY >= wallMinY && rayY < wallMaxY) {
                        wallType = 3; 
                        hitWall = true;
                        break;
                    }
                }

                if (testX >= 0 && testX < curLevel->cols && testY >= 0 && testY < curLevel->rows) {
                    int type = curLevel->grid[testY][testX].type;
                    if (type == 1 || type == 3 || type == 9) {
                        wallType = type;
                        hitWall = true;
                    }
                    else if (type == 2 && !curLevel->grid[testY][testX].isOpen) {
                        wallType = type;
                        hitWall = true;
                    }
                } else {
                    hitWall = true; 
                }
            }

            // --- CÁLCULO DE PROYECCIÓN VERTICAL ---
            float correctedDistance = distance * std::cos(rayAngle - player.angle);
            if (correctedDistance < 1.0f) correctedDistance = 1.0f; 

            int wallHeight = (int)((TILE_SIZE * SCREEN_HEIGHT) / correctedDistance);
            int drawStart = (SCREEN_HEIGHT / 2) - (wallHeight / 2);
            int drawEnd = (SCREEN_HEIGHT / 2) + (wallHeight / 2);

            int screenDrawStart = std::max(0, drawStart);
            int screenDrawEnd = std::min(SCREEN_HEIGHT - 1, drawEnd);

            // --- MAPEO HORIZONTAL (TEX_X) ---
            int blockX = (int)rayX % TILE_SIZE;
            int blockY = (int)rayY % TILE_SIZE;
            int texX = 0;

            if (std::abs(blockX - 0) < 2 || std::abs(blockX - 63) < 2) {
                texX = blockY;
            } else {
                texX = blockX;
            }
            texX = std::max(0, std::min(texX, TILE_SIZE - 1));

            // --- REBANADO VERTICAL (TEX_Y) ---
            sf::VertexArray wallSlice(sf::Points, screenDrawEnd - screenDrawStart);
            int vertexCounter = 0;

            for (int y = screenDrawStart; y < screenDrawEnd; y++) {
                // Relación flotante exacta para mapear la textura de techo a suelo sin cortes
                float texYRatio = (float)(y - drawStart) / (float)wallHeight;
                int texY = (int)(texYRatio * textureManager.getTextureSize());
                texY = std::max(0, std::min(texY, textureManager.getTextureSize() - 1));

                sf::Color pixelColor = textureManager.getPixelColor(wallType, texX, texY);

                // Sombreado dinámico por distancia (Faux-3D depth)
                if (correctedDistance > 150.0f) {
                    float shadow = 1.0f - (correctedDistance / 750.0f);
                    if (shadow < 0.15f) shadow = 0.15f;
                    pixelColor.r = (sf::Uint8)(pixelColor.r * shadow);
                    pixelColor.g = (sf::Uint8)(pixelColor.g * shadow);
                    pixelColor.b = (sf::Uint8)(pixelColor.b * shadow);
                }

                wallSlice[vertexCounter].position = sf::Vector2f((float)i, (float)y);
                wallSlice[vertexCounter].color = pixelColor;
                vertexCounter++;
            }

            window.draw(wallSlice);
        }
    }

    void render() {
        window.clear(sf::Color(40, 40, 40)); 
        
        renderRaycast();

        if (mapManager.showFullMap) {
            mapManager.drawFullMap(window);
        }

        window.display();
    }

public:
    Game() : 
        window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Wolfenstein 3D Engine - CETI"),
        player(sf::Vector2f(96.0f, 96.0f)),
        textureManager(64)
    {
        window.setFramerateLimit(60);
        player.pos = mapManager.getCurrentLevel()->spawnPoint;
        
        if (!textureManager.loadTextures("../assets/texturas/CASTLEBRICKS.png")) {
            std::cerr << "🛑 Alerta: No se pudo cargar ../assets/texturas/CASTLEBRICKS.png" << std::endl;
        } else {
            std::cout << "✅ Exito: Texturas cargadas correctamente." << std::endl;
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