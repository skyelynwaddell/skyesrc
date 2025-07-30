#version 330

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

#define MAX_LIGHTS 150
#define LIGHT_DIRECTIONAL 0
#define LIGHT_POINT 1

struct Light {
    int enabled;
    int type;
    vec3 position;
    vec3 target;
    vec4 color;
};

uniform Light lights[MAX_LIGHTS];
uniform vec4 ambient;
uniform float radius[MAX_LIGHTS];

out vec4 finalColor;

void main()
{
    vec4 texelColor = texture(texture0, fragTexCoord);
    vec3 normal = normalize(fragNormal);
    vec3 lightAccum = vec3(0.0);
    vec4 tint = colDiffuse * fragColor;

    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        if (lights[i].enabled == 1)
        {
            vec3 lightDir;
            float attenuation = 1.0;

            if (lights[i].type == LIGHT_DIRECTIONAL)
            {
                lightDir = -normalize(lights[i].target - lights[i].position);
            }
            else if (lights[i].type == LIGHT_POINT)
            {
                vec3 delta = lights[i].position - fragPosition;
                float dist = length(delta);
                lightDir = normalize(delta);

                float normDist = dist / radius[i];
                attenuation = 1.0 - smoothstep(0.2, 1.0, normDist); // Smoother fade near edge
            }

            float NdotL = max(dot(normal, lightDir), 0.0);
            lightAccum += lights[i].color.rgb * NdotL * attenuation;
        }
    }

    // Apply accumulated lighting
    vec3 litColor = texelColor.rgb * lightAccum * tint.rgb;

    // Add ambient light
    litColor += texelColor.rgb * (ambient.rgb * 0.1) * tint.rgb;

    finalColor = vec4(litColor, 1.0);

    // Optional gamma correction (slight cost, still OK)
    finalColor.rgb = pow(finalColor.rgb, vec3(1.0 / 2.2));
}
