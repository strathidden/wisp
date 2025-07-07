#version 460 core

in vec2 fragCoord;
out vec4 FragColor;

uniform int fractalType;

uniform int iterations;
uniform vec3 offset;
uniform float scale;

uniform float power;
uniform float bailout;

uniform int pointsPerFrame;
uniform vec3 rotation;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPosition;

const float MAX_DIST = 100.0;
const float MIN_DIST = 0.001;
const int MAX_STEPS = 100;

float opUnion(float d1, float d2) {
    return min(d1, d2);
}

float opSubtract(float d1, float d2) {
    return max(-d1, d2);
}

float opIntersect(float d1, float d2) {
    return max(d1, d2);
}

vec3 rotate(vec3 p, vec3 angles) {
    float cx = cos(angles.x);
    float sx = sin(angles.x);
    float cy = cos(angles.y);
    float sy = sin(angles.y);
    float cz = cos(angles.z);
    float sz = sin(angles.z);
    
    mat3 rotX = mat3(
        1, 0, 0,
        0, cx, -sx,
        0, sx, cx
    );
    
    mat3 rotY = mat3(
        cy, 0, sy,
        0, 1, 0,
        -sy, 0, cy
    );
    
    mat3 rotZ = mat3(
        cz, -sz, 0,
        sz, cz, 0,
        0, 0, 1
    );
    
    return rotZ * rotY * rotX * p;
}

float mandelbulbDE(vec3 pos) {
    vec3 z = pos + offset;
    float dr = 1.0;
    float r = 0.0;
    
    for (int i = 0; i < iterations; i++) {
        r = length(z);
        if (r > bailout) break;
        
        float theta = acos(z.z / r);
        float phi = atan(z.y, z.x);
        dr = pow(r, power - 1.0) * power * dr + 1.0;
        
        float zr = pow(r, power);
        theta = theta * power;
        phi = phi * power;
        
        z = zr * vec3(sin(theta) * cos(phi), sin(phi) * sin(theta), cos(theta));
        z += pos + offset;
    }
    return 0.5 * log(r) * r / dr;
}

float mengerSpongeDE(vec3 p) {
    p += offset;
    float d = length(p) - 0.5;
    float s = 1.0;
    
    for (int i = 0; i < iterations; i++) {
        p = abs(p);
        if (p.x < p.y) p.xy = p.yx;
        if (p.x < p.z) p.xz = p.zx;
        if (p.y < p.z) p.yz = p.zy;
        
        p = p * scale - offset * (scale - 1.0);
        s *= scale;
        
        vec3 q = p - min(p, 1.0);
        d = min(d, length(q) / s);
    }
    return d;
}

float raymarch(vec3 ro, vec3 rd) {
    float t = 0.0;
    
    for (int i = 0; i < MAX_STEPS; i++) {
        vec3 p = ro + rd * t;
        float d;
        
        if (fractalType == 0) {
            d = mandelbulbDE(p);
        } else if (fractalType == 1) {
            d = mengerSpongeDE(p);
        } else {
            d = 1.0;
        }
        
        if (d < MIN_DIST || t > MAX_DIST) break;
        t += d;
    }
    
    return t;
}

vec3 calcNormal(vec3 p) {
    const float eps = 0.001;
    vec2 e = vec2(eps, 0);
    
    float d = mandelbulbDE(p);
    return normalize(vec3(
        mandelbulbDE(p - e.xyy) - d,
        mandelbulbDE(p - e.yxy) - d,
        mandelbulbDE(p - e.yyx) - d
    ));
}

vec3 lighting(vec3 p, vec3 rd) {
    vec3 normal = calcNormal(p);
    vec3 lightDir = normalize(vec3(1, 1, 1));
    
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 color = vec3(0.8, 0.6, 0.2);
    
    vec3 ambient = 0.1 * color;
    
    vec3 viewDir = normalize(-rd);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = vec3(0.5) * spec;
    
    return (ambient + diff) * color + specular;
}

void main() {
    vec2 uv = fragCoord;
    vec4 rayClip = vec4(uv, 1.0, 1.0);
    vec4 rayEye = inverse(projection) * rayClip;
    rayEye = vec4(rayEye.xy, -1.0, 0.0);
    vec3 rayWorld = normalize((inverse(view) * rayEye).xyz);
    
    float t = raymarch(cameraPosition, rayWorld);
    
    if (t < MAX_DIST) {
        vec3 p = cameraPosition + rayWorld * t;
        vec3 color = lighting(p, rayWorld);
        FragColor = vec4(color, 1.0);
    } else {
        vec3 color = mix(vec3(0.5, 0.7, 1.0), vec3(0.0, 0.1, 0.3), rayWorld.y);
        FragColor = vec4(color, 1.0);
    }
}
