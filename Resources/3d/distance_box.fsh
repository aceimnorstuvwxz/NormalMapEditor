varying vec4 v_edge;
varying vec4 v_corner;

uniform float u_alpha;
uniform vec4 u_color;

#define OFFSET_HARD 0.1
#define OFFSET_SOFT 0.2

void main()
{

    vec2 pointcoord = vec2(0.0,0.0);//2.0*(gl_PointCoord - vec2(0.5,0.5));
    vec4 resColor = vec4(0.0,0.0,0.0,0.0);

    // Edge

    //上
    resColor = mix(resColor, u_color, step(1.0-OFFSET_HARD, (-pointcoord.y)*v_edge.x));
    //下
    resColor = mix(resColor, u_color, step(1.0-OFFSET_HARD, (pointcoord.y)*v_edge.z));
    //右
    resColor = mix(resColor, u_color, step(1.0-OFFSET_HARD, pointcoord.x*v_edge.y));
    //左
    resColor = mix(resColor, u_color, step(1.0-OFFSET_HARD, (-pointcoord.x)*v_edge.w));

    // 上右
    resColor = mix(resColor, u_color, step(1.0-OFFSET_HARD, -pointcoord.y) * step(1.0-OFFSET_HARD, pointcoord.x) * v_corner.x);
    // 右下
    resColor = mix(resColor, u_color, step(1.0-OFFSET_HARD, pointcoord.x) * step(1.0-OFFSET_HARD, (pointcoord.y)) * v_corner.y);
    // 下左
    resColor = mix(resColor, u_color, step(1.0-OFFSET_HARD, (pointcoord.y)) * step(1.0-OFFSET_HARD, (-pointcoord.x)) * v_corner.z);
    // 左上
    resColor = mix(resColor, u_color, step(1.0-OFFSET_HARD, (-pointcoord.x)) * step(1.0-OFFSET_HARD, -pointcoord.y) * v_corner.w);



    gl_FragColor =  vec4(resColor.rgb, resColor.a * u_alpha);
}