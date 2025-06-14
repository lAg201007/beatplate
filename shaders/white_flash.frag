#version 330 core

uniform int WhiteMultiplier;
uniform vec2 resolution;
uniform sampler2D image;

out vec4 FragColor;

void main() {
    vec2 texCoord = gl_FragCoord.xy / resolution;
    vec4 color = texture(image, texCoord);

    float intensity = 1.0 - length(color.rgb);
    FragColor = vec4(color.rgb + vec3(intensity * WhiteMultiplier), color.a);
}