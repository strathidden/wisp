#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform vec3 u_cameraPosition;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;
uniform vec2 u_resolution;

uniform int u_maxIterations;
uniform float u_power;
uniform float u_bailout;
uniform float u_scale;
uniform vec3 u_offset;

uniform float u_minDistance;
uniform float u_maxDistance;
uniform vec3 u_color1;
uniform vec3 u_color2;
uniform float u_ambient;
uniform float u_diffuse;
uniform float u_specular;
uniform float u_shininess;
uniform int u_maxSteps;
uniform int u_samples;

float DE(vec3 pos);
vec3 calculateNormal(vec3 pos);
vec3 shade(vec3 pos, vec3 normal, vec3 lightDir, float ao);
vec3 getRayDirection(vec2 uv);

float DE(vec3 pos) {
    vec3 z = pos * u_scale + u_offset;
    float dr = 1.0;
    float r = 0.0;
    
    for (int i = 0; i < u_maxIterations; i++) {
        r = length(z);
        if (r > u_bailout) break;
        
        float theta = acos(z.z / r) * u_power;
        float phi = atan(z.y, z.x) * u_power;
        float zr = pow(r, u_power);
        
        z = zr * vec3(sin(theta) * cos(phi), sin(phi) * sin(theta), cos(theta));
        z += pos * u_scale + u_offset;
        
        dr = zr / r * u_power * dr + 1.0;
    }
    
    return 0.5 * log(r) * r / dr / u_scale;
}

vec3 calculateNormal(vec3 pos) {
    float distance = DE(pos);
    float eps = max(0.001, 0.001 * distance);
    vec3 e = vec3(eps, 0, 0);
    
    return normalize(vec3(
        DE(pos + e.xyy) - DE(pos - e.xyy),
        DE(pos + e.yxy) - DE(pos - e.yxy),
        DE(pos + e.yyx) - DE(pos - e.yyx)
    ));
}

vec3 shade(vec3 pos, vec3 normal, vec3 lightDir, float ao) {
    vec3 baseColor = (normal.y > 0.0) ? u_color1 : u_color2;
    
    vec3 ambient = u_ambient * baseColor * ao;
    
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * u_diffuse * baseColor;
    
    vec3 viewDir = normalize(u_cameraPosition - pos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_shininess);
    vec3 specular = u_specular * spec * vec3(1.0);
    
    return ambient + diffuse + specular;
}

vec3 getRayDirection(vec2 uv) {
    vec4 rayClip = vec4(uv * 2.0 - 1.0, -1.0, 1.0);
    vec4 rayEye = inverse(u_projectionMatrix) * rayClip;
    rayEye = vec4(rayEye.xy, -1.0, 0.0);
    return normalize((inverse(u_viewMatrix) * rayEye).xyz);
}

void main() {
    vec3 finalColor = vec3(0.0);
    vec3 lightDir = normalize(vec3(1.0, 1.0, 0.5));
    
    for (int s = 0; s < u_samples; s++) {
        //AA with golden ratio sampling
        float goldenAngle = 2.39996; // radians (≈137.5°)
        float r = sqrt(float(s) + 0.5) / sqrt(float(u_samples));
        float theta = float(s) * goldenAngle;
        vec2 jitter = vec2(r * cos(theta), r * sin(theta)) / u_resolution;
        
        vec2 uv = TexCoords + jitter;
        vec3 rayDir = getRayDirection(uv);
        vec3 rayPos = u_cameraPosition;
        
        float totalDistance = 0.0;
        float dist = 0.0;
        int steps = 0;
        float ao = 1.0;
    
        for (int i = 0; i < u_maxSteps; i++) {
            dist = DE(rayPos);
            totalDistance += dist;
            rayPos += rayDir * dist;
            steps++;
            
            if (dist < u_minDistance || totalDistance > u_maxDistance) {
                break;
            }
        }
        
        vec3 color;
        
        if (dist < u_minDistance) {
            ao = exp(-0.1 * float(steps));
            vec3 normal = calculateNormal(rayPos);
            color = shade(rayPos, normal, lightDir, ao);
        } 
        else {
            float t = rayDir.y * 0.5 + 0.5;
            color = mix(vec3(0.8), vec3(1.0), t);
        }
        
        finalColor += color;
    }
    
    finalColor /= float(u_samples);
    finalColor = pow(finalColor, vec3(1.0 / 2.2));
    
    FragColor = vec4(finalColor, 1.0);
}
