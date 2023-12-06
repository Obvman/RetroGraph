#version 450

layout(location = 0) in vec2 position;

uniform float xOffset;
uniform vec4 color;
uniform mat4 model;

out vec4 vertColor;

void main() {
    vertColor = color;
    gl_Position = model * vec4(position.x + xOffset, position.y, 1.0, 1.0);
}
