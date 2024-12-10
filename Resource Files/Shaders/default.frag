#version 330 core

out vec4 FragColor;

in vec3 color;       // kolor z wierzchołków
in vec2 texCoord;    // współrzędne tekstury z wierzchołków
in vec3 vNormal;
in vec3 vLight;
in vec3 vEye;


uniform sampler2D textures[5];
uniform int type;


uniform vec4 lightColor;


void main()
{
	vec3 vNormal_norm = normalize(vNormal);
	vec3 vLight_norm = normalize(vLight);
	vec3 vEye_norm = normalize(vEye);
	vec3 vReflect_norm = reflect(-vLight_norm, vNormal_norm);


	float ambient = 0.1;
	float spec = pow(clamp(dot(vReflect_norm, vEye_norm), 0.0, 1.0 - ambient), 8);
	float diff = clamp(dot(vNormal_norm, vLight_norm), 0.0, 1.0 - ambient);


	FragColor = texture(textures[type], texCoord) * (spec + diff + ambient) * lightColor;
}
