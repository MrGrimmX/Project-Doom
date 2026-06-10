#include "Enemigo.h"

DoomEnemy::DoomEnemy(std::string name, int hp, float range, float speed, float painChance)
    : name(name), hp(hp), attackRange(range), speed(speed), painChance(painChance), 
      currentState(EnemyState::IDLE), position({0.0f, 0.0f}) {
    std::srand(std::time(0)); // Inicializar semilla aleatoria
}

void DoomEnemy::update(Vector2D playerPos, bool playerFired) {
    if (currentState == EnemyState::DEAD) return;

    float distanceToPlayer = position.distance(playerPos);

    switch (currentState) {
        case EnemyState::IDLE:
            // Percepción clásica: si el jugador dispara en la misma habitación 
            // O si entra en el rango de visión cercano, el enemigo se "despierta"
            if (playerFired || distanceToPlayer < 12.0f) {
                currentState = EnemyState::CHASE;
                std::cout << "📢 [" << name << "]: ¡GRRR! (Escuchó o vio al jugador. Alerta máxima).\n";
            } else {
                std::cout << "💤 [" << name << "] patrulla en estado pasivo.\n";
            }
            break;

        case EnemyState::CHASE:
            // Si está lo suficientemente cerca, ataca
            if (distanceToPlayer <= attackRange) {
                currentState = EnemyState::ATTACK;
            } else {
                // Si no, sigue caminando hacia él
                moveTowards(playerPos);
                std::cout << "🏃 [" << name << "] persiguiendo. Pos: (" << position.x << ", " << position.y 
                          << ") | Distancia: " << distanceToPlayer << "m\n";
            }
            break;

        case EnemyState::ATTACK:
            // En DOOM, el ataque detiene momentáneamente al enemigo
            std::cout << "🔥 [" << name << "] lanza un proyectil/zarpazo directo al jugador!\n";
            
            // Tras atacar, regresa inmediatamente a perseguir (el bucle agresivo de DOOM)
            currentState = EnemyState::CHASE; 
            break;

        case EnemyState::PAIN:
            // El estado de dolor dura un frame/instante en la lógica, interrumpiendo lo que hacía
            std::cout << "🥴 [" << name << "] se estremece por el impacto y pierde el turno de ataque!\n";
            currentState = EnemyState::CHASE; // Recupera la compostura
            break;

        case EnemyState::DEAD:
            break;
    }
}

void DoomEnemy::moveTowards(Vector2D targetPos) {
    // Calcular dirección
    float dx = targetPos.x - position.x;
    float dy = targetPos.y - position.y;
    float distance = std::sqrt(dx*dx + dy*dy);

    if (distance > 0) {
        // Movimiento básico normalizado hacia el jugador
        position.x += (dx / distance) * speed;
        position.y += (dy / distance) * speed;

        // El toque errático de DOOM: 25% de probabilidad de hacer un ligero zigzag lateral
        // Esto evita que se atoren perfectamente en esquinas y los hace menos predecibles
        if ((std::rand() % 100) < 25) {
            position.x += ((std::rand() % 3) - 1) * 0.5f; // Añade -0.5, 0 o 0.5 aleatorio
        }
    }
}

void DoomEnemy::takeDamage(int damage) {
    if (currentState == EnemyState::DEAD) return;

    hp -= damage;
    std::cout << "\n💥 ¡Impacto en el enemigo! Daño infligido: " << damage << " | HP restante: " << hp << "\n";

    if (hp <= 0) {
        currentState = EnemyState::DEAD;
        std::cout << "💀 [" << name << "]: Muere con un grito gutural y cae al suelo.\n\n";
    } else {
        // Simulación del "Pain Chance" original de DOOM
        float randomRoll = static_cast<float>(std::rand()) / RAND_MAX;
        if (randomRoll < painChance) {
            currentState = EnemyState::PAIN;
        }
    }
}