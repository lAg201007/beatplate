#include "shader_manager.h"

namespace ShaderUtils {
    std::shared_ptr<sf::Shader> blurShaderPtr = nullptr;
    std::shared_ptr<sf::Shader> whiteMaskShaderPtr = nullptr;

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

    void drawShaderCompound(sf::RenderWindow& window, const ShaderCompound& compound) {
        if (!compound.shader || !compound.renderSprite.has_value()) return;
        window.draw(compound.renderSprite.value(), compound.shader.get());
    }

    void drawCompoundVector(sf::RenderWindow& window, const std::vector<ShaderCompound>& compounds) {
        if (compounds.empty()) return;

        sf::Vector2u winSize = window.getSize();
        sf::RenderTexture tempRT({winSize.x, winSize.y});

        tempRT.clear(sf::Color::Transparent);
        tempRT.draw(compounds[0].renderSprite.value(), compounds[0].shader.get());
        tempRT.display();

        for (size_t i = 1; i < compounds.size(); ++i) {
            sf::Sprite prevSprite(tempRT.getTexture());

            if (compounds[i].shader) {
                compounds[i].shader->setUniform("image", tempRT.getTexture());
            }

            tempRT.clear(sf::Color::Transparent);
            tempRT.draw(prevSprite, compounds[i].shader.get());
            tempRT.display();
        }

        sf::Sprite finalSprite(tempRT.getTexture());
        window.draw(finalSprite);
    }
}