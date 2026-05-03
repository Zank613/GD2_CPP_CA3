#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <SFML/Graphics.hpp>

#include <map>
#include <string>

/**
 * @brief Loads and stores SFML textures.
 */
class TextureManager {
    std::map<std::string, sf::Texture> textures;

public:
    /**
     * @brief Loads a texture from a file.
     * @param id Name used to retrieve the texture later.
     * @param filePath Path to the image file.
     * @return true if loading succeeded.
     */
    bool load(const std::string& id, const std::string& filePath);

    /**
     * @brief Gets a texture by id.
     * @param id Texture id.
     * @return Pointer to texture, or nullptr if it was not found.
     */
    const sf::Texture* get(const std::string& id) const;
};

#endif