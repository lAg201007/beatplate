#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include "../utils/SFML_CLASSES.h"

namespace ShaderUtils {
    void drawRadialBlurSprite(sf::RenderWindow& mWindow, Object obj, float radius) 
    {
        const sf::Vector2u winSize = mWindow.getSize();

        sf::Shader radialBlurShader;
        if (!radialBlurShader.loadFromFile("shaders/radial_blur.frag", sf::Shader::Type::Fragment)) {
            std::cerr << "Erro ao carregar shader radial_blur.frag" << std::endl;
            return;
        }

        // Render o objeto para uma textura
        sf::RenderTexture sceneRT({winSize.x, winSize.y});
        sceneRT.clear(sf::Color::Transparent);
        sceneRT.draw(*obj.sprite);
        sceneRT.display();

        // Configura uniforms
        radialBlurShader.setUniform("txr", sceneRT.getTexture());
        radialBlurShader.setUniform("xs", static_cast<float>(winSize.x));
        radialBlurShader.setUniform("ys", static_cast<float>(winSize.y));
        radialBlurShader.setUniform("r", radius);

        // Prepara um quad que cobre a tela inteira (coords de -1 a 1)
        sf::VertexArray screenQuad(sf::PrimitiveType::TriangleStrip, 4);
        screenQuad[0].position = sf::Vector2f(-1.f, -1.f);
        screenQuad[1].position = sf::Vector2f(+1.f, -1.f);
        screenQuad[2].position = sf::Vector2f(-1.f, +1.f);
        screenQuad[3].position = sf::Vector2f(+1.f, +1.f);

        // Passa a mesma posição como atributo "pos"
        screenQuad[0].texCoords = sf::Vector2f(-1.f, -1.f);
        screenQuad[1].texCoords = sf::Vector2f(+1.f, -1.f);
        screenQuad[2].texCoords = sf::Vector2f(-1.f, +1.f);
        screenQuad[3].texCoords = sf::Vector2f(+1.f, +1.f);

        // Renderiza com o shader
        sf::RenderStates states;
        states.shader = &radialBlurShader;

        mWindow.draw(screenQuad, states);
    }

}