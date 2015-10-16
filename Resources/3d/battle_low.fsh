
varying  vec2 v_texcoord;

uniform sampler2D u_texture_fieldbg;
uniform sampler2D u_texture_buildingbg;
//uniform vec4 u_color;
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
    /*
    float start = 10.0;
    float end = 20.0;
    float radio = 1.0;
    if (Distance <= start) {
    }
    else if (Distance > start && Distance < end) {
        radio = 1.0 - (Distance - start) / (end - start);
    } else {
        discard;
    }
    vec4 tc = texture2D(u_texture_normal, v_texcoord);
    vec3 normal = rotate_vector(v_posture, 2.0*(tc.rgb-vec3(0.5,0.5,0.5)));//CC_NormalMatrix * rotate_vector(v_posture, 2.0*(tc.rgb-vec3(0.5,0.5,0.5))); //旋转不再改变光照
//    vec4 shadow_color = texture2D(u_texture_shadow, v_texcoord);
     */
    vec4 fieldColor = texture2D(u_texture_fieldbg, v_texcoord);
    vec4 buildingColor = texture2D(u_texture_buildingbg, v_texcoord);
    gl_FragColor =  mix(fieldColor, buildingColor, buildingColor.a);//vec4(0.7*u_color.xyz + computeLighting(normal, vec3(0,0,1), vec3(1,1,1), 0.3), radio*0.65);
}