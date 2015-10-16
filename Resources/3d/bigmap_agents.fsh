
varying vec4 v_color;

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
//    vec4 fieldColor = texture2D(u_texture_fieldbg, v_texcoord);
//    vec4 buildingColor = texture2D(u_texture_buildingbg, v_texcoord);
    gl_FragColor =  v_color;//mix(fieldColor, buildingColor, buildingColor.a);
}