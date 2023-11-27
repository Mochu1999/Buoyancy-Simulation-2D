#shader vertex

#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

uniform mat4 u_MVP;
out vec2 v_TexCoord;

void main() {
	gl_Position = u_MVP * position;
	v_TexCoord = texCoord;
}


#shader fragment

#version 330 core
layout(location = 0) out vec4 color;
in vec2 v_TexCoord;

uniform vec4 u_Color;  // Uniform variable for color
uniform sampler2D u_Texture;
uniform int u_RenderType; // 0 for geometry, 1 for text

void main() {
	
	if (u_RenderType == 0) {
		color = u_Color;
	}
	else if (u_RenderType == 1) {
		float alpha = texture(u_Texture, v_TexCoord).r;
		color = vec4(1.0 * alpha, 1.0* alpha, 1.0* alpha, 0);
		//El valor alpha (el final) no funciona, ni aquí ni con los que hacen colorLocation
		//haz funcionar eso y el background de las letras tendría que irse
    }
};

