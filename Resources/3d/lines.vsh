#ifdef GL_ES
precision mediump float;
#endif

attribute vec4 a_positioin;

void main()
{
    gl_Position = CC_MVPMatrix * a_positioin;
}