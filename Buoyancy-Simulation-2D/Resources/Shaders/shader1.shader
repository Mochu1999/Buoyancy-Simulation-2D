#shader vertex
#version 330 core

layout(location = 0) in vec4 position;

uniform mat4 u_MVP;

void main() {
	gl_Position = u_MVP * position;
};
//#version 330 core
//
//layout(location = 0) in vec2 vertexPosition; // Changed to vec2 since we're only interested in x and y
//layout(location = 1) in vec2 vertexTexCoord; // Texture coordinate input from the VBO
//
//uniform mat4 u_MVP; // Uniform for Model-View-Projection matrix
//
//out vec2 texCoord; // Pass texture coordinate to fragment shader
//
//void main() {
//    gl_Position = u_MVP * vec4(vertexPosition, 0.0, 1.0); // Extend vertexPosition to vec4 with z = 0.0 and w = 1.0
//    texCoord = vertexTexCoord; // Pass the texture coordinate to the fragment shader
//};


#shader fragment


#version 330 core
layout(location = 0) out vec4 color;
uniform vec4 u_Color;  // Uniform variable for color

void main() {
	color = u_Color;
};

//#version 330 core
//
//in vec2 texCoord; // Received from the vertex shader
//
//uniform sampler2D u_Texture; // Uniform for the text texture atlas
//
//layout(location = 0) out vec4 color; // Output color
//
//void main() {
//    color = vec4(1.0, 1.0, 1.0, texture(u_Texture, texCoord).r); // Use the red channel for the glyph's alpha
//};