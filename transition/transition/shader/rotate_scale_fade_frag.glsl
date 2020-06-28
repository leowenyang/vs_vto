#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform float progress;

uniform sampler2D texture1;
uniform sampler2D texture2;

#define PI 3.14159265359

uniform vec2 center = vec2(0.5, 0.5); // ;
uniform float rotations  = 1; //;
uniform float scale = 8; //;
uniform vec4 backColor = vec4(0.15, 0.15, 0.15, 1.0); //;

void main()
{
  vec2 difference = TexCoord - center;
  vec2 dir = normalize(difference);
  float dist = length(difference);
  
  float angle = 2.0 * PI * rotations * progress;
  
  float c = cos(angle);
  float s = sin(angle);
  
  float currentScale = mix(scale, 1.0, 2.0 * abs(progress - 0.5));
  
  vec2 rotatedDir = vec2(dir.x  * c - dir.y * s, dir.x * s + dir.y * c);
  vec2 rotatedUv = center + rotatedDir * dist / currentScale;
  
  if (rotatedUv.x < 0.0 || rotatedUv.x > 1.0 ||
      rotatedUv.y < 0.0 || rotatedUv.y > 1.0)
    FragColor = backColor;
    
  FragColor = mix(texture(texture1, rotatedUv), texture(texture2, rotatedUv), progress);
	
}
