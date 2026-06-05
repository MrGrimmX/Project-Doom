#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <vector>

enum class EnemyState { IDLE, CHASE, PAIN, DEAD };
enum class GameState { PLAYING, GAME_OVER };

const int TILE_SIZE = 64; 
const int MAP_ROWS = 12;
const int MAP_COLS = 16;

int mapGrid[MAP_ROWS][MAP_COLS] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,0,1,0,1,1,1,1,1,0,0,1},
    {1,0,0,3,1,0,0,0,1,0,0,0,1,0,0,1}, 
    {1,0,1,1,1,0,1,0,1,0,0,0,1,0,0,1},
    {1,0,0,0,0,0,1,0,1,1,2,1,1,0,0,1}, 
    {1,1,1,0,1,1,1,0,1,0,0,0,1,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,1,1,1,1,1,1,0,0,1},
    {1,0,1,0,0,0,0,0,0,0,0,0,1,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

class DoomEnemy {
public:
    sf::Vector2f pos;
    sf::Vector2f startPos;
    EnemyState state;
    float speed = 90.0f; 
    float size = 40.0f;
    int hp = 100;
    sf::Clock painClock; 

    DoomEnemy(float x, float y) {
        startPos = sf::Vector2f(x, y);
        reset();
    }

    void reset() {
        pos = startPos;
        state = EnemyState::IDLE;
        hp = 100;
    }

    void takeDamage(int damage) {
        if (state == EnemyState::DEAD) return;
        hp -= damage;
        if (hp <= 0) state = EnemyState::DEAD;
        else {
            state = EnemyState::PAIN;
            painClock.restart(); 
        }
    }

    void update(sf::Vector2f playerPos, float deltaTime, int& playerHp) {
        if (state == EnemyState::DEAD) return;

        if (state == EnemyState::PAIN) {
            if (painClock.getElapsedTime().asSeconds() > 0.3f) state = EnemyState::CHASE;
            return; 
        }

        sf::Vector2f toPlayer = playerPos - pos;
        float distance = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);

        if (state == EnemyState::IDLE && distance < 400.0f) state = EnemyState::CHASE;

        if (state == EnemyState::CHASE) {
            if (distance <= 40.0f) { 
                playerHp -= 1; 
                if (playerHp < 0) playerHp = 0;
            } else {
                sf::Vector2f direction = toPlayer / distance;
                sf::Vector2f nextPos = pos + direction * speed * deltaTime;

                int cellX = nextPos.x / TILE_SIZE;
                int cellY = nextPos.y / TILE_SIZE;
                if (cellX >= 0 && cellX < MAP_COLS && cellY >= 0 && cellY < MAP_ROWS) {
                    if (mapGrid[cellY][cellX] == 0) {
                        pos = nextPos;
                    }
                }
            }
        }
    }
};

int main() {
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "DOOM 3D Completo - Primera Persona");
    window.setFramerateLimit(60);

    const sf::Vector2f spawnPoint(96.0f, 96.0f);
    sf::Vector2f playerPos = spawnPoint; 
    float playerAngle = 0.0f; 
    float playerSpeed = 160.0f;
    float rotationSpeed = 3.2f;
    int playerHp = 100;

    DoomEnemy enemy(650.0f, 250.0f);
    GameState currentState = GameState::PLAYING;

    const float FOV = 3.14159f / 3.0f; 
    const float HALF_FOV = FOV / 2.0f;
    const float MAX_DEPTH = 1200.0f;

    sf::Clock deltaClock;
    sf::Clock shootTimer;
    bool isShootingVisual = false; 
    sf::Clock flashClock;
    bool doorOpened = false;

    std::vector<float> zBuffer(SCREEN_WIDTH, MAX_DEPTH);

    while (window.isOpen()) {
        float deltaTime = deltaClock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::E) {
                if (currentState == GameState::PLAYING) {
                    int pCellX = playerPos.x / TILE_SIZE;
                    int pCellY = playerPos.y / TILE_SIZE;
                    for(int dy = -1; dy <= 1; dy++) {
                        for(int dx = -1; dx <= 1; dx++) {
                            int tx = pCellX + dx, ty = pCellY + dy;
                            if(tx >= 0 && tx < MAP_COLS && ty >= 0 && ty < MAP_ROWS && mapGrid[ty][tx] == 3) {
                                for(int r=0; r<MAP_ROWS; r++) {
                                    for(int c=0; c<MAP_COLS; c++) if(mapGrid[r][c] == 2) mapGrid[r][c] = 0;
                                }
                                doorOpened = true;
                                std::cout << "[!] INTERRUPTOR ACTIVADO EN 3D" << std::endl;
                            }
                        }
                    }
                } else if (currentState == GameState::GAME_OVER) {
                    playerPos = spawnPoint;
                    playerAngle = 0.0f;
                    playerHp = 100;
                    enemy.reset();
                    if (doorOpened) {
                        mapGrid[5][10] = 2; 
                        doorOpened = false;
                    }
                    currentState = GameState::PLAYING;
                }
            }
        }

        if (currentState == GameState::PLAYING) {
            if (playerHp <= 0) currentState = GameState::GAME_OVER;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) playerAngle -= rotationSpeed * deltaTime;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) playerAngle += rotationSpeed * deltaTime;

            sf::Vector2f moveDir(std::cos(playerAngle), std::sin(playerAngle));
            sf::Vector2f nextPos = playerPos;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) nextPos += moveDir * playerSpeed * deltaTime;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) nextPos -= moveDir * playerSpeed * deltaTime;

            int cellX = nextPos.x / TILE_SIZE;
            int cellY = nextPos.y / TILE_SIZE;
            if (cellX >= 0 && cellX < MAP_COLS && cellY >= 0 && cellY < MAP_ROWS) {
                if (mapGrid[cellY][cellX] != 1 && mapGrid[cellY][cellX] != 2) {
                    playerPos = nextPos;
                }
            }

            enemy.update(playerPos, deltaTime, playerHp);

            if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && shootTimer.getElapsedTime().asSeconds() > 0.25f) {
                isShootingVisual = true;
                flashClock.restart();
                shootTimer.restart();

                if (enemy.state != EnemyState::DEAD) {
                    sf::Vector2f toEnemy = enemy.pos - playerPos;
                    float angleToEnemy = std::atan2(toEnemy.y, toEnemy.x) - playerAngle;

                    if (angleToEnemy < -3.14159f) angleToEnemy += 2 * 3.14159f;
                    if (angleToEnemy > 3.14159f) angleToEnemy -= 2 * 3.14159f;

                    if (std::abs(angleToEnemy) < 0.12f) { 
                        enemy.takeDamage(34);
                    }
                }
            }

            if (isShootingVisual && flashClock.getElapsedTime().asSeconds() > 0.08f) {
                isShootingVisual = false;
            }
        }

        // --- RENDERIZADO EN 1RA PERSONA ---
        window.clear();

        sf::RectangleShape ceiling(sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT / 2));
        ceiling.setFillColor(sf::Color(35, 35, 35));
        window.draw(ceiling);
        sf::RectangleShape floor(sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT / 2));
        floor.setPosition(0, SCREEN_HEIGHT / 2);
        floor.setFillColor(sf::Color(55, 40, 35));
        window.draw(floor);

        for (int x = 0; x < SCREEN_WIDTH; x++) {
            float rayAngle = (playerAngle - HALF_FOV) + ((float)x / (float)SCREEN_WIDTH) * FOV;
            float distanceToWall = 0.0f;
            bool hitWall = false;
            int wallType = 0;

            float cosRay = std::cos(rayAngle);
            float sinRay = std::sin(rayAngle);

            while (!hitWall && distanceToWall < MAX_DEPTH) {
                distanceToWall += 3.0f;
                int testX = (int)(playerPos.x + cosRay * distanceToWall) / TILE_SIZE;
                int testY = (int)(playerPos.y + sinRay * distanceToWall) / TILE_SIZE;

                if (testX < 0 || testX >= MAP_COLS || testY < 0 || testY >= MAP_ROWS) {
                    hitWall = true;
                    distanceToWall = MAX_DEPTH;
                } else if (mapGrid[testY][testX] > 0) {
                    hitWall = true;
                    wallType = mapGrid[testY][testX]; 
                }
            }

            float correctedDistance = distanceToWall * std::cos(rayAngle - playerAngle);
            zBuffer[x] = correctedDistance; 

            int wallHeight = (int)(SCREEN_HEIGHT * TILE_SIZE / correctedDistance);
            if (wallHeight > SCREEN_HEIGHT) wallHeight = SCREEN_HEIGHT;
            int wallTop = (SCREEN_HEIGHT / 2) - (wallHeight / 2);

            sf::RectangleShape wallColumn(sf::Vector2f(1.0f, wallHeight));
            wallColumn.setPosition(x, wallTop);

            int shadow = 255 - (int)(correctedDistance * 0.35f);
            if (shadow < 25) shadow = 25;
            if (shadow > 255) shadow = 255;

            if (wallType == 1)      wallColumn.setFillColor(sf::Color(shadow, shadow, shadow)); 
            else if (wallType == 2) wallColumn.setFillColor(sf::Color(shadow * 0.6f, shadow * 0.3f, shadow * 0.1f)); 
            else if (wallType == 3) wallColumn.setFillColor(sf::Color(shadow, shadow * 0.75f, 0)); 

            window.draw(wallColumn);
        }

        if (enemy.state != EnemyState::DEAD) {
            sf::Vector2f toEnemy = enemy.pos - playerPos;
            float distanceToEnemy = std::sqrt(toEnemy.x * toEnemy.x + toEnemy.y * toEnemy.y);

            float enemyAngle = std::atan2(toEnemy.y, toEnemy.x) - playerAngle;
            if (enemyAngle < -3.14159f) enemyAngle += 2 * 3.14159f;
            if (enemyAngle > 3.14159f) enemyAngle -= 2 * 3.14159f;

            if (std::abs(enemyAngle) < HALF_FOV && distanceToEnemy > 15.0f) {
                int enemyScreenHeight = (int)(SCREEN_HEIGHT * TILE_SIZE / distanceToEnemy);
                int enemyScreenWidth = enemyScreenHeight; 

                int enemyScreenX = (int)((SCREEN_WIDTH / 2) + (enemyAngle / HALF_FOV) * (SCREEN_WIDTH / 2) - (enemyScreenWidth / 2));
                int enemyScreenY = (SCREEN_HEIGHT / 2) - (enemyScreenHeight / 2);

                for (int i = 0; i < enemyScreenWidth; i++) {
                    int colX = enemyScreenX + i;
                    if (colX >= 0 && colX < SCREEN_WIDTH && distanceToEnemy < zBuffer[colX]) {
                        sf::RectangleShape enemyCol(sf::Vector2f(1.0f, enemyScreenHeight));
                        enemyCol.setPosition(colX, enemyScreenY);

                        if (enemy.state == EnemyState::CHASE)      enemyCol.setFillColor(sf::Color(180, 0, 0)); 
                        else if (enemy.state == EnemyState::PAIN) enemyCol.setFillColor(sf::Color(0, 180, 200)); 
                        else                                       enemyCol.setFillColor(sf::Color(160, 160, 160)); 

                        window.draw(enemyCol);
                    }
                }
            }
        }

        sf::CircleShape crosshair(3.0f);
        crosshair.setOrigin(3.0f, 3.0f);
        crosshair.setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
        crosshair.setFillColor(sf::Color::Green);
        window.draw(crosshair);

        sf::RectangleShape weapon(sf::Vector2f(60.0f, 140.0f));
        weapon.setOrigin(30.0f, 140.0f);
        weapon.setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT);
        weapon.setFillColor(isShootingVisual ? sf::Color(240, 200, 50) : sf::Color(70, 75, 80)); 
        window.draw(weapon);

        sf::RectangleShape hudBack(sf::Vector2f(SCREEN_WIDTH, 45.0f));
        hudBack.setPosition(0, SCREEN_HEIGHT - 45.0f);
        hudBack.setFillColor(sf::Color(25, 25, 28));
        window.draw(hudBack);

        sf::RectangleShape hpBox(sf::Vector2f(180.0f, 20.0f));
        hpBox.setPosition(20.0f, SCREEN_HEIGHT - 32.0f);
        hpBox.setFillColor(sf::Color(10, 10, 10));
        window.draw(hpBox);

        if (playerHp > 0) {
            float hpWidth = (playerHp / 100.0f) * 180.0f;
            sf::RectangleShape hpFill(sf::Vector2f(hpWidth, 20.0f));
            hpFill.setPosition(20.0f, SCREEN_HEIGHT - 32.0f);
            hpFill.setFillColor(sf::Color(210, 25, 25)); 
            window.draw(hpFill);
        }

        if (currentState == GameState::GAME_OVER) {
            sf::RectangleShape bloodScreen(sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT));
            bloodScreen.setFillColor(sf::Color(255, 0, 0, 75)); 
            window.draw(bloodScreen);
        }

        window.display();
    }
    return 0;
}