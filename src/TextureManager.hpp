#ifndef TEXTURE_MANAGER_HPP
#define TEXTURE_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class TextureManager {
private:
    sf::Image textureSheet; // Usamos sf::Image porque necesitamos acceso directo a los píxeles (RGBA)
    int textureSize;        // Tamaño de cada textura cuadrada (ej: 64x64)

public:
    TextureManager(int size = 64);
    
    // Carga la imagen del mapa de texturas desde un archivo externo
    bool loadTextures(const std::string& filename);
    
    // Obtiene el color de un píxel específico de una textura dada una columna (texX) y una fila (texY)
    sf::Color getPixelColor(int textureID, int texX, int texY);
    
    int getTextureSize() const { return textureSize; }
};

#endif