#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform float progress;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float count = 10.0; // = 10.0
uniform float smoothness = 0.5; // = 0.5


void main()
{
	float pr = smoothstep(-smoothness, 0.0, TexCoord.x - progress * (1.0 + smoothness));
	float s = step(pr, fract(count * TexCoord.x));
	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), s);
	
	
}