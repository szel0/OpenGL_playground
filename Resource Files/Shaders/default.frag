#version 330 core

out vec4 FragColor;

in vec3 color;       // kolor z wierzchołków
in vec2 texCoord;    // współrzędne tekstury z wierzchołków
in vec3 vNormal;
in vec3 vLight;


uniform sampler2D textures[5];
uniform int type;
uniform int shaderType;


uniform vec4 lightColor;

const vec4 Ka = vec4(0.1, 0.1, 0.1, 1.0);
const vec4 Kd = vec4(1.0, 1.0, 1.0, 1.0);



void main()
{
	vec3 vNormal_norm = normalize(vNormal);
	vec3 vLight_norm = normalize(vLight);


	float diff = clamp(dot(vNormal_norm, vLight_norm), 0.0, 1.0);

	switch (shaderType) {
		case 0:
			FragColor = texture(textures[type], texCoord) * Ka + texture(textures[type], texCoord) * Kd * diff * lightColor;
			break;
		case 1:
			FragColor = vec4(0.0, 0.0, 0.0, 1.0);
			break;
		case 2:
			FragColor = vec4(0.0, 0.0, 0.0, 1.0);
			break;
		case 3:
			FragColor = texture(textures[type], texCoord);
			break;
		case 4:
			FragColor = vec4(0.0, 0.0, 0.0, 1.0);
			break;
		default:
			FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	}	
}
