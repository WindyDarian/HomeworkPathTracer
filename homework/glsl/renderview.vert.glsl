#version 150
in vec3 position;
in vec2 texcoord;
out vec2 fragTexCoord;

void main() {
    // Pass the tex coord straight through to the fragment shader
    fragTexCoord = texcoord;

    gl_Position = vec4(position, 1);
}
