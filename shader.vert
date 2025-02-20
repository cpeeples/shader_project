#version 330 core
layout(location = 0) in vec2 position;

out vec2 fragCoord;

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    fragCoord = (position + 1.0) * 0.5 * vec2(3840.0, 2160.0); // Map to pixel coords
}
