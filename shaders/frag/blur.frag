#version 330 core

uniform sampler2D image;
uniform vec2 resolution;
uniform float blurStrength; // funciona como o sigma da Gaussiana
uniform bool horizontal;

out vec4 FragColor;

const int KERNEL_RADIUS = 5;

float gaussian(float x, float sigma) {
    return exp(-(x * x) / (2.0 * sigma * sigma)) / (2.0 * 3.14159265 * sigma * sigma);
}

void main() {
    vec2 texCoord = gl_FragCoord.xy / resolution;
    vec2 direction = horizontal ? vec2(1.0 / resolution.x, 0.0) : vec2(0.0, 1.0 / resolution.y);

    vec4 color = vec4(0.0);
    float totalWeight = 0.0;

    for (int i = -KERNEL_RADIUS; i <= KERNEL_RADIUS; ++i) {
        float offset = float(i);
        float weight = gaussian(offset, blurStrength);

        vec2 sampleCoord = texCoord + direction * offset * blurStrength;
        color += texture(image, sampleCoord) * weight;
        totalWeight += weight;
    }

    FragColor = color / totalWeight;
}
