#version 330 core

in vec3 vertexPosition;
in vec2 vertexTexCoord;

uniform mat4 mvp; // Model-View-Projection matrix

out vec2 fragTexCoord;

void main() {
    fragTexCoord = vertexTexCoord;
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
