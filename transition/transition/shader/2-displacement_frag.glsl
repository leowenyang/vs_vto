#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform float progress;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform sampler2D filterMap;

uniform float strength = 0.5; // = 0.5

void main()
{
  float displacement = texture2D(filterMap, TexCoord).r * strength;

  vec2 uvFrom = vec2(TexCoord.x + progress * displacement, TexCoord.y);
  vec2 uvTo = vec2(TexCoord.x - (1.0 - progress) * displacement, TexCoord.y);

  FragColor = mix(texture(texture1, uvFrom), texture(texture2, uvTo), progress);
}

