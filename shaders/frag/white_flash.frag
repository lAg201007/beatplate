#version 130

uniform float WhiteMultiplier;
uniform sampler2D tex;

out vec4 FragColor;

void main() {
    vec2 texCoord = gl_TexCoord[0].xy; // pega as UVs do sprite
    vec4 color = texture(tex, texCoord);

    float intensity = clamp(WhiteMultiplier / 255.0, 0.0, 1.0);
    vec3 result = mix(color.rgb, vec3(1.0), intensity);

    FragColor = vec4(result, color.a);
}
