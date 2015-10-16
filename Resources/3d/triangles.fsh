
varying vec3 v_normal;
varying vec4 v_color;

uniform int u_show_state;

void main()
{
    gl_FragColor = v_color;
}