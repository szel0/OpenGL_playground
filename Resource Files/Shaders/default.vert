#version 330 core


layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec3 aNormal;


out vec3 color;
out vec2 texCoord;


out vec3 vNormal;
out vec3 vLight;
out vec3 vEye;


uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;


uniform vec3 lightPos;
uniform vec3 camPos;


void main()
{
	gl_Position = proj * view * model * vec4(aPos, 1.0);


	
	vec3 crntPos = vec3(model * vec4(aPos, 1.0f));
	vLight = lightPos.xyz - crntPos.xyz;
	vNormal = aNormal;
	vEye = camPos.xyz - crntPos.xyz;


	color = aColor;
	texCoord = aTex;
}