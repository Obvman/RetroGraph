#version 450

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in float lineLength;

uniform mat4 model;

out vec4 color;

void main() {
    float radiusSq = 0.2 * 0.2;
    color = vec4(1.0, 1.0, 1.0, 1.0f - (lineLength / radiusSq));
    gl_Position = model * vec4(vertexPosition, 1.0, 1.0);
}
