#version 330 core

in vec2 UV;

out vec3 color;

uniform sampler2D myTextureSampler;

void main() {
	vec2 UVi = vec2(UV.x, 1.0-UV.y); // invert UV map
	color = texture(myTextureSampler, UVi).rgb;
}