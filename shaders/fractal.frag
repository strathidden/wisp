#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform mat4 u_view;
uniform mat4 u_projection;
uniform vec3 u_cameraPos;
uniform vec2 u_resolution;

uniform int u_maxIterations;
uniform float u_power;
uniform float u_bailout;
uniform float u_scale;
uniform vec3 u_offset;
uniform vec3 u_julia;

uniform float u_stepSize;
uniform float u_minDistance;
uniform float u_maxDistance;
uniform vec3 u_color1;
uniform vec3 u_color2;
uniform float u_ambient;
uniform float u_diffuse;
uniform float u_specular;
uniform float u_shininess;

float DE(vec3 pos) {
    vec3 z = pos;
    float dr = 1.0;
    float r = 0.0;
    
    for (int i = 0; i < u_maxIterations; i++) {
        r = length(z);
        if (r > u_bailout) break;
        
        float theta = acos(z.z / r) * u_power;
        float phi = atan(z.y, z.x) * u_power;
        float zr = pow(r, u_power);
        
        z = zr * vec3(sin(theta) * cos(phi), sin(phi) * sin(theta), cos(theta));
        z += pos;
        
        dr = pow(r, u_power - 1.0) * u_power * dr + 1.0;
    }
    
    return 0.5 * log(r) * r / dr;
}

vec3 calculateNormal(vec3 pos) {
    const float eps = 0.001;
    vec3 n = vec3(
        DE(pos + vec3(eps, 0, 0)) - DE(pos - vec3(eps, 0, 0)),
        DE(pos + vec3(0, eps, 0)) - DE(pos - vec3(0, eps, 0)),
        DE(pos + vec3(0, 0, eps)) - DE(pos - vec3(0, 0, eps))
    );
    return normalize(n);
}

vec3 shade(vec3 pos, vec3 normal, vec3 lightDir) {
    vec3 ambient = u_ambient * u_color1;
    
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * u_diffuse * u_color2;
    
    vec3 viewDir = normalize(u_cameraPos - pos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_shininess);
    vec3 specular = u_specular * spec * vec3(1.0);
    
    return ambient + diffuse + specular;
}

void main() {
    vec2 uv = (TexCoords - 0.5) * 2.0;
    uv.x *= u_resolution.x / u_resolution.y;
    
    vec4 rayClip = vec4(uv, -1.0, 1.0);
    vec4 rayEye = inverse(u_projection) * rayClip;
    rayEye = vec4(rayEye.xy, -1.0, 0.0);
    vec3 rayDir = normalize((inverse(u_view) * rayEye).xyz);
    
    vec3 rayPos = u_cameraPos;
    float totalDistance = 0.0;
    float dist = 0.0;
    
    for (int i = 0; i < 256; i++) {
        dist = DE(rayPos);
        totalDistance += dist;
        rayPos += rayDir * dist;
        
        if (dist < u_minDistance || totalDistance > u_maxDistance) break;
    }
    
    if (dist < u_minDistance) {
        vec3 normal = calculateNormal(rayPos);
        vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
        vec3 color = shade(rayPos, normal, lightDir);
        FragColor = vec4(color, 1.0);
    } else {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
