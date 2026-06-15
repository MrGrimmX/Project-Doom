#include "Game.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int TILE_SIZE = 64;

Game::Game() : 
    window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Wolfenstein 3D Engine - CETI"),
    player(sf::Vector2f(96.0f, 96.0f)),
    textureManager(32),
    gameState(MENU_MAIN)
{
    window.setFramerateLimit(60);
    player.pos = mapManager.getCurrentLevel()->spawnPoint;
    window.setMouseCursorVisible(false);
    window.setKeyRepeatEnabled(false);
    
    if (!textureManager.loadTextures("assets/texturas/CASTLEBRICKS.png")) {
        std::cerr << "🛑 Alerta: No se pudo cargar assets/texturas/CASTLEBRICKS.png" << std::endl;
    } else {
        std::cout << "✅ Éxito: Base recuperada y texturas de 32x32 cargadas." << std::endl;
    }

    const float PI = 3.14159265f;

while(player.angle < 0)
    player.angle += 2.0f * PI;

while(player.angle >= 2.0f * PI)
    player.angle -= 2.0f * PI;
}

Game::~Game() {}

void Game::processEvents()
{
    sf::Event event;

    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            gameState = EXIT_GAME;

        if (gameState == MENU_MAIN || gameState == MENU_OPTIONS || gameState == GAME_PAUSED)
        {
            menu.handleInput(event, gameState);
        }

        if (gameState == GAME_RUNNING)
        {
            // TECLADO
            if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code)
                {
                    case sf::Keyboard::Num1:
                        player.switchWeapon(WeaponType::Pistol);
                        break;

                    case sf::Keyboard::Num2:
                        player.switchWeapon(WeaponType::AK);
                        break;

                    case sf::Keyboard::Num3:
                        player.switchWeapon(WeaponType::Shotgun);
                        break;

                    case sf::Keyboard::Num4:
                        player.switchWeapon(WeaponType::SKS);
                        break;

                    case sf::Keyboard::Num5:
                        player.switchWeapon(WeaponType::Uzi);
                        break;

                    case sf::Keyboard::Space:
                    {
                        float lookX = player.pos.x + std::cos(player.angle) * 45.0f;
                        float lookY = player.pos.y + std::sin(player.angle) * 45.0f;

                        int targetX = (int)(lookX) / TILE_SIZE;
                        int targetY = (int)(lookY) / TILE_SIZE;

                        if (mapManager.isDoor(targetX, targetY))
                            mapManager.openDoor(targetX, targetY);

                        else if (mapManager.isSecret(targetX, targetY))
                            mapManager.triggerSecret(targetX, targetY, player.angle);

                        break;
                    }

                    case sf::Keyboard::Tab:
                        mapManager.showFullMap = !mapManager.showFullMap;
                        break;

                    case sf::Keyboard::Escape:
                        gameState = GAME_PAUSED;
                        menu.setState(GAME_PAUSED);
                        break;

                    default:
                        break;
                }
            }

            // MOUSE (FUERA DEL SWITCH)
Weapon* weapon = player.getCurrentWeapon();

bool mouse = sf::Mouse::isButtonPressed(sf::Mouse::Left);

if (weapon->automatic)
{
    if (mouse)
    {
        if (player.consumeAmmo())
        {
            weapon->shoot();
        }
    }
}
else
{
    static bool lastMouse = false;

    if (mouse && !lastMouse)
    {
        if (player.consumeAmmo())
        {
            weapon->shoot();
        }
    }

    lastMouse = mouse;
}
        } // GAME_RUNNING
    } // while
} // processEvents

void Game::update(float deltaTime) {
    if (gameState == GAME_RUNNING) {
    player.update(deltaTime, mapManager);
        player.update(deltaTime, mapManager);
        player.getCurrentWeapon()->update(deltaTime);
        sf::Vector2i center(
    window.getSize().x / 2,
    window.getSize().y / 2
);

sf::Vector2i mousePos =
    sf::Mouse::getPosition(window);

int deltaX = mousePos.x - center.x;

player.angle += deltaX * 0.003f;

sf::Mouse::setPosition(center, window);
        mapManager.updateSecret(deltaTime);
        mapManager.updateDoors(deltaTime, player.pos);
    } else if (gameState == MENU_MAIN || gameState == MENU_OPTIONS || gameState == GAME_PAUSED) {
        menu.update(deltaTime);
    }
}

void Game::renderRaycast() {
    float fov = 60.0f * (3.14159265f / 180.0f);
    float halfFov = fov / 2.0f;
    int numRays = SCREEN_WIDTH;
    float deltaAngle = fov / (float)numRays;
    
    Level* curLevel = mapManager.getCurrentLevel();
    
    for (int i = 0; i < numRays; i++) {
        float rayAngle = (player.angle - halfFov) + (float)i * deltaAngle;
        
        while (rayAngle < 0) rayAngle += 2.0f * 3.14159265f;
        while (rayAngle >= 2.0f * 3.14159265f) rayAngle -= 2.0f * 3.14159265f;
        
        float cosRay = std::cos(rayAngle);
        float sinRay = std::sin(rayAngle);
        
        float distance = 0.0f;
        float stepSize = 0.5f;
        bool hitWall = false;
        int wallType = 1;
        int side = 0;
        
        float rayX = player.pos.x;
        float rayY = player.pos.y;
        
        while (!hitWall && distance < 800.0f) {
            distance += stepSize;
            rayX = player.pos.x + cosRay * distance;
            rayY = player.pos.y + sinRay * distance;
            
            int testX = (int)(rayX) / TILE_SIZE;
            int testY = (int)(rayY) / TILE_SIZE;
            
            // Detección de muro secreto activo
            if (mapManager.activeSecret.isActive) {
                float wallMinX = (float)mapManager.activeSecret.originalX * TILE_SIZE + mapManager.activeSecret.moveX;
                float wallMaxX = wallMinX + TILE_SIZE;
                float wallMinY = (float)mapManager.activeSecret.originalY * TILE_SIZE + mapManager.activeSecret.moveY;
                float wallMaxY = wallMinY + TILE_SIZE;
                
                if (rayX >= wallMinX && rayX < wallMaxX && rayY >= wallMinY && rayY < wallMaxY) {
                    wallType = 3;
                    hitWall = true;
                    
                    float hitX = rayX - wallMinX;
                    float hitY = rayY - wallMinY;
                    
                    if (std::min(hitX, (float)TILE_SIZE - hitX) < std::min(hitY, (float)TILE_SIZE - hitY)) {
                        side = 0;
                    } else {
                        side = 1;
                    }
                    break;
                }
            }
            
            // Detección de muros estáticos
            if (testX >= 0 && testX < curLevel->cols && testY >= 0 && testY < curLevel->rows) {
                int type = curLevel->grid[testY][testX].type;
                
                bool isStaticObstacle = (type == 1 || type == 9 || (type == 2 && !curLevel->grid[testY][testX].isOpen));
                bool isSecretAtRest = (type == 3 && !mapManager.activeSecret.isActive);
                
                if (isStaticObstacle || isSecretAtRest) {
                    wallType = type;
                    hitWall = true;
                    
                    float hitX = rayX - (testX * TILE_SIZE);
                    float hitY = rayY - (testY * TILE_SIZE);
                    
                    if (std::min(hitX, (float)TILE_SIZE - hitX) < std::min(hitY, (float)TILE_SIZE - hitY)) {
                        side = 0;
                    } else {
                        side = 1;
                    }
                }
            } else {
                hitWall = true;
            }
        }
        
        float correctedDistance = distance * std::cos(rayAngle - player.angle);
        if (correctedDistance < 1.0f) correctedDistance = 1.0f;
        
        int wallHeight = (int)((TILE_SIZE * SCREEN_HEIGHT) / correctedDistance);
        int drawStart = (SCREEN_HEIGHT / 2) - (wallHeight / 2);
        int drawEnd = (SCREEN_HEIGHT / 2) + (wallHeight / 2);
        
        int screenDrawStart = std::max(0, drawStart);
        int screenDrawEnd = std::min(SCREEN_HEIGHT - 1, drawEnd);
        
        int blockX = (int)rayX % TILE_SIZE;
        int blockY = (int)rayY % TILE_SIZE;
        int wallOffset = (side == 0) ? blockY : blockX;
        
        float scaleFactor = (float)textureManager.getTextureSize() / (float)TILE_SIZE;
        int texX = (int)((float)wallOffset * scaleFactor);
        
        if ((side == 0 && cosRay > 0) || (side == 1 && sinRay < 0)) {
            texX = textureManager.getTextureSize() - 1 - texX;
        }
        texX = std::max(0, std::min(texX, textureManager.getTextureSize() - 1));
        
        sf::VertexArray wallSlice(sf::Points, screenDrawEnd - screenDrawStart);
        int vertexCounter = 0;
        
        for (int y = screenDrawStart; y < screenDrawEnd; y++) {
            float texYRatio = (float)(y - drawStart) / (float)wallHeight;
            int texY = (int)(texYRatio * (float)textureManager.getTextureSize());
            texY = std::max(0, std::min(texY, textureManager.getTextureSize() - 1));
            
            int textureToRender = (wallType == 3) ? 1 : wallType;
            sf::Color pixelColor = textureManager.getPixelColor(textureToRender, texX, texY);
            
            float shadow = 1.0f;
            if (correctedDistance > 150.0f) {
                shadow = 1.0f - (correctedDistance / 750.0f);
                if (shadow < 0.15f) shadow = 0.15f;
            }
            if (side == 1) {
                shadow *= 0.7f;
            }
            
            pixelColor.r = (sf::Uint8)((float)pixelColor.r * shadow);
            pixelColor.g = (sf::Uint8)((float)pixelColor.g * shadow);
            pixelColor.b = (sf::Uint8)((float)pixelColor.b * shadow);
            
            wallSlice[vertexCounter].position = sf::Vector2f((float)i, (float)y);
            wallSlice[vertexCounter].color = pixelColor;
            vertexCounter++;
        }
        window.draw(wallSlice);
    }
}

void Game::render() {
    if (gameState == GAME_RUNNING || gameState == GAME_PAUSED) {
        // Renderizar el juego
        window.clear(sf::Color(45, 45, 50));
        
        sf::RectangleShape floorShape(sf::Vector2f((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT / 2.0f));
        floorShape.setPosition(0.0f, (float)SCREEN_HEIGHT / 2.0f);
        floorShape.setFillColor(sf::Color(145, 145, 145));
        window.draw(floorShape);
        Weapon* weapon = player.getCurrentWeapon();
weapon->sprite.setPosition(SCREEN_WIDTH / 2.f - weapon->frameWidth / 2.f + 80.f, SCREEN_HEIGHT - weapon->frameHeight - 50.f);

window.draw(weapon->sprite);
        
        renderRaycast();
        renderHUD();

        if (mapManager.showFullMap) {
            mapManager.drawFullMap(window, player.getPosition());        
        }
        
        // Si está en pausa, dibujar overlay del menú
        if (gameState == GAME_PAUSED) {
            menu.render(window);
        }
    } else {
        // Renderizar solo el menú
        window.clear(sf::Color(0, 0, 0));
        menu.render(window);
    }
    
    window.display();
}

void Game::renderHUD()
{
    const int HUD_HEIGHT = 100;

    hudFont.loadFromFile("assets/Fonts/Gamer.ttf");

    sf::RectangleShape hudBg(
        sf::Vector2f(SCREEN_WIDTH, HUD_HEIGHT));

    hudBg.setPosition(
        0,
        SCREEN_HEIGHT - HUD_HEIGHT);

    hudBg.setFillColor(
        sf::Color(70, 70, 70));

    window.draw(hudBg);

    sf::Text healthText;
    healthText.setFont(hudFont);
    healthText.setCharacterSize(24);
    healthText.setFillColor(sf::Color::White);

    healthText.setString(
        "VIDA: " +
        std::to_string(player.health));

    healthText.setPosition(
        20,
        SCREEN_HEIGHT - 90);

    window.draw(healthText);

    sf::Text ammoText;
    ammoText.setFont(hudFont);
    ammoText.setCharacterSize(24);
    ammoText.setFillColor(sf::Color::White);

    ammoText.setString(    "AMMO: " + std::to_string(player.getCurrentAmmo()));
    ammoText.setPosition(250, SCREEN_HEIGHT - 90);

window.draw(ammoText);
    sf::Text scoreText;
    scoreText.setFont(hudFont);
    scoreText.setCharacterSize(24);

    scoreText.setString(
        "SCORE: " +
        std::to_string(player.score));

    scoreText.setPosition(
        450,
        SCREEN_HEIGHT - 90);

    window.draw(scoreText);
}

void Game::run() {
    while (window.isOpen() && gameState != EXIT_GAME) {
        float deltaTime = gameClock.restart().asSeconds();
        processEvents();
        update(deltaTime);
        render();
    }
}