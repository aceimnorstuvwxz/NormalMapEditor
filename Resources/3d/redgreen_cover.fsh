varying vec4 v_color;

uniform float u_alpha;
//uniform vec3 u_light_color;
//uniform vec3 u_light_direction;
//varying float Distance;

/*
vec3 computeLighting(vec3 normalVector, vec3 lightDirection, vec3 lightColor, float attenuation)
{
    float diffuse = dot(normalVector, lightDirection);
    vec3 diffuseColor = lightColor  * diffuse * attenuation;

    return diffuseColor;
}

vec4 inverse(vec4 color)
{
    return vec4(1.0 - color.r, 1.0 - color.g, 1.0 - color.b, color.a);
}
vec3 rotate_vector( vec4 quat, vec3 vec )
{
    return vec + 2.0 * cross( cross( vec, quat.xyz ) + quat.w * vec, quat.xyz );
}*/
void main()
{
//    vec4 fieldColor = texture2D(u_texture_fieldbg, v_texcoord);
//    vec4 buildingColor = texture2D(u_texture_buildingbg, v_texcoord);
    if (v_color.a == 0.0) {
        discard;
    }
    /*
    float radio = 1.0;
    vec2 pointcoord = 2.0*(gl_PointCoord - vec2(0.5,0.5));
    if (abs(pointcoord.x) > 0.9 || abs(pointcoord.y) > 0.9)
        radio = 2.0;*/
    gl_FragColor = v_color;// vec4(v_color.rgb, v_color.a*u_alpha*radio);//mix(fieldColor, buildingColor, buildingColor.a);
}