#version 410 core
out vec4 frag_color;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 Viewpos;

struct Light_st {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light_st light;

struct Material_st {
    float shininess;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Material_st mat;

void main() {
    vec3 ambient = light.ambient * mat.ambient;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * mat.diffuse);

    vec3 viewDir = normalize(Viewpos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
    vec3 specular = light.specular * (spec * mat.specular);

    vec3 Summ = ambient + diffuse + specular;
    frag_color = vec4(Summ, 1.0);
}