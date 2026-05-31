#version 410 core
out vec4 frag_color;
uniform vec4 uni_color;

void main() {
    frag_color = uni_color;
}