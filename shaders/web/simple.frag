#version 300 es
precision highp float;

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