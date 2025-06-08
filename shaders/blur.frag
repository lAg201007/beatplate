uniform sampler2D texture;
uniform float radius;
uniform vec2 direction; // (1.0, 0.0) para horizontal, (0.0, 1.0) para vertical

void main()
{
    vec2 texCoord = gl_TexCoord[0].xy;
    vec4 sum = vec4(0.0);
    float blurSize = radius / 300.0;

    for (int i = -4; i <= 4; ++i)
    {
        sum += texture2D(texture, texCoord + float(i) * direction * blurSize) * 0.111;
    }

    gl_FragColor = sum;
}