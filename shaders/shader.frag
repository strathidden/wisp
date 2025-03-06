#version 330 core
out vec4 FragColor;
in vec2 vTexCoord;

uniform vec2 u_resolution;
uniform float u_time;
uniform vec3 u_cameraPos;
uniform mat3 u_cameraRot;

uniform int u_maxSteps;
uniform float u_epsilon;
uniform float u_maxDistance;
uniform int u_iterations;
uniform float u_bailout;
uniform float u_power;

float mandelbulbDE(vec3 pos) {
    vec3 z = pos;
    float dr = 1.0;
    float r = length(z);
    for (int i = 0; i < u_iterations; i++) {
        if (r > u_bailout) break;
        
        float theta = acos(z.z / r);
        float phi = atan(z.y, z.x);
        float zr = pow(r, u_power);
        
        dr = pow(r, u_power - 1.0) * u_power * dr + 1.0;
        
        float sinTheta = sin(u_power * theta);
        z = zr * vec3(
            sinTheta * cos(u_power * phi),
            sinTheta * sin(u_power * phi),
            cos(u_power * theta)
        );
        z += pos;
        r = length(z);
    }
    return 0.5 * log(r) * r / dr;
}

float rayMarch(vec3 ro, vec3 rd) {
    float distanceTraveled = 0.0;
    for (int i = 0; i < u_maxSteps; i++) {
        vec3 pos = ro + rd * distanceTraveled;
        float dist = mandelbulbDE(pos);
        if (dist < u_epsilon)
            return distanceTraveled;
        distanceTraveled += dist;
        if (distanceTraveled > u_maxDistance)
            break;
    }
    return distanceTraveled;
}

vec3 estimateNormal(vec3 p) {
    float h = u_epsilon;
    float dx = mandelbulbDE(p + vec3(h, 0.0, 0.0)) - mandelbulbDE(p - vec3(h, 0.0, 0.0));
    float dy = mandelbulbDE(p + vec3(0.0, h, 0.0)) - mandelbulbDE(p - vec3(0.0, h, 0.0));
    float dz = mandelbulbDE(p + vec3(0.0, 0.0, h)) - mandelbulbDE(p - vec3(0.0, 0.0, h));
    return normalize(vec3(dx, dy, dz));
}

void main() {
    vec2 uv = (gl_FragCoord.xy - 0.5 * u_resolution.xy) / u_resolution.y;
    
    vec3 rayDir = normalize(u_cameraRot * normalize(vec3(uv, -1.0)));
    
    vec3 rayOrigin = u_cameraPos;
    
    float dist = rayMarch(rayOrigin, rayDir);
    
    vec3 color = vec3(0.0);
    if (dist < u_maxDistance) {
        vec3 pos = rayOrigin + rayDir * dist;
        vec3 normal = estimateNormal(pos);
        vec3 lightDir = normalize(vec3(0.5, 0.8, -0.6));
        float diff = clamp(dot(normal, lightDir), 0.0, 1.0);
        color = vec3(diff);
    } else {
        color = mix(vec3(0.1, 0.1, 0.2), vec3(0.0, 0.0, 0.0), uv.y + 0.5);
    }
    
    color = pow(color, vec3(0.4545));
    
    FragColor = vec4(color, 1.0);
}
