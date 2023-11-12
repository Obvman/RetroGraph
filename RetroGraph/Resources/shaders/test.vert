#version 400

in layout(location = 0) vec3 position;
in layout(location = 1) vec4 vertexColor;

out vec4 outColor;

void main() {
    outColor = vertexColor;
}
