#version 450

layout(location = 0) in vec2 position;

uniform float xOffset;
uniform vec4 color;

out vec4 vertColor;

void main() {
    vertColor = color;
    gl_Position = vec4(position.x + xOffset, position.y, 1.0, 1.0);
}
