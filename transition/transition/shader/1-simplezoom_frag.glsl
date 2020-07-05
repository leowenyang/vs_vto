#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform float progress;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float zoom_quickness = 0.8; // = 0.8
float nQuick = clamp(zoom_quickness,0.2,1.0);


vec2 zoom(vec2 uv, float amount) {
  return 0.5 + ((uv - 0.5) * (1.0-amount));	
}

void main()
{
  FragColor = mix(
    texture(texture1, zoom(TexCoord, smoothstep(0.0, nQuick, progress))),
    texture(texture2, TexCoord),
   smoothstep(nQuick-0.2, 1.0, progress)
  );
}