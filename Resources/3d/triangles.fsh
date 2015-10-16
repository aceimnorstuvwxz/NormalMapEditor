
varying vec3 v_normal;
varying vec4 v_color;

uniform int u_building_animation_index;

uniform vec4 u_light;//{pos.x,pos.y,quantity,height}//这里的pox是光源->我的相对坐标，我们还需要就每个pixel的偏移做出修正。通过quantity取0来不显示阴影。

const float shininess = 16.0;
const float screenGamma = 2.2; // Assume the monitor is calibrated to the sRGB color space

uniform int u_show_state;

void main()
{
    gl_FragColor = v_color;
}