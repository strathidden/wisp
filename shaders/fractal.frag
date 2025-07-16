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

uniform float u_stepSize;
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
        
        dr = pow(r, u_power - 1.0) * u_power * dr + 1.0;
    }
    
    return 0.5 * log(r) * r / dr;
}

vec3 calculateNormal(vec3 pos) {
    const float eps = 0.001;
    vec2 e = vec2(eps, 0);
    
    float d = DE(pos);
    return normalize(vec3(
        DE(pos + e.xyy) - d,
        DE(pos + e.yxy) - d,
        DE(pos + e.yyx) - d
    ));
}

vec3 shade(vec3 pos, vec3 normal, vec3 lightDir, float ao) {
    float colorFactor = smoothstep(0.0, 1.0, length(pos) / 5.0);
    vec3 baseColor = mix(u_color1, u_color2, colorFactor);
    
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

    vec3 rayWorld = normalize((inverse(u_viewMatrix) * rayEye).xyz);

    return rayWorld;
}

void main() {
    vec3 finalColor = vec3(0.0);
    
    for (int s = 0; s < u_samples; s++) {
        vec2 jitter = vec2(
            float(s % 2) * 0.5,
            float(s / 2) * 0.5
        ) / float(u_samples);
        
        vec2 uv = TexCoords + jitter;
        
        vec3 rayDir = getRayDirection(uv);
        vec3 rayPos = u_cameraPosition;
        
        float totalDistance = 0.0;
        float dist = 0.0;
        int steps = 0;
        float aoFactor = 1.0;
    
        for (int i = 0; i < u_maxSteps; i++) {
            dist = DE(rayPos);
            totalDistance += dist;
            rayPos += rayDir * dist;
            steps++;
            
            aoFactor -= 1.0 / float(u_maxSteps);
            
            if (dist < u_minDistance || totalDistance > u_maxDistance) {
                break;
            }
        }
        
        vec3 color;
        
        if (dist < u_minDistance) {
            vec3 normal = calculateNormal(rayPos);
            vec3 lightDir = normalize(vec3(1.0, 1.0, 0.5));
            color = shade(rayPos, normal, lightDir, clamp(aoFactor, 0.1, 1.0));
        } 
        else {
            float t = 0.5 * (rayDir.y + 1.0);
            color = mix(vec3(0.3, 0.4, 0.6), vec3(0.7, 0.8, 1.0), t);
        }
        
        finalColor += color;
    }
    
    finalColor /= float(u_samples);
    finalColor = pow(finalColor, vec3(1.0 / 2.2));
    
    FragColor = vec4(finalColor, 1.0);
}
