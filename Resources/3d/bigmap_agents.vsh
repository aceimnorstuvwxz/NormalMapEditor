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

uniform int u_is_stroke; //是否是描边，通过画两边来描边！
uniform float u_scaler;
const float AGENT_WIDTH = 5.0;
const float STROKE_WIDTH = 2.0;




void main()
{
    gl_Position = CC_MVPMatrix * a_positioin;
    if (u_is_stroke > 0) {
        gl_PointSize = (AGENT_WIDTH + STROKE_WIDTH)*u_scaler;
        v_color = vec4(0.0,0.0,0.0,1.0);
    } else {
        gl_PointSize = AGENT_WIDTH * u_scaler;
        v_color = a_color;
    }
}