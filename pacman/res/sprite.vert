#version 450 core
/* Per vertex attributes */
layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_uv;

/* Per instance attributes */
layout(location = 2) in vec2 ai_pos;
layout(location = 3) in vec2 ai_scale;
layout(location = 4) in vec3 ai_col;
layout(location = 5) in uint ai_tex_id;

/* Uniforms */
layout(binding = 0, std140) uniform Matrices
{
    mat4 view_matrix;
    mat4 projection_matrix;
};

/* Output attributes */
layout(location = 0) out vec2 vs_uv;
layout(location = 1) out vec3 vs_col;
layout(location = 2) out flat uint vs_tex_id;

void main()
{
    vs_uv = a_uv;
    vs_col = ai_col;
    vs_tex_id = ai_tex_id;
    gl_Position = projection_matrix * view_matrix * vec4(ai_scale * a_pos + ai_pos, 0., 1.);        
}
