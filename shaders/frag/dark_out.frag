#version 130

uniform float DarkMultiplier;
uniform sampler2D tex;

out vec4 FragColor;

void main() {
    vec2 texCoord = gl_TexCoord[0].xy;
    vec4 color = texture(tex, texCoord);

    float intensity = clamp(DarkMultiplier / 255.0, 0.0, 1.0);
    vec3 result = mix(color.rgb, vec3(0.0), intensity);

    FragColor = vec4(result, color.a);
}