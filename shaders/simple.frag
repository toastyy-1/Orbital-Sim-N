#version 330 core
in vec3 vertexColor;
out vec4 FragColor;

uniform vec4 colorOverride;
uniform bool useOverride;

void main() {
    if (useOverride) {
        FragColor = colorOverride;
    } else {
        FragColor = vec4(vertexColor, 1.0);
    }
}
