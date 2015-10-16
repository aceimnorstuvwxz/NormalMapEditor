#ifdef GL_ES
precision mediump float;
#endif

attribute vec4 a_positioin;
attribute vec3 a_normal;
attribute vec4 a_color;

varying vec3 v_normal;
varying vec4 v_color;

void main()
{
    v_normal = a_normal;
    v_color = a_color;
    gl_Position = CC_MVPMatrix * a_positioin;
}