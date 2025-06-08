#version 120

uniform sampler2D image;
uniform vec2 resolution;
uniform float blurStrength;
uniform bool horizontal;

const float offset[3] = float[](0.0, 1.3846153846, 3.2307692308);
const float weight[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

void main() {
    vec2 texCoord = gl_FragCoord.xy / resolution;

    vec4 color = texture2D(image, texCoord) * weight[0];

    if (horizontal) {
        for (int i = 1; i < 3; ++i) {
            // deslocamento no eixo X para blur horizontal
            vec2 scaledOffset = vec2(offset[i] * blurStrength / resolution.x, 0.0);

            color += texture2D(image, texCoord + scaledOffset) * weight[i];
            color += texture2D(image, texCoord - scaledOffset) * weight[i];
        }
    } else {
        for (int i = 1; i < 3; ++i) {
            // deslocamento no eixo Y para blur vertical
            vec2 scaledOffset = vec2(0.0, offset[i] * blurStrength / resolution.y);

            color += texture2D(image, texCoord + scaledOffset) * weight[i];
            color += texture2D(image, texCoord - scaledOffset) * weight[i];
        }
    }

    gl_FragColor = color;
}
