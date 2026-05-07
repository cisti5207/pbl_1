#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec2 resolution;   // size texture (width, height)
uniform float radius;      // bán kính bo góc

out vec4 finalColor;

void main()
{
    vec2 pos = fragTexCoord * resolution;

    vec2 minCorner = vec2(radius, radius);
    vec2 maxCorner = resolution - vec2(radius, radius);

    vec2 closest = clamp(pos, minCorner, maxCorner);
    float dist = length(pos - closest);

    if (dist > radius)
        discard;

    finalColor = texture(texture0, fragTexCoord) * fragColor;
}