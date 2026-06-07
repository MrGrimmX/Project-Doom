#ifndef DOOM_ENEMY_H
#define DOOM_ENEMY_H

#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>

// Estructura simple para manejar posiciones en el mapa
struct Vector2D {
    float x;
    float y;

    float distance(const Vector2D& other) const {
        return std::sqrt((other.x - x) * (other.x - x) + (other.y - y) * (other.y - y));
    }
};

// Los 5 estados mecánicos del Doom clásico
enum class EnemyState {
    IDLE,       // Esperando/Patrullando
    CHASE,      // Persiguiendo al jugador
    ATTACK,     // Animación y ejecución del ataque
    PAIN,       // Interrupción por recibir daño (Stun)
    DEAD        // Desactivado/Animación de muerte
};

class DoomEnemy {
private:
    std::string name;
    int hp;
    float attackRange;
    float speed;
    float painChance; // Probabilidad de interrumpir ataque (0.0 a 1.0)
    
    EnemyState currentState;
    Vector2D position;

    void moveTowards(Vector2D targetPos);

public:
    DoomEnemy(std::string name, int hp, float range, float speed, float painChance);
    
    void update(Vector2D playerPos, bool playerFired);
    void takeDamage(int damage);
    
    // Getters auxiliares
    bool isDead() const { return currentState == EnemyState::DEAD; }
    std::string getStateString() const;
};

#endif