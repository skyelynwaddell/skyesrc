#version 330 core

in vec2 fragTexCoord;
out vec4 fragColor;

uniform sampler2D tex;
uniform float time;
uniform float wave_speed = 1.5;
uniform float water_alpha = 0.6;

const float PI = 3.14159265359;

void main() {
    vec2 uv = fragTexCoord;

    vec2 baseUV = uv;

    uv.x += sin(baseUV.y * wave_speed * PI + time) * cos(baseUV.y * wave_speed * PI + time) * 0.1;
    uv.y += cos(baseUV.x * wave_speed * PI + time) * -sin(baseUV.y * wave_speed * PI + time) * 0.1;

    uv = fract(uv);

    vec4 color = texture(tex, uv);

    // Set output color with alpha multiplied by water_alpha uniform
    fragColor = vec4(color.rgb, color.a * water_alpha);
}
