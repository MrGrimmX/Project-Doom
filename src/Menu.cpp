#include "Menu.hpp"
#include <iostream>
#include <cmath>  // ← ESTA LÍNEA ES LA QUE FALTA (para sin, cos, etc.)

Menu::Menu() : selectedItem(0), currentState(MENU_MAIN) {
    // Cargar fuente (usa una fuente que tengas disponible)
    if (!font.loadFromFile("assets/fonts/Cybergame.ttf")) {
        // Fallback: intentar con otra ruta o crear una fuente por defecto
        if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            std::cerr << "⚠️ No se pudo cargar la fuente, usando fuente por defecto" << std::endl;
            // Crear una fuente básica por defecto (sin texto estilizado)
        }
    }
    
    // Configurar opciones del menú principal
    mainOptions = {"JUGAR", "OPCIONES", "SALIR"};
    
    // Configurar opciones del menú de opciones
    optionsList = {"VOLUMEN: 50%", "CONTROLES", "VOLVER"};
    
    // Configurar título
    title.setFont(font);
    title.setString("The Abyss of Doom");
    title.setCharacterSize(48);
    title.setFillColor(sf::Color::Yellow);
    title.setStyle(sf::Text::Bold);
    
    // Centrar título
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.left + titleBounds.width / 2.0f, 
                    titleBounds.top + titleBounds.height / 2.0f);
    
    // Configurar fondo
    background.setSize(sf::Vector2f(800, 600));
    background.setFillColor(sf::Color(20, 20, 40, 230)); // Semi-transparente
    
    // Configurar fondo del título
    titleBox.setSize(sf::Vector2f(400, 80));
    titleBox.setFillColor(sf::Color(0, 0, 0, 180));
    titleBox.setOutlineColor(sf::Color::Yellow);
    titleBox.setOutlineThickness(2.0f);
    
    // Crear items del menú
    for (size_t i = 0; i < mainOptions.size(); i++) {        
        sf::Text text;
        text.setFont(font);
        text.setString(mainOptions[i]);
        text.setCharacterSize(32);
        text.setFillColor(sf::Color::White);
        
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setOrigin(textBounds.left + textBounds.width / 2.0f,
                      textBounds.top + textBounds.height / 2.0f);
        
        menuItems.push_back(text);
    }
    controlsText.setFont(font);
controlsText.setCharacterSize(24);
controlsText.setFillColor(sf::Color::White);

controlsText.setString(
    "CONTROLES:\n\n"
    "WASD - Mover\n"
    "Mouse / Flechas - Rotar\n"
    "Click Derecho - Disparo\n"
    "Space / Enter - Seleccionar\n"
    "ESC - Pausa / Salir\n"
);
}

Menu::~Menu() {}

void Menu::handleInput(const sf::Event& event, GameState& state)
{
    if (event.type == sf::Event::KeyPressed)
    {
        switch (event.key.code)
        {
            case sf::Keyboard::Up:
                if (canMove) {
                    moveUp();
                    canMove = false;
                }
                break;

            case sf::Keyboard::Down:
                if (canMove) {
                    moveDown();
                    canMove = false;
                }
                break;

            case sf::Keyboard::Return:
            case sf::Keyboard::Space:
                selectOption(state);
                break;

            case sf::Keyboard::Escape:
                if (currentState == GAME_RUNNING) {
                    currentState = GAME_PAUSED;
                    state = GAME_PAUSED;
                }
                else if (currentState == GAME_PAUSED) {
                    currentState = GAME_RUNNING;
                    state = GAME_RUNNING;
                }
                else {
                    state = EXIT_GAME;
                }
                break;
        }
    }

    if (event.type == sf::Event::KeyReleased)
    {
        if (event.key.code == sf::Keyboard::Up ||
            event.key.code == sf::Keyboard::Down)
        {
            canMove = true;
        }
    }
}

void Menu::update(float deltaTime) {
    // Animaciones del menú pueden ir aquí
    static float time = 0;
    time += deltaTime;
    
    // Efecto de parpadeo en el título (ahora sin sin para evitar errores)
    int alpha = 128 + (int)(127 * (1.0f + std::sin(time * 3)) / 2.0f);  // ← Usamos std::sin
    title.setFillColor(sf::Color(255, 255, 0, alpha));
}

void Menu::render(sf::RenderWindow& window) {
    // Dibujar fondo semi-transparente
    background.setPosition(0, 0);
    window.draw(background);
    
    // Dibujar caja del título
    titleBox.setPosition(window.getSize().x / 2.0f - 200, 60);
    window.draw(titleBox);
    
    // Dibujar título
    title.setPosition(window.getSize().x / 2.0f, 100);
    window.draw(title);

    std::vector<std::string>* currentOptions = nullptr;
if (showingControls)
{
    sf::FloatRect bounds = controlsText.getLocalBounds();

    controlsText.setOrigin(
        bounds.left + bounds.width / 2.0f,
        bounds.top + bounds.height / 2.0f
    );

    controlsText.setPosition(
        window.getSize().x / 2.0f,
        window.getSize().y / 2.0f
    );

    window.draw(controlsText);

    return; // 🔴 ESTO ES LO QUE TE FALTA
}
    if (currentState == MENU_MAIN) {
        currentOptions = &mainOptions;
    } else if (currentState == MENU_OPTIONS) {
        currentOptions = &optionsList;
    } else if (currentState == GAME_PAUSED) {
        // Opciones para el menú de pausa
        pauseOptions = {"CONTINUAR", "MENU PRINCIPAL"};
        currentOptions = &pauseOptions;
        
        // Asegurar que hay suficientes items en menuItems para pausa
        if (menuItems.size() < pauseOptions.size()) {
            for (size_t i = menuItems.size(); i < pauseOptions.size(); i++) {
                sf::Text text;
                text.setFont(font);
                text.setString(pauseOptions[i]);
                text.setCharacterSize(32);
                text.setFillColor(sf::Color::White);
                sf::FloatRect textBounds = text.getLocalBounds();
                text.setOrigin(textBounds.left + textBounds.width / 2.0f,
                              textBounds.top + textBounds.height / 2.0f);
                menuItems.push_back(text);
            }
        }
    }
    
    if (currentOptions) {
        float startY = 200;
        float spacing = 60;
        
        for (size_t i = 0; i < currentOptions->size() && i < menuItems.size(); i++) {
            sf::FloatRect bounds = menuItems[i].getLocalBounds();

            menuItems[i].setOrigin(
                bounds.left + bounds.width / 2.0f,
                bounds.top + bounds.height / 2.0f
            );
            
            if (static_cast<int>(i) == selectedItem) {
                menuItems[i].setFillColor(sf::Color::Yellow);
                menuItems[i].setScale(1.1f, 1.1f);
            } else {
                menuItems[i].setFillColor(sf::Color::White);
                menuItems[i].setScale(1.0f, 1.0f);
            }
            
            menuItems[i].setPosition(window.getSize().x / 2.0f, startY + i * spacing);
            window.draw(menuItems[i]);
        }
    }
}

void Menu::moveUp() {
    int maxItems = 0;
if (currentState == MENU_MAIN) {
    maxItems = static_cast<int>(mainOptions.size());
}
else if (currentState == MENU_OPTIONS) {
    maxItems = static_cast<int>(optionsList.size());
}
else if (currentState == GAME_PAUSED) {
    maxItems = 2;
}
    
    selectedItem--;
    if (selectedItem < 0) selectedItem = maxItems - 1;
}

void Menu::moveDown() {
    int maxItems = 0;
    if (currentState == MENU_MAIN) {
        maxItems = mainOptions.size();
    } else if (currentState == MENU_OPTIONS) {
        maxItems = optionsList.size();
    } else if (currentState == GAME_PAUSED) {
        maxItems = 2;
    }
    
    selectedItem++;
    if (selectedItem >= maxItems) selectedItem = 0;
}

void Menu::selectOption(GameState& state) {
    if (currentState == MENU_MAIN) {
        switch (selectedItem) {
            case 0: // JUGAR
                state = GAME_RUNNING;
                currentState = GAME_RUNNING;
                selectedItem = 0;
                break;
            case 1: // OPCIONES
                currentState = MENU_OPTIONS;
                selectedItem = 0;
                break;
            case 2: // SALIR
                state = EXIT_GAME;
                break;
        }
    } else if (currentState == MENU_OPTIONS) {
    switch (selectedItem) {
        case 0: // VOLUMEN
            std::cout << "Ajustar volumen" << std::endl;
            break;

        case 1: // CONTROLES
            showingControls = true;
            break;

        case 2: // VOLVER
            currentState = MENU_MAIN;
            selectedItem = 0;
            showingControls = false;
            break;
        }
    } else if (currentState == GAME_PAUSED) {
        switch (selectedItem) {
            case 0: // CONTINUAR
                state = GAME_RUNNING;
                currentState = GAME_RUNNING;
                selectedItem = 0;
                break;
            case 1: // MENU PRINCIPAL
                state = MENU_MAIN;
                currentState = MENU_MAIN;
                selectedItem = 0;
                break;
        }
    }
}