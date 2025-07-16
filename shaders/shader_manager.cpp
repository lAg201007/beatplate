#include "shader_manager.h"

namespace ShaderUtils {
    std::shared_ptr<sf::Shader> blurShaderPtr = nullptr;
    std::shared_ptr<sf::Shader> whiteMaskShaderPtr = nullptr;
    std::shared_ptr<sf::Shader> blackMaskShaderPtr = nullptr;

    void loadShaders() {
        if (!blurShaderPtr) {
            blurShaderPtr = std::make_shared<sf::Shader>();
            if (!blurShaderPtr->loadFromFile("shaders/frag/blur.frag", sf::Shader::Type::Fragment)) {
                std::cerr << "Erro ao carregar shader blur.frag" << std::endl;
                blurShaderPtr = nullptr;
            }
        }
        if (!whiteMaskShaderPtr) {
            whiteMaskShaderPtr = std::make_shared<sf::Shader>();
            if (!whiteMaskShaderPtr->loadFromFile("shaders/frag/white_flash.frag", sf::Shader::Type::Fragment)) {
                std::cerr << "Erro ao carregar shader white_flash.frag" << std::endl;
                whiteMaskShaderPtr = nullptr;
            }
        }
        if (!blackMaskShaderPtr) {
            blackMaskShaderPtr = std::make_shared<sf::Shader>();
            if (!blackMaskShaderPtr->loadFromFile("shaders/frag/dark_out.frag", sf::Shader::Type::Fragment)) {
                std::cerr << "Erro ao carregar shader dark_out.frag" << std::endl;
                blackMaskShaderPtr = nullptr;
            }
        }
    }

    ShaderCompound createVerticalBlurCompound(sf::RenderWindow& mWindow, sf::Sprite& sprite, float BlurStrength) {
        if (!blurShaderPtr) loadShaders();
        if (!blurShaderPtr) return {};
       
        const sf::Vector2u winSize = mWindow.getSize();

        sf::RenderTexture sceneRT({winSize.x, winSize.y});
        std::shared_ptr<sf::RenderTexture> blurRT = std::make_shared<sf::RenderTexture>(sf::Vector2u({winSize.x, winSize.y}));

        sceneRT.clear(sf::Color::Transparent);
        sceneRT.draw(sprite);
        sceneRT.display();

        sf::Sprite sceneSprite(sceneRT.getTexture());
        sf::Sprite blurSprite(blurRT->getTexture());

        blurShaderPtr->setUniform("image", sceneRT.getTexture());
        blurShaderPtr->setUniform("resolution", sf::Glsl::Vec2(winSize));
        blurShaderPtr->setUniform("blurStrength", BlurStrength);
        blurShaderPtr->setUniform("horizontal", true);

        blurRT->clear(sf::Color::Transparent);
        blurRT->draw(sceneSprite, blurShaderPtr.get());
        blurRT->display();

        blurShaderPtr->setUniform("image", blurRT->getTexture());
        blurShaderPtr->setUniform("horizontal", false);

        ShaderCompound compound;
        compound.renderSprite = blurSprite;
        compound.shader = blurShaderPtr;
        compound.textureHolder = blurRT;

        return compound;
    }

    ShaderCompound createWhiteMaskCompound(sf::RenderWindow& window, sf::Sprite& sprite, int WhiteIntensity) {
        if (!whiteMaskShaderPtr) loadShaders();
        if (!whiteMaskShaderPtr) return {};

        const sf::Vector2u winSize = window.getSize();
        std::shared_ptr<sf::RenderTexture> renderTexture = std::make_shared<sf::RenderTexture>(sf::Vector2u({winSize.x, winSize.y}));
        

        renderTexture->clear(sf::Color::Transparent);
        renderTexture->draw(sprite);
        renderTexture->display();

        sf::Sprite renderSprite(renderTexture->getTexture());

        whiteMaskShaderPtr->setUniform("WhiteMultiplier", static_cast<float>(WhiteIntensity));
        whiteMaskShaderPtr->setUniform("image", renderSprite.getTexture());
        whiteMaskShaderPtr->setUniform("resolution", sf::Glsl::Vec2(winSize));

        ShaderCompound compound;
        compound.renderSprite = renderSprite;
        compound.shader = whiteMaskShaderPtr;
        compound.textureHolder = renderTexture;

        return compound;
    }

    ShaderCompound createBlackOutCompound(sf::RenderWindow& window, sf::Sprite& sprite, int BlackIntensity) {
        if (!blackMaskShaderPtr) loadShaders();
        if (!blackMaskShaderPtr) return {};

        const sf::Vector2u winSize = window.getSize();
        std::shared_ptr<sf::RenderTexture> renderTexture = std::make_shared<sf::RenderTexture>(sf::Vector2u({winSize.x, winSize.y}));
        

        renderTexture->clear(sf::Color::Transparent);
        renderTexture->draw(sprite);
        renderTexture->display();

        sf::Sprite renderSprite(renderTexture->getTexture());

        blackMaskShaderPtr->setUniform("DarkMultiplier", static_cast<float>(BlackIntensity));
        blackMaskShaderPtr->setUniform("image", renderSprite.getTexture());
        blackMaskShaderPtr->setUniform("resolution", sf::Glsl::Vec2(winSize));

        ShaderCompound compound;
        compound.renderSprite = renderSprite;
        compound.shader = blackMaskShaderPtr;
        compound.textureHolder = renderTexture;

        return compound;
    }

    void drawShaderCompound(sf::RenderWindow& window, const ShaderCompound& compound) {
        if (!compound.shader || !compound.renderSprite.has_value()) return;
        window.draw(compound.renderSprite.value(), compound.shader.get());
    }

    void ShaderUtils::drawCompoundVector(sf::RenderWindow& window, const std::vector<ShaderCompound>& compounds) {
        if (compounds.empty()) return;
        const sf::Vector2u winSize = window.getSize();

        sf::RenderTexture tempTexture({winSize.x, winSize.y});
        
        sf::Sprite tempSprite(tempTexture.getTexture());

        tempTexture.clear(sf::Color::Transparent);
        tempTexture.draw(compounds[0].renderSprite.value(), compounds[0].shader.get());
        tempTexture.display();

        tempSprite.setTexture(tempTexture.getTexture());

        if (compounds.size() == 1) {
            window.draw(tempSprite);
            return;
        }

        for (int i = 1; i < compounds.size(); ++i) {
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