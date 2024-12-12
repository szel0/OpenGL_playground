#version 330 core

out vec4 FragColor;

in vec3 color;       // kolor z wierzchołków
in vec2 texCoord;    // współrzędne tekstury z wierzchołków
in vec3 vNormal;
in vec3 vLight;
in vec3 vEye;


uniform sampler2D tex0;
uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float Ns;


uniform vec4 lightColor;

const float attenuationConst = 1.0;
const float attenuationLinear = 0.09;
const float attenuationQuad = 0.032;


void main()
{
	float distance = length(vLight);

	vec3 vNormal_norm = normalize(vNormal);
	vec3 vLight_norm = normalize(vLight);
	vec3 vEye_norm = normalize(vEye);

	vec3 vReflect_norm = reflect(-vLight_norm, vNormal_norm);

	vec3 ambient = Ka;

    float diff = max(dot(vNormal_norm, vLight_norm), 0.0);
    vec3 diffuse = Kd * diff * lightColor.rgb;

	float spec = pow(max(dot(vReflect_norm, vEye_norm), 0.0), Ns);
    vec3 specular = Ks * spec * lightColor.rgb;

	vec3 texColor = texture(tex0, texCoord).rgb;

	float attenuation = 1.0 / (attenuationConst + attenuationLinear * distance + attenuationQuad * distance * distance);

	vec3 finalColor = (ambient + attenuation * (diffuse + specular));

	FragColor = vec4(finalColor * texColor, 1.0);
}
