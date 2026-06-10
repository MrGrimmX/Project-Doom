#ifndef MENU_HPP
#define MENU_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

enum GameState {
    MENU_MAIN,
    MENU_OPTIONS,
    GAME_RUNNING,
    GAME_PAUSED,
    EXIT_GAME
};

class Menu {
private:
    sf::Font font;
    std::vector<sf::Text> menuItems;
    int selectedItem;
    sf::RectangleShape background;
    sf::RectangleShape titleBox;
    sf::Text title;
    sf::Text controlsText;
    std::vector<std::string> mainOptions;
    std::vector<std::string> optionsList;
    std::vector<std::string> pauseOptions;
    GameState currentState;
    
public:
    Menu();~Menu();
    bool showingControls = false;
    bool canMove = true;
    void handleInput(const sf::Event& event, GameState& state);
    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    void moveUp();
    void moveDown();
    void selectOption(GameState& state);
    void setState(GameState state) { currentState = state; }
    GameState getState() const { return currentState; }
};

#endif