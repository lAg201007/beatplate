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
    std::unordered_map<uint32_t, ShaderCompound> shaderCompoundCache;

    std::shared_ptr<sf::Shader> cacheShader(const std::string& shaderInstanceKey, std::string shaderPath,
    std::unordered_map<std::string, std::shared_ptr<sf::Shader>>& shaderInstanceCache) {
        std::shared_ptr<sf::Shader> shader;
        auto shaderIt = shaderInstanceCache.find(shaderInstanceKey);
        if (shaderIt != shaderInstanceCache.end()) {
            shader = shaderIt->second;
        } else {
            // carrega o shader e coloca no cache se n tiver la
            shader = std::make_shared<sf::Shader>();
            if (!shader->loadFromFile(shaderPath, sf::Shader::Type::Fragment)) {
                std::cerr << "Erro ao carregar shader em " << shaderPath << "\n";
                return {};
            }
            shaderInstanceCache[shaderInstanceKey] = shader;
        }
        return shader;
    }

    // função para shaders genericos de uma passada só
    ShaderCompound createGenericShaderCompound(sf::RenderWindow& window, 
        ShaderSprite& sprite, 
        std::string shaderPath, 
        std::vector<std::pair<std::string, UniformValue>>& uniforms)
    {
        std::string shaderInstanceKey;

        auto compoundIt = shaderCompoundCache.find(sprite.uniqueId);    
        if (compoundIt != shaderCompoundCache.end()) {
            return compoundIt->second; // Retorna o cache se já existir
        }

        // uniforms podem ser apenas tipos primitivos
        // loopa pelos uniforms e cria uma chave única
        for (const auto& uniform : uniforms) {
            shaderInstanceKey += uniform.first;
            std::visit([&](auto&& v) {
                using T = std::decay_t<decltype(v)>;

                if constexpr (std::is_same_v<T, int>) {
                    shaderInstanceKey += std::to_string(v);
                }
                else if constexpr (std::is_same_v<T, float>) {
                    shaderInstanceKey += std::to_string(v);
                }
                else if constexpr (std::is_same_v<T, bool>) {
                    shaderInstanceKey += v ? "true" : "false";
                }
                else if constexpr (std::is_same_v<T, sf::Vector2f>) {
                    shaderInstanceKey += "(" + std::to_string(v.x) + "," + std::to_string(v.y) + ")";
                }
                else if constexpr (std::is_same_v<T,  std::reference_wrapper<const sf::Texture>>) {
                    // usa o handle único da textura para a chave
                    shaderInstanceKey += std::to_string(v.get().getNativeHandle());
                }   
                else {
                    static_assert(always_false<T>::value,
                        "Tipo de UniformValue não tratado no key generator");
                }
            }, uniform.second);
        }

        // pega winSize e cria a renderTexture
        const sf::Vector2u winSize = window.getSize();
        std::shared_ptr<sf::RenderTexture> renderTexture = std::make_shared<sf::RenderTexture>(winSize);

        // procura o shader no cache
        std::shared_ptr<sf::Shader> shader;
        shader = cacheShader(shaderInstanceKey, shaderPath, shaderInstanceCache);

        renderTexture->clear(sf::Color::Transparent);
        renderTexture->draw(sprite);
        renderTexture->display();

        sf::Sprite renderSprite(renderTexture->getTexture());

        uniforms.emplace_back("image", std::cref(renderTexture->getTexture())); // passar image aqui, usando render texture, não sprite

        // set uniforms
        for (auto& u : uniforms)
        {
            std::visit([&](auto&& v) {

                shader->setUniform(u.first, v);

            }, u.second);
        }

        ShaderCompound compound;
        compound.renderSprite = renderSprite;
        compound.shader = shader;
        compound.textureHolder = renderTexture;

        shaderCompoundCache[sprite.uniqueId] = compound; 
        return compound;
    }

    // do not pass image uniform here, it needs to be passed using the render texture´s texture, not the sprite one
    // doing this will not crash anything, but it will make the sprite enormously big
    // this happens because the sprite uses the non scaled texture size, and the render texture transforms
    // that sprite into a texture with the window´s size, so the sprite will remain with his scaled size

    ShaderCompound createWhiteMaskCompound(sf::RenderWindow& window, ShaderSprite& sprite, int WhiteIntensity) {
        std::vector<std::pair<std::string, UniformValue>> uniforms;
        uniforms.reserve(3);
        uniforms.emplace_back("WhiteMultiplier", static_cast<float>(WhiteIntensity));
        sf::Vector2u winSize = window.getSize();
        sf::Vector2f resolution(winSize.x, winSize.y); 
        uniforms.emplace_back("resolution", UniformValue{resolution});
        ShaderCompound compound = createGenericShaderCompound(window, sprite, "shaders/frag/white_flash.frag", uniforms);
        return compound;
    }

    ShaderCompound createBlackOutCompound(sf::RenderWindow& window, ShaderSprite& sprite, int BlackIntensity) {
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
        std::string shaderKey = "verticalBlur" + std::to_string(BlurStrength);
        auto compoundIt = shaderCompoundCache.find(sprite.uniqueId);    
        if (compoundIt != shaderCompoundCache.end()) {
            return compoundIt->second; // Retorna o cache se já existir
        }

        const sf::Vector2u winSize = mWindow.getSize();
        sf::RenderTexture sceneRT(winSize);
        std::shared_ptr<sf::RenderTexture> blurRT = std::make_shared<sf::RenderTexture>(winSize);

        std::shared_ptr<sf::Shader> shaderCopy;
        shaderCopy = cacheShader(shaderKey, "shaders/frag/blur.frag", shaderInstanceCache);

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

        shaderCompoundCache[sprite.uniqueId] = compound; 
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