#version 330 core

uniform float DarkMultiplier;
uniform sampler2D image;
uniform vec2 resolution;

out vec4 FragColor;

void main() {
    vec2 texCoord = gl_FragCoord.xy / resolution;
    vec4 color = texture(image, texCoord);
    float intensity = clamp(DarkMultiplier / 255.0, 0.0, 1.0);
    vec3 result = mix(color.rgb, vec3(0.0), intensity);
    FragColor = vec4(result, color.a);
}