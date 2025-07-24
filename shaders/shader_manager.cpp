#include "shader_manager.h"
#include "../utils/SFML_CLASSES.h"
#include <SFML/Graphics.hpp>
#include <optional>
#include <unordered_map>
#include <vector>
#include <utility>
#include <iostream>
#include <variant>
#include <string>

uint32_t uniqueIdCounter = 0;

namespace ShaderUtils {
    std::unordered_map<std::string, std::shared_ptr<sf::Shader>> shaderInstanceCache;
    std::unordered_map<std::string, ShaderCompound> shaderCompoundCache;

    std::shared_ptr<sf::Shader> cacheShader(
    const std::string& shaderPath,
    std::unordered_map<std::string, std::shared_ptr<sf::Shader>>& shaderInstanceCache,
    bool useVertexShader = false,
    const std::string& vertexShaderPath = "shaders/vert/default.vert") {
        std::string cacheKey = shaderPath;

        if (useVertexShader) {
            // chave precisa diferenciar shaders com vertex default
            cacheKey = vertexShaderPath + shaderPath;
        }

        auto shaderIt = shaderInstanceCache.find(cacheKey);
        if (shaderIt != shaderInstanceCache.end()) {
            return shaderIt->second;
        }

        auto shader = std::make_shared<sf::Shader>();

        if (useVertexShader) {
            if (!shader->loadFromFile(vertexShaderPath, shaderPath)) {
                std::cerr << "Erro ao carregar vertex+fragment shaders em " << vertexShaderPath << " + " << shaderPath << "\n";
                return {};
            }
        } else {
            if (!shader->loadFromFile(shaderPath, sf::Shader::Type::Fragment)) {
                std::cerr << "Erro ao carregar fragment shader em " << shaderPath << "\n";
                return {};
            }
        }
 
        shaderInstanceCache[cacheKey] = shader;
        return shader;
    }

    ShaderCompound createGenericShaderCompound(
        sf::RenderWindow& window,
        sf::Sprite& sprite,
        const std::string& shaderPath,
        std::vector<std::pair<std::string, UniformValue>>& uniforms,
        const std::string& vertexShaderPath) 
    {
        const sf::Vector2u winSize = window.getSize();
        auto renderTexture = std::make_shared<sf::RenderTexture>(winSize);

        // Aqui usamos cacheShader adaptado para vertex + fragment (caso não tenha, substitua pela criação direta)
        std::shared_ptr<sf::Shader> shaderCopy;
        shaderCopy = cacheShader(shaderPath, shaderInstanceCache, true, vertexShaderPath);

        renderTexture->clear(sf::Color::Transparent);
        renderTexture->draw(sprite);
        renderTexture->display();

        sf::Sprite renderSprite(renderTexture->getTexture());

        uniforms.emplace_back("image", std::cref(renderTexture->getTexture())); // image = renderTexture

        for (auto& u : uniforms)
        {
            std::visit([&](auto&& v) {
                shaderCopy->setUniform(u.first, v);
            }, u.second);
        }

        ShaderCompound compound;
        compound.renderSprite = renderSprite;
        compound.shader = shaderCopy;
        compound.textureHolder = renderTexture;
        return compound;
    }

    // do not pass image uniform here, it needs to be passed using the render texture´s texture, not the sprite one
    // doing this will not crash anything, but it will make the sprite enormously big
    // this happens because the sprite uses the non scaled texture size, and the render texture transforms
    // that sprite into a texture with the window´s size, so the sprite will remain with his scaled size

    ShaderCompound createWhiteMaskCompound(sf::RenderWindow& window, sf::Sprite& sprite, int WhiteIntensity) {
        std::vector<std::pair<std::string, UniformValue>> uniforms;
        uniforms.reserve(3);
        uniforms.emplace_back("WhiteMultiplier", static_cast<float>(WhiteIntensity));
        sf::Vector2u winSize = window.getSize();
        sf::Vector2f resolution(winSize.x, winSize.y); 
        uniforms.emplace_back("resolution", UniformValue{resolution});
        ShaderCompound compound = createGenericShaderCompound(window, sprite, "shaders/frag/white_flash.frag", uniforms);
        return compound;
    }

    ShaderCompound createBlackOutCompound(sf::RenderWindow& window, sf::Sprite& sprite, int BlackIntensity) {
        std::vector<std::pair<std::string, UniformValue>> uniforms;
        uniforms.reserve(3);
        uniforms.emplace_back("DarkMultiplier", static_cast<float>(BlackIntensity));
        sf::Vector2u winSize = window.getSize();
        sf::Vector2f resolution(winSize.x, winSize.y); 
        uniforms.emplace_back("resolution", UniformValue{resolution});
        ShaderCompound compound = createGenericShaderCompound(window, sprite, "shaders/frag/dark_out.frag", uniforms);
        return compound;
    }

    // shaders mais complexos que precisam de uma função própia

    ShaderCompound createVerticalBlurCompound(sf::RenderWindow& mWindow, ShaderSprite& sprite, float BlurStrength) {
        // como essa é uma função especifica de shader, nós setamos as keys manualmente

        std::string compoundKey = std::to_string(sprite.uniqueId);
        compoundKey += "pos(" + std::to_string(sprite.getPosition().x) + "," + std::to_string(sprite.getPosition().y) + ")";
        compoundKey += "blurStrength" + std::to_string(BlurStrength);

        auto compoundIt = shaderCompoundCache.find(compoundKey);    
        if (compoundIt != shaderCompoundCache.end()) {
            return compoundIt->second; // Retorna o cache se já existir
        }

        const sf::Vector2u winSize = mWindow.getSize();
        sf::RenderTexture sceneRT(winSize);
        std::shared_ptr<sf::RenderTexture> blurRT = std::make_shared<sf::RenderTexture>(winSize);

        std::shared_ptr<sf::Shader> shaderCopy;
        shaderCopy = cacheShader("shaders/frag/blur.frag", shaderInstanceCache, false);

        sceneRT.clear(sf::Color::Transparent);
        sceneRT.draw(sprite);
        sceneRT.display();

        sf::Sprite sceneSprite(sceneRT.getTexture());
        sf::Sprite blurSprite(blurRT->getTexture());

        shaderCopy->setUniform("image", sceneRT.getTexture());
        shaderCopy->setUniform("resolution", sf::Glsl::Vec2(winSize));
        shaderCopy->setUniform("blurStrength", BlurStrength);
        shaderCopy->setUniform("horizontal", true);

        blurRT->clear(sf::Color::Transparent);
        blurRT->draw(sceneSprite, shaderCopy.get());
        blurRT->display();

        shaderCopy->setUniform("image", blurRT->getTexture());
        shaderCopy->setUniform("horizontal", false);

        ShaderCompound compound;
        compound.renderSprite = blurSprite;
        compound.shader = shaderCopy;
        compound.textureHolder = blurRT;

        shaderCompoundCache[compoundKey] = compound; 
        return compound;
    } 

    
    void drawShaderCompound(sf::RenderWindow& window, const ShaderCompound& compound) {
        if (!compound.shader || !compound.renderSprite.has_value()) return;
        window.draw(compound.renderSprite.value(), compound.shader.get());
    }

    void ShaderUtils::drawCompoundVector(sf::RenderWindow& window, const std::vector<ShaderCompound>& compounds) {
        if (compounds.empty()) return;
        const sf::Vector2u winSize = window.getSize();
        sf::RenderTexture tempTexture(winSize);
        sf::Sprite tempSprite(tempTexture.getTexture());

        tempTexture.clear(sf::Color::Transparent);
        tempTexture.draw(compounds[0].renderSprite.value(), compounds[0].shader.get());
        tempTexture.display();

        tempSprite.setTexture(tempTexture.getTexture());
        if (compounds.size() == 1) {
            window.draw(tempSprite);
            return;
        }

        for (size_t i = 1; i < compounds.size(); ++i) {
            sf::RenderStates states;
            states.shader = compounds[i].shader.get();
            states.blendMode = sf::BlendNone;

            tempTexture.clear(sf::Color::Transparent);
            tempTexture.draw(tempSprite, states);
            tempTexture.display();
            tempSprite.setTexture(tempTexture.getTexture());
        }

        window.draw(tempSprite);
    }
}
