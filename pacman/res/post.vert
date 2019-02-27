#version 450 core

/* Define the fullscreen quad directly in the shader since it's always like this */
const vec2[4] positions = {vec2(-1.f, -1.f), vec2(1.f, -1.f),
                           vec2(1.f, 1.f),   vec2(-1.f, 1.f)};

const vec2[4] uvs = {vec2(0.f, 0.f), vec2(1.f, 0.f),
                     vec2(1.f, 1.f), vec2(0.f, 1.f)};

const uint[6] indices = { 0, 1, 2, 2, 3, 0 };

layout(location = 0) out vec2 vs_uv;

void main()
{
    vs_uv = uvs[gl_VertexID];
    gl_Position = vec4(positions[gl_VertexID], 0.f, 1.f);
}
