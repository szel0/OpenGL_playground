#version 330 core

out vec4 FragColor;

in vec3 color;       // kolor z wierzchołków
in vec2 texCoord;    // współrzędne tekstury z wierzchołków

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform sampler2D tex4;

uniform int type;

void main()
{
	if (type == 0) {
		FragColor = texture(tex0, texCoord);
	}
	if (type == 1) {
		FragColor = texture(tex1, texCoord);
	}
	if (type == 2) {
		FragColor = texture(tex2, texCoord);
	}
	if (type == 3) {
		FragColor = texture(tex3, texCoord);
	}
	if (type == 4) {
		FragColor = texture(tex4, texCoord);
	}
}
