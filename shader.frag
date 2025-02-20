#version 330 core

in vec2 fragCoord;
out vec4 fragColor;

uniform float time;
uniform vec2 resolution;

// Smooth noise for plasma
float noise(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float smoothNoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(noise(i), noise(i + vec2(1.0, 0.0)), u.x),
               mix(noise(i + vec2(0.0, 1.0)), noise(i + vec2(1.0, 1.0)), u.x), u.y);
}

float layeredNoise(vec2 p) {
    float n = 0.0;
    n += 0.5000 * smoothNoise(p * 2.0);
    n += 0.2500 * smoothNoise(p * 4.0);
    n += 0.1250 * smoothNoise(p * 8.0);
    n += 0.0625 * smoothNoise(p * 16.0);
    return n * 0.6; // Darker plasma
}

// Shape functions for particles
float star(vec2 p, float size) {
    float a = atan(p.y, p.x) + time * 0.25; // Slightly faster rotation
    float r = length(p);
    float starShape = abs(cos(a * 5.0)) * 0.5 + 0.5;
    return smoothstep(size * 0.8, size * 0.2, r * starShape);
}

float heart(vec2 p, float size) {
    p /= size;
    float x = p.x;
    float y = p.y - sqrt(abs(x));
    float d = length(vec2(x, y)) - 0.7;
    return smoothstep(0.1, 0.0, d);
}

float spiral(vec2 p, float size) {
    float a = atan(p.y, p.x) + time * 0.4; // Faster swirl
    float r = length(p);
    float spiralShape = sin(a * 10.0 + r * 20.0 - time * 2.5);
    return smoothstep(0.2, 0.0, abs(r - size * spiralShape * 0.1));
}

// Morphing between shapes
float particleShape(vec2 p, float t) {
    float cycle = mod(t * 0.1, 3.0);
    float blend = fract(cycle);
    float shape1, shape2;

    if (cycle < 1.0) {
        shape1 = star(p, 0.8);
        shape2 = heart(p, 0.8);
    } else if (cycle < 2.0) {
        shape1 = heart(p, 0.8);
        shape2 = spiral(p, 0.8);
    } else {
        shape1 = spiral(p, 0.8);
        shape2 = star(p, 0.8);
    }

    return mix(shape1, shape2, smoothstep(0.0, 1.0, blend));
}

void main() {
    vec2 uv = fragCoord / resolution.xy;
    vec2 p = uv * 12.0 - vec2(6.0);

    // Plasma background
    float t = time * 0.7;
    float swirl = sin(t) * 0.8 + cos(p.x + t * 1.2) * 0.6 + sin(p.y + t * 0.9) * 0.5;
    vec2 swirlOffset = vec2(cos(swirl + p.y), sin(swirl + p.x)) * 1.0;
    vec2 noisePos = p + swirlOffset + vec2(t * 0.4, t * 0.5);
    float plasma = layeredNoise(noisePos);

    float pulse = sin(t * 1.5 + length(p) * 4.0) * 0.15 + 0.85; // Tighter pulse range
    plasma *= pulse;

    float depth = sin(plasma * 3.14159 + t * 2.0) * 0.15 + 0.85; // Subtler depth
    float shine = pow(plasma * pulse, 2.0) * 0.8; // Much dimmer shine

    // Particle system with shapes
    vec3 particleColor = vec3(0.0);
    const int numParticles = 20;
    for (int i = 0; i < numParticles; i++) {
        float angle = float(i) * 6.28318 / float(numParticles) + t * 0.15; // Faster swirl
        float radius = 3.0 + sin(float(i) + t * 0.4) * 2.0; // Wider orbit
        vec2 center = vec2(cos(angle), sin(angle)) * radius;
        vec2 relPos = p - center;
        float shape = particleShape(relPos, t + float(i) * 0.25);
        float glow = exp(-length(relPos) * 4.0) * 0.3; // Tighter, dimmer glow
        particleColor += vec3(0.7, 0.5, 0.6) * shape * 1.0 + vec3(0.5, 0.4, 0.6) * glow;
    }

    // Muted color palette
    vec3 color = vec3(0.0);
    color.r = sin(plasma * 10.0 + t * 2.0) * 0.3 + 0.3;
    color.g = cos(plasma * 8.0 + t * 1.8) * 0.3 + 0.3;
    color.b = sin(plasma * 9.0 + t * 1.5 + depth) * 0.3 + 0.3;

    color = clamp(color, 0.0, 1.0);
    color += vec3(shine) * vec3(0.6, 0.5, 0.4); // Very soft shine

    // Combine with particles
    color += particleColor;

    // Minimal bloom
    float bloom = pow(pulse + length(particleColor) * 0.05, 2.0) * 0.2; // Barely there
    color += vec3(bloom) * vec3(0.4, 0.5, 0.6);

    fragColor = vec4(clamp(color, 0.0, 1.0), 1.0);
}
