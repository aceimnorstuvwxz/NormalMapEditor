#ifdef GL_ES
precision mediump float;
#endif

// Inputs
attribute vec4 a_positioin;
attribute vec4 a_edge;
attribute vec4 a_corner;


// Varyings
#ifdef GL_ES
varying mediump vec4 v_edge;
varying mediump vec4 v_corner;
#else
varying vec4 v_edge;
varying vec4 v_corner;
#endif

uniform float u_width;



void main()
{
    gl_Position = CC_MVPMatrix * a_positioin;
    gl_PointSize = u_width;
    v_edge = a_edge;
    v_corner = a_corner;
}