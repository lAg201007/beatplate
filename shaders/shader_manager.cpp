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

    void drawVerticalBlurSprite(sf::RenderWindow& mWindow, sf::Sprite sprite, float BlurStrength) {
        if (!blurShaderPtr) loadShaders();
        if (!blurShaderPtr) return;

        const sf::Vector2u winSize = mWindow.getSize();

        sf::RenderTexture sceneRT({winSize.x, winSize.y});
        sf::RenderTexture blurRT({winSize.x, winSize.y});

        sceneRT.clear(sf::Color::Transparent);
        sceneRT.draw(sprite);
        sceneRT.display();

        sf::Sprite sceneSprite(sceneRT.getTexture());
        sf::Sprite blurSprite(blurRT.getTexture());

        blurShaderPtr->setUniform("image", sceneRT.getTexture());
        blurShaderPtr->setUniform("resolution", sf::Glsl::Vec2(winSize));
        blurShaderPtr->setUniform("blurStrength", BlurStrength);
        blurShaderPtr->setUniform("horizontal", true);

        blurRT.clear(sf::Color::Transparent);
        blurRT.draw(sceneSprite, blurShaderPtr.get());
        blurRT.display();

        blurShaderPtr->setUniform("image", blurRT.getTexture());
        blurShaderPtr->setUniform("horizontal", false);

        mWindow.draw(blurSprite, blurShaderPtr.get());
    }

    void drawSpriteWithWhiteMaskShader(sf::RenderWindow& window, sf::Sprite sprite, int WhiteIntensity) {
        if (!whiteMaskShaderPtr) loadShaders();
        if (!whiteMaskShaderPtr) return;

        const sf::Vector2u winSize = window.getSize();
        sf::RenderTexture renderTexture({winSize.x, winSize.y});

        renderTexture.clear(sf::Color::Transparent);
        renderTexture.draw(sprite);
        renderTexture.display();

        sf::Sprite renderSprite(renderTexture.getTexture());

        whiteMaskShaderPtr->setUniform("WhiteMultiplier", static_cast<float>(WhiteIntensity));
        whiteMaskShaderPtr->setUniform("image", renderSprite.getTexture());
        whiteMaskShaderPtr->setUniform("resolution", sf::Glsl::Vec2(winSize));

        window.draw(renderSprite, whiteMaskShaderPtr.get());
    }
}