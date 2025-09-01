#include "shader_manager.h"
#include <variant>
#include <optional>

namespace ShaderUtils {
    sf::Shader createShader(const std::string& fragmentShaderPath, std::optional<std::string> vertexShaderPath, std::vector<std::pair<std::string, std::variant<int, float, bool, sf::Vector2f, sf::Texture, sf::Shader::CurrentTextureType>>> uniforms) {
        sf::Shader shader;

        if (vertexShaderPath.has_value()) {
            if (!shader.loadFromFile(vertexShaderPath.value(), fragmentShaderPath)) {
                std::cerr << "Failed to load shader from " << vertexShaderPath.value() << " and " << fragmentShaderPath << std::endl;
                return shader;
            }
        }
        else {
            if (!shader.loadFromFile(fragmentShaderPath, sf::Shader::Type::Fragment)) {
                std::cerr << "Failed to load shader from " << fragmentShaderPath << std::endl;
                return shader;
            }
        }
        
        for (const auto& [name, value] : uniforms) {
            if (std::holds_alternative<int>(value)) {
                shader.setUniform(name, std::get<int>(value));
            } else if (std::holds_alternative<float>(value)) {
                shader.setUniform(name, std::get<float>(value));
            } else if (std::holds_alternative<sf::Vector2f>(value)) {
                shader.setUniform(name, std::get<sf::Vector2f>(value));
            } else if (std::holds_alternative<sf::Texture>(value)) {
                shader.setUniform(name, std::get<sf::Texture>(value));
            } else if (std::holds_alternative<sf::Shader::CurrentTextureType>(value)) {
                shader.setUniform(name, std::get<sf::Shader::CurrentTextureType>(value));
            } else if (std::holds_alternative<bool>(value)) {
                shader.setUniform(name, std::get<bool>(value));
            }
            else {
                std::cerr << "Unsupported uniform type for " << name << std::endl;
            }
        }

        return shader;
    }

    sf::Shader createWhiteMaskShader(float WhiteMultiplier, sf::RenderWindow& window) {
        sf::Shader shader = ShaderUtils::createShader("shaders/frag/white_flash.frag", std::nullopt, { 
                {"WhiteMultiplier", WhiteMultiplier},
                {"texture", sf::Shader::CurrentTexture}
            }
        );

        return shader;
    }

    sf::Texture applyBlur(sf::RenderWindow& window, sf::Sprite& sprite, float blurStrength) {
        sf::RenderTexture rtH = (window.getSize());
        sf::RenderTexture rtV = (window.getSize());
        
        sf::Shader shaderH = ShaderUtils::createShader("shaders/frag/blur.frag", std::nullopt, {
            {"texture", sf::Shader::CurrentTexture},
            {"resolution", sf::Vector2f(window.getSize())},
            {"blurStrength", blurStrength},
            {"horizontal", true}
        });

        sf::Shader shaderV = ShaderUtils::createShader("shaders/frag/blur.frag", std::nullopt, {
            {"texture", sf::Shader::CurrentTexture},
            {"resolution", sf::Vector2f(window.getSize())},
            {"blurStrength", blurStrength},
            {"horizontal", false}
        });

        rtH.clear(sf::Color::Transparent);
        rtH.draw(sprite, &shaderH);
        rtH.display();

        sf::Sprite intermediate(rtH.getTexture());
        rtV.clear(sf::Color::Transparent);
        rtV.draw(intermediate, &shaderV);
        rtV.display();

        sf::Image img = rtV.getTexture().copyToImage();
        img.flipVertically();

        sf::Texture result;
        if (!result.loadFromImage(img)) {
            std::cerr << "Failed to load texture from image" << std::endl;
        }
        return result;
    }

    // silly lil hack to apply blur without exploding the game, and scaling it correctly
    Object applyBlurToObject(sf::RenderWindow& window, Object& object, float blurStrength) {
        *object.spriteTexture = ShaderUtils::applyBlur(window, *object.sprite, 2.f);
        object.sprite->setTexture(*object.spriteTexture, true); 
        auto ws = window.getSize();
        auto ts = object.spriteTexture->getSize();
        object.sprite->setScale({ float(ws.x)/ts.x, float(ws.y)/ts.y });
        return object;
    }
    
    void drawShader(sf::RenderWindow& window, sf::Sprite& sprite, sf::Shader& shader) {
        window.draw(sprite, &shader);
    }
}