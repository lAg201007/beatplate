#include "../utils/texture_caching.h"

std::unordered_map<std::string, sf::Texture> TextureCache;

sf::Texture& LoadTexture(const std::string& path) {
    // Se já existe no cache, retorna a existente
    auto it = TextureCache.find(path);
    if (it != TextureCache.end()) {
        return it->second;
    }

    // Carregar do disco
    sf::Texture texture;
    if (!texture.loadFromFile(path)) {
        throw std::runtime_error("Erro ao carregar textura: " + path);
    }

    // Inserir no cache e devolver a referência
    auto& stored = TextureCache[path] = std::move(texture);
    return stored;
}
