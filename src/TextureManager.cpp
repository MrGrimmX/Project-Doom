#include "TextureManager.hpp"
#include <iostream>

TextureManager::TextureManager(int size) {
    textureSize = size;
}

bool TextureManager::loadTextures(const std::string& filename) {
    // Intentar cargar el archivo de imagen perimetral
    if (!textureSheet.loadFromFile(filename)) {
        std::cerr << "ERROR: No se pudo cargar el archivo de texturas: " << filename << std::endl;
        return false;
    }
    return true;
}

sf::Color TextureManager::getPixelColor(int textureID, int texX, int texY) {
    // Calcular el desplazamiento horizontal en el SpriteSheet basado en el ID de la textura
    // ID 1 = Muro (columnas 0-63), ID 2 = Puerta (columnas 64-127), ID 3 = Secreto (128-191), etc.
    int sheetX = ((textureID - 1) * textureSize) + texX;
    int sheetY = texY;

    // Asegurar protección de límites para evitar crasheos (Out of bounds)
    if (sheetX < 0 || sheetX >= (int)textureSheet.getSize().x || sheetY < 0 || sheetY >= (int)textureSheet.getSize().y) {
        return sf::Color::Magenta; // Color de error visible si algo sale mal
    }

    return textureSheet.getPixel(sheetX, sheetY);
}