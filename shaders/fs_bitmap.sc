$input v_texcoord0

#include "bgfx_shader.sh"

SAMPLER2D(u_sampler, 0);

void main() {
    gl_FragColor = texture2D(u_sampler, v_texcoord0);
}
