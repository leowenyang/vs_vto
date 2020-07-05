#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform float progress;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float intensity = 0.1; // = 0.1
const int passes = 6;


void main()
{
    vec4 c1 = vec4(0.0);
    vec4 c2 = vec4(0.0);

    float disp = intensity*(0.5-distance(0.5, progress));
    for (int xi=0; xi<passes; xi++)
    {
        float x = float(xi) / float(passes) - 0.5;
        for (int yi=0; yi<passes; yi++)
        {
            float y = float(yi) / float(passes) - 0.5;
            vec2 v = vec2(x,y);
            float d = disp;
            c1 += texture(texture1, TexCoord + d*v);
            c2 += texture(texture2, TexCoord + d*v);
        }
    }
    c1 /= float(passes*passes);
    c2 /= float(passes*passes);
    FragColor = mix(c1, c2, progress);
}
