#version 450

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec4 vertexColor;

uniform mat4 model;

out vec4 color;


void main() {
    color = vertexColor;
    gl_Position = model * vec4(vertexPosition, 1.0, 1.0);
}
