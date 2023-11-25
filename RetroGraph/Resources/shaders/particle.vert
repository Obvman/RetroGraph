#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in float scale;

uniform mat4 model;

out vec4 color;

void main() {
    color = vec4(0.6, 0.6, 0.6, 1.0);
    gl_PointSize = scale;
    gl_Position = model * vec4(position, 1.0, 1.0);
}
