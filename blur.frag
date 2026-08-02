uniform sampler2D texture;
uniform vec2 u_direction; // (1,0) or (0,1)
uniform float u_radius;

void main()
{
    vec2 uv = gl_TexCoord[0].xy;
    vec4 sum = vec4(0.0);

    for (float i = -u_radius; i <= u_radius; i++)
    {
        sum += texture2D(texture, uv + u_direction * i * 0.002);
    }

    gl_FragColor = sum / (u_radius * 2.0 + 1.0);
}
