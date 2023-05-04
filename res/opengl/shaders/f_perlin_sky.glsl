#version 330 core

// implementation based on https://www.shadertoy.com/view/3syfDc

uniform float u_Time;

vec2 u_Resolution;

in vec2 v_TexCoord;

out vec4 fragColor;

vec4 permute(vec4 x) {
    return mod(((x*34.0)+1.0) * x, 289.0);
}
vec2 fade(vec2 t) {
    return t*t*t * (t*(t*6.0-15.0) + 10.0);
}

float perlinNoise(vec2 P) 
{
  vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
  vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
  Pi = mod(Pi, 289.0); // To avoid truncation effects in permutation
  vec4 ix = Pi.xzxz;
  vec4 iy = Pi.yyww;
  vec4 fx = Pf.xzxz;
  vec4 fy = Pf.yyww;
  
  vec4 i = permute(permute(ix) + iy);
  vec4 gx = 2.0 * fract(i * 0.0243902439) - 1.0; // 1/41 = 0.024...
  vec4 gy = abs(gx) - 0.5;
  vec4 tx = floor(gx + 0.5);
  gx = gx - tx;
  
  vec2 g00 = vec2(gx.x,gy.x);
  vec2 g10 = vec2(gx.y,gy.y);
  vec2 g01 = vec2(gx.z,gy.z);
  vec2 g11 = vec2(gx.w,gy.w);
  
  vec4 norm = 1.79284291400159 - 0.85373472095314 * 
    vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11));
  g00 *= norm.x;
  g01 *= norm.y;
  g10 *= norm.z;
  g11 *= norm.w;
  
  float n00 = dot(g00, vec2(fx.x, fy.x));
  float n10 = dot(g10, vec2(fx.y, fy.y));
  float n01 = dot(g01, vec2(fx.z, fy.z));
  float n11 = dot(g11, vec2(fx.w, fy.w));
  
  vec2 fade_xy = fade(Pf.xy);
  vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
  
  float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
  return 2.3 * n_xy * (1 + sin(u_Time * 2.0) * 0.1);
}

void main()
{
    u_Resolution = vec2(1000, 680);
    vec2 offset = vec2((u_Time), 0.0) * 10;
    vec2 cord = v_TexCoord * 100. + offset;

    float b = perlinNoise(cord) + perlinNoise(cord*2.)
        + perlinNoise(cord * 4.) + perlinNoise(cord * 8.) / 4.;

    // perlin noise threshold and glow intensity
    b = (b - 1.2) * 1.2;

    gl_FragColor = vec4(b, b, b, 1.0);
}