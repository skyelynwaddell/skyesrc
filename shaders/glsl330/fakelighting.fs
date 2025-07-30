#version 330

in vec3 fragNormal;
in vec2 fragTexCoord;

out vec4 finalColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

void main()
{
    vec3 normal = normalize(fragNormal);

    vec3 lightDir = normalize(vec3(-0.5, 1.0, 0.5));
    float diffuse = max(dot(normal, lightDir), 0.0);

    // Snap diffuse into bands for hard shadows
    if (diffuse > 0.66)
        diffuse = 1.0;
    else if (diffuse > 0.33)
        diffuse = 0.6;
    else if (diffuse > 0.1)
        diffuse = 0.3;
    else
        diffuse = 0.1;

    float ambient = 0.15;

    // Brighten upward facing surfaces (like skylight)
    float upLight = max(dot(normal, vec3(0.0, 1.0, 0.0)), 0.0);
    // Scale and clamp to add a little extra light on tops
    upLight = clamp(upLight * 0.5, 0.0, 0.5);

    float lighting = (diffuse + ambient + upLight);
    lighting = clamp(lighting, 0.0, 1.0);

    vec4 texColor = texture(texture0, fragTexCoord);
    finalColor = texColor * colDiffuse * vec4(vec3(lighting), 1.0);
}
