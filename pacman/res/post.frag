#version 450 core

/* Inputs */
layout(location = 0) in vec2 vs_uv;

/* Uniforms */
layout(location = 0) uniform sampler2D rendered_tex;
layout(location = 1) uniform float u_time;

/* Output attributes */
layout(location = 0) out vec4 fs_color;


/*!
 * \brief radial_distortion dsitorts radially
 */
vec2 radial_distortion(vec2 coord, float distort) {
    vec2 cc = coord - vec2(0.5);
    float dist = dot(cc, cc) * distort;
    return coord + cc * (1.0 - dist) * dist;
}

void main()
{
    vec4 out_color = texture(rendered_tex, radial_distortion(vs_uv, 0.5f));
    vec4 ca_color = texture(rendered_tex, radial_distortion(vs_uv, 0.49f));
    
    /* Vertical Lines */
    out_color = out_color * (max(0.7f, (int(gl_FragCoord.x) % 2)));
    out_color.gb = mix(out_color.gb, ca_color.gb, 0.5f);

    /* Flickering */
    out_color = out_color * (max(0.8f, fract(sin(u_time) * 5000.f)));

    fs_color = vec4(out_color.rgb, 1.f);
}

