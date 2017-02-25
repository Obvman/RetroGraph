#version 450

in layout(location = 0) vec3 position;
in layout(location = 1) vec3 vertexColor;

out vec3 outColor;

void main() {
    outColor = vertexColor;
}
