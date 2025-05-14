#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main() {
    vec4 texColor = texture(texture1, TexCoords);
    // Add a glow effect to the sun
    FragColor = texColor * 1.2; // Make it slightly brighter
}