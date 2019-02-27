#version 450 core

/* Inputs */
layout(location = 0) in vec2 vs_uv;

/* Uniforms */
layout(location = 0) uniform sampler2D rendered_tex;

/* Output attributes */
layout(location = 0) out vec4 fs_color;

void main()
{
    vec4 out_color = texture(rendered_tex, vs_uv);
    
    /* Vertical Lines */
    out_color = out_color * (int(gl_FragCoord.x / 5) % 2);

    /* Distortion */

    fs_color = vec4(out_color.rgb, 1.f);
}

