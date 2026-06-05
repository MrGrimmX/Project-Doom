#include <SFML/Graphics.hpp>
#include <cmath>
#include "Map.hpp"
#include "Player.hpp"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const float PI = 3.14159265f;
const int TILE_SIZE = 64;

class Game {
private:
    sf::RenderWindow window;
    MapManager mapManager;
    Player player;
    sf::Clock gameClock;

public:
    Game() : 
        window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Doom Height Engine Fijo"),
        player(sf::Vector2f(96.0f, 96.0f)) 
    {
        window.setFramerateLimit(60);
    }

    void run() {
        while (window.isOpen()) {
            float deltaTime = gameClock.restart().asSeconds();
            processEvents();
            update(deltaTime);
            render();
        }
    }

private:
    void processEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Tab) {
                    mapManager.showFullMap = !mapManager.showFullMap;
                }
                if (event.key.code == sf::Keyboard::Space) {
                    float lookX = player.pos.x + std::cos(player.angle) * 45.0f;
                    float lookY = player.pos.y + std::sin(player.angle) * 45.0f;
                    int targetX = (int)(lookX) / TILE_SIZE;
                    int targetY = (int)(lookY) / TILE_SIZE;
                    if (mapManager.isDoor(targetX, targetY)) {
                        mapManager.openDoor(targetX, targetY);
                    }
                }
            }
        }
    }

    void update(float deltaTime) {
        player.update(deltaTime, mapManager);
        mapManager.updateElevator(deltaTime);
    }

    void renderRaycast() {
        float fov = 60.0f * (PI / 180.0f); 
        int numRays = SCREEN_WIDTH;
        float angleStep = fov / (float)numRays;
        float startAngle = player.angle - (fov / 2.0f);

        Level* curLevel = mapManager.getCurrentLevel();
        int playerCellX = (int)(player.pos.x) / TILE_SIZE;
        int playerCellY = (int)(player.pos.y) / TILE_SIZE;
        
        float playerFloor = 0.0f;
        if(playerCellX >= 0 && playerCellX < curLevel->cols && playerCellY >= 0 && playerCellY < curLevel->rows) {
            playerFloor = curLevel->grid[playerCellY][playerCellX].floorHeight;
        }

        for (int i = 0; i < numRays; i++) {
            float rayAngle = startAngle + (float)i * angleStep;
            float distance = 0.0f;
            float maxDistance = 700.0f;
            float stepSize = 1.5f; 

            float cosRay = std::cos(rayAngle);
            float sinRay = std::sin(rayAngle);
            
            bool rayFinished = false;
            float currentHighestEdge = playerFloor; 

            while (!rayFinished && distance < maxDistance) {
                distance += stepSize;
                float rayX = player.pos.x + cosRay * distance;
                float rayY = player.pos.y + sinRay * distance;

                int testX = (int)(rayX) / TILE_SIZE;
                int testY = (int)(rayY) / TILE_SIZE;

                if (testX >= 0 && testX < curLevel->cols && testY >= 0 && testY < curLevel->rows) {
                    Tile targetTile = curLevel->grid[testY][testX];
                    
                    if (targetTile.type == 1) {
                        drawColumnSegment(i, distance, rayAngle, currentHighestEdge, 1.5f, sf::Color(90, 95, 100));
                        rayFinished = true;
                    }
                    else if (targetTile.type == 2) { // Renderizar Puertas
                        drawColumnSegment(i, distance, rayAngle, currentHighestEdge, 1.0f, sf::Color(130, 40, 130));
                        rayFinished = true;
                    }
                    else if (targetTile.floorHeight > currentHighestEdge) {
                        sf::Color stepColor = targetTile.isElevator ? sf::Color(190, 110, 30) : sf::Color(50, 100, 160);
                        drawColumnSegment(i, distance, rayAngle, currentHighestEdge, targetTile.floorHeight, stepColor);
                        currentHighestEdge = targetTile.floorHeight;
                    }
                } else {
                    rayFinished = true; 
                }
            }
        }
    }

    void drawColumnSegment(int screenX, float distance, float rayAngle, float currentFloor, float targetFloor, sf::Color color) {
        float correctedDistance = distance * std::cos(rayAngle - player.angle);
        if (correctedDistance < 1.0f) correctedDistance = 1.0f;

        int baseWallHeight = (int)(SCREEN_HEIGHT * TILE_SIZE / correctedDistance);

        int horizonOffset = (int)(player.cameraZ / correctedDistance);
        int finalTop = (SCREEN_HEIGHT / 2) - (baseWallHeight / 2) - horizonOffset + (int)((1.0f - targetFloor) * baseWallHeight);
        int finalBottom = (SCREEN_HEIGHT / 2) + (baseWallHeight / 2) - horizonOffset + (int)((1.0f - currentFloor) * baseWallHeight);

        float shadow = 1.0f - (distance / 700.0f);
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

    void render() {
        window.clear(sf::Color(15, 15, 15)); 

        // Suelo general estático de fondo
        sf::RectangleShape floorRect(sf::Vector2f((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT / 2.0f));
        floorRect.setPosition(0.0f, (float)SCREEN_HEIGHT / 2.0f);
        floorRect.setFillColor(sf::Color(30, 30, 30)); 
        window.draw(floorRect);

        if (mapManager.showFullMap) {
            mapManager.drawFullMap(window);
            player.drawOnFullMap(window, mapManager);
        } else {
            renderRaycast();
        }

        window.display();
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}