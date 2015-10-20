
varying vec3 v_normal;
varying vec4 v_color;

uniform int u_building_animation_index;

uniform vec2 u_light_position;

const float shininess = 16.0;
const float screenGamma = 2.2; // Assume the monitor is calibrated to the sRGB color space

uniform int u_show_state;


vec3 computeLighting(vec3 diffuseColor, vec3 specularColor, vec3 normal, vec3 lightDir, float lightRadio)// norlmal和lightDir已经归一化，注意这个lightDir是从Point到光源的！
{
    float lambertian = max(dot(lightDir, normal), 0.0);
    float specular = 0.0;

    /*
    if (lambertian > 0.0) {
        vec3 viewDir = vec3(0.0,0.0,1.0);

        // this is blinn phong
        vec3 halfDir = normalize(lightDir + viewDir);
        float specAngle = max(dot(halfDir, normal), 0.0);
        specular = pow(specAngle, shininess);
    }*/
    vec3 colorLinear = (lambertian * diffuseColor + specular * specularColor) * lightRadio;

    return colorLinear;
}


void main()
{
    vec3 col = computeLighting(v_color.rgb, vec3(1.0,1.0,1.0), normalize(v_normal), normalize(vec3(u_light_position,200.)), v_color.a);

    gl_FragColor = vec4(col.rgb,1.0);
}