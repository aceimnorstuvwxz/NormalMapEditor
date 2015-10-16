#ifdef GL_ES
precision mediump float;
#endif

// Inputs
attribute vec4 a_positioin;
attribute vec4 a_color;

// Varyings
#ifdef GL_ES
varying mediump vec4 v_color;
#else
varying vec4 v_color;
#endif

uniform float u_scaler;
const float AGENT_WIDTH = 5.0*11.0+2.0; //offset部分不cover

void main()
{
    gl_Position = CC_MVPMatrix * a_positioin;
    gl_PointSize = AGENT_WIDTH *u_scaler;
    v_color = a_color;
}