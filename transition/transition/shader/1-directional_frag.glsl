#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform float progress;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform vec2 direction = vec2(1.0, 0.0); // = vec2(0.0, 1.0)


void main()
{
    vec2 p = TexCoord + progress * sign(direction);
    vec2 f = fract(p);
    FragColor = mix(texture(texture2, f), texture(texture1, f), step(0.0, p.y) * step(p.y, 1.0) * step(0.0, p.x) * step(p.x, 1.0));
}