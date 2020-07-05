#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform float progress;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    vec2 dir = TexCoord - vec2(.5);
    float dist = length(dir);
	
    if (dist > progress) {
      FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), progress);
    } else {
      vec2 offset = dir * sin(dist * 30 - progress * 30);
      FragColor = mix(texture(texture2, TexCoord + offset), texture(texture2, TexCoord), progress);
    }
}