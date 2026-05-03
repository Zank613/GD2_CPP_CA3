#include "TextureManager.h"

bool TextureManager::load(const std::string& id, const std::string& filePath) {
    sf::Texture texture;

    if (!texture.loadFromFile(filePath)) {
        return false;
    }

    textures[id] = texture;
    return true;
}

const sf::Texture* TextureManager::get(const std::string& id) const {
    auto it = textures.find(id);

    if (it == textures.end()) {
        return nullptr;
    }

    return &it->second;
}