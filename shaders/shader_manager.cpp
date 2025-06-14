#include "shader_manager.h"

namespace ShaderUtils {
    void drawVerticalBlurSprite(sf::RenderWindow& mWindow, sf::Sprite sprite, float BlurStrength) {
        const sf::Vector2u winSize = mWindow.getSize();

        // Carregar o shader
        sf::Shader blurShader;
        if (!blurShader.loadFromFile("shaders/frag/blur.frag", sf::Shader::Type::Fragment)) {
            std::cerr << "Erro ao carregar shader blur.frag" << std::endl;
            return;
        }

        // Criar RenderTextures para passes
        sf::RenderTexture sceneRT({winSize.x, winSize.y});
        sf::RenderTexture blurRT({winSize.x, winSize.y});

        // Desenhar objeto original em sceneRT
        sceneRT.clear(sf::Color::Transparent);
        sceneRT.draw(sprite);
        sceneRT.display();

        // Configurar quad para desenhar fullscreen (usando Sprite facilita)
        sf::Sprite sceneSprite(sceneRT.getTexture());
        sf::Sprite blurSprite(blurRT.getTexture());

        // Passo 1: blur horizontal (sceneRT -> blurRT)
        blurShader.setUniform("image", sceneRT.getTexture());
        blurShader.setUniform("resolution", sf::Glsl::Vec2(winSize));
        blurShader.setUniform("blurStrength", BlurStrength);
        blurShader.setUniform("horizontal", true);

        blurRT.clear(sf::Color::Transparent);
        blurRT.draw(sceneSprite, &blurShader);
        blurRT.display();

        // Passo 2: blur vertical (blurRT -> janela)
        blurShader.setUniform("image", blurRT.getTexture());
        blurShader.setUniform("horizontal", false);

        mWindow.draw(blurSprite, &blurShader);
    }

    void drawSpriteWithWhiteMaskShader(sf::RenderWindow& window, sf::Sprite sprite, int WhiteIntensity) {
        sf::Shader whiteMaskShader;
        const sf::Vector2u winSize = window.getSize();
        if (!whiteMaskShader.loadFromFile("shaders/frag/white_flash.frag", sf::Shader::Type::Fragment)) {
            std::cerr << "Erro ao carregar shader white_flash.frag" << std::endl;
            return;
        }

        sf::RenderTexture renderTexture({winSize.x, winSize.y});

        renderTexture.clear(sf::Color::Transparent);
        renderTexture.draw(sprite);
        renderTexture.display();

        sf::Sprite renderSprite(renderTexture.getTexture());

        whiteMaskShader.setUniform("WhiteMultiplier", static_cast<float>(WhiteIntensity));
        whiteMaskShader.setUniform("image", renderSprite.getTexture());
        whiteMaskShader.setUniform("resolution", sf::Glsl::Vec2(winSize));

        window.draw(renderSprite, &whiteMaskShader);
    }
}