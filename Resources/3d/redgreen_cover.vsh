#ifdef GL_ES
precision mediump float;
#endif

// Inputs
attribute vec4 a_positioin;
attribute float a_state;

// Varyings
#ifdef GL_ES
varying mediump vec4 v_color;
#else
varying vec4 v_color;
#endif

uniform float u_width;



void main()
{
    gl_Position = CC_MVPMatrix * a_positioin;
    gl_PointSize = u_width*0.95;
    if (a_state < 0.5) {
        v_color = vec4(0.0,0.0,0.0,0.0);
    } else if (a_state < 1.5) {
        v_color = vec4(0.0,1.0,0.0,0.25);
    } else {
        v_color = vec4(1.0,0.0,0.0,0.25);
    }
}