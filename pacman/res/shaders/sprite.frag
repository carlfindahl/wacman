#version 450 core
/* Input attributes */
layout(location = 0) in vec2 vs_uv;
layout(location = 1) in vec3 vs_col;
layout(location = 2) in flat uint vs_tex_id;
layout(location = 3) in flat uint vs_frame_no;

/* Uniforms */
layout(location = 0) uniform sampler2DArray sprites[16];

/* Output attributes */
layout(location = 0) out vec4 fs_color;

/*!
 * \brief sample texture parses the texture id and samples the correct layer and texture
 */ 
vec4 sample_texture()
{
    return texture(sprites[vs_tex_id], vec3(vs_uv, float(vs_frame_no)));
}

void main()
{
    fs_color = vec4(vs_col, 1.) * sample_texture();    
}
