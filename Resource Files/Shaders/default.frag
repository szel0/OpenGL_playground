#version 330 core

out vec4 FragColor;

in vec3 color;       // kolor z wierzchołków
in vec2 texCoord;    // współrzędne tekstury z wierzchołków
in vec3 Normal;
in vec3 crntPos;

uniform sampler2D textures[5];
uniform int type;


uniform vec4 lightColor;

uniform vec3 lightPos;

uniform vec3 camPos;

void main()
{
	// ambient lighting
	float ambient = 0.20f;

	float distance = length(lightPos - crntPos); // odległość między źródłem światła a obiektem
	float attenuation = 1.0 / (distance * distance); // lub jakaś inna funkcja wygładzająca

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPos - crntPos);
	float diffuse = max(dot(normal, lightDirection), 0.0f) * attenuation;

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 8);
	float specular = specAmount * specularLight;

	FragColor = texture(textures[type], texCoord) * lightColor * (diffuse + ambient + specular);
}
