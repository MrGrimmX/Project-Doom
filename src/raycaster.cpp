#include "raycaster.hpp"

Raycaster::Raycaster() : window(
        sf::VideoMode(1280, 720), 
        "Raycaster",
        sf:: Style::Titlebar | sf::Style::Close
        ){

    player_x = player_y = {8.f};
    player_angle = {0.f};

    map = {
        "################",
        "#..............#",
        "#.......########",
        "#..............#",
        "#......##......#",
        "#......##......#",
        "#..............#",
        "###............#",
        "##.............#",
        "#......####..###",
        "#......#.......#",
        "#......#.......#",
        "#..............#",
        "#......#########",
        "#..............#",
        "################"
    };
}

void Raycaster::events(){
    sf::Event event;
    while(window.pollEvent(event)){
        if(event.type == sf::Event::Closed){
            window.close();
        }
    }
}

void Raycaster::draw(){
    window.clear();
    draw_minimap();
    rays();
    window.display();
}

void Raycaster::run(){
    while(window.isOpen()){
        events();
        keys();
        draw();
    }
}

void Raycaster::draw_minimap(){
    int minimapSize = window.getSize().y - 20;
    int cellSize = minimapSize / map[0].size();
    sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
    
    for(size_t y = 0; y < map.size(); ++y){
        for(size_t x = 0; x < map[y].size(); ++x){
            if(map[y][x] == '#'){
                cell.setFillColor(sf::Color::White);
            } else {
                cell.setFillColor(sf::Color(50, 50, 50));
            }
            cell.setPosition(10+ x * cellSize, 10 + y * cellSize);
            window.draw(cell);
        }
    }

    sf::CircleShape playerDot(cellSize / 3.f);
    playerDot.setFillColor(sf::Color::Red);
    playerDot.setPosition(
        10 + player_x * cellSize - playerDot.getRadius(), 
        10 + player_y * cellSize - playerDot.getRadius()
    );
    window.draw(playerDot);

    sf::Vertex line[] = {

        sf::Vertex(
            sf::Vector2f(
            10 + player_x * cellSize, 
            10 + player_y * cellSize
        ),
            sf::Color::Red
        ),

        sf::Vertex(
            sf::Vector2f(
            10 + (player_x + std::cos(player_angle) * 1.5f) * cellSize, 
            10 + (player_y + std::sin(player_angle) * 1.5f) * cellSize
        ),
            sf::Color::Red
        ),

    };
    window.draw(line, 2, sf::Lines);
}

void Raycaster::keys(){
    float timer = clock.restart().asSeconds();

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
        player_angle -= 1.5f * timer;
    }
        
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
        player_angle += 1.5f * timer;
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
        player_x += std::cos(player_angle) * 5.f * timer;
        player_y += std::sin(player_angle) * 5.f * timer;

        if(map[int(player_y)][int(player_x)] == '#'){
            player_x -= std::cos(player_angle) * 5.f * timer;
            player_y -= std::sin(player_angle) * 5.f * timer;
        }
    }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
        player_x -= std::cos(player_angle) * 5.f * timer;
        player_y -= std::sin(player_angle) * 5.f * timer;

        if(map[int(player_y)][int(player_x)] == '#'){
            player_x += std::cos(player_angle) * 5.f * timer;
            player_y += std::sin(player_angle) * 5.f * timer;
        }
    }
}

void Raycaster::rays(){
    int cellSize = (window.getSize().y - 20) / map[0].size();

    int numRays = 3;
    float spread = 0.785f;

    for(int i = 0; i < numRays; ++i){
        float offset = ((i / (float)(numRays - 1)) - 0.5f) * spread * 2.0f;
        float rayAngle = player_angle + offset;

        float rayX = player_x, rayY = player_y;

    while(map[(int)rayY][(int)rayX] != '#'){
        rayX += std::cos(rayAngle) * 0.1f;
        rayY += std::sin(rayAngle) * 0.1f;

        sf::Vertex line[] = {
            sf::Vertex(
                sf::Vector2f(
                    10 + player_x * cellSize, 
                    10 + player_y * cellSize
                ),
                sf::Color::Yellow
            ),
            sf::Vertex(
                sf::Vector2f(
                    10 + rayX * cellSize, 
                    10 + rayY * cellSize
                ),
                sf::Color::Yellow
            )
        };
    }
}