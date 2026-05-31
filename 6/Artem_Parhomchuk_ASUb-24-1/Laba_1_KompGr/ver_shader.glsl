#version 410 core
layout (location=0) in vec3 vp;
layout (location=1) in vec3 aNormal;
uniform vec3 yPos;
uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

out vec3 Normal;
out vec3 FragPos;

void main() {
	Normal = normalMatrix * aNormal;
	FragPos = vec3(model * vec4(vp, 1.0));
	gl_Position = projection * view * model * transform * vec4( vp.x, vp.y, vp.z, 1.0);
}