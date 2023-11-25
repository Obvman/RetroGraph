#version 450

layout(location = 0) in vec2 vertexPosition;

uniform mat4 model;

out vec4 color;

void main() {
    color = vec4(1.0, 1.0, 1.0, 1.0f);
    gl_Position = model * vec4(vertexPosition, 1.0, 1.0);
}
