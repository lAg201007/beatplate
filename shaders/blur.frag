#version 130

uniform sampler2D image;
uniform vec2 resolution;
uniform float blurStrength;
uniform bool horizontal;

const float offset[3] = float[](0.0, 1.3846153846, 3.2307692308);
const float weight[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

out vec4 FragColor;

void main() {
    vec2 texCoord = gl_FragCoord.xy / resolution;

    vec4 color = texture(image, texCoord) * weight[0];

    if (horizontal) {
        for (int i = 1; i < 3; ++i) {
            vec2 scaledOffset = vec2(offset[i] * blurStrength / resolution.x, 0.0);

            color += texture(image, texCoord + scaledOffset) * weight[i];
            color += texture(image, texCoord - scaledOffset) * weight[i];
        }
    } else {
        for (int i = 1; i < 3; ++i) {
            vec2 scaledOffset = vec2(0.0, offset[i] * blurStrength / resolution.y);

            color += texture(image, texCoord + scaledOffset) * weight[i];
            color += texture(image, texCoord - scaledOffset) * weight[i];
        }
    }

    FragColor = color;
}
