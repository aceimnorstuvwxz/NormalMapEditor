
varying  vec2 v_texcoord;

uniform sampler2D u_texture_building_animation;
uniform sampler2D u_texture_building_normalmap;
uniform sampler2D u_texture_be_attacked;
uniform sampler2D u_texture_be_cured;

uniform vec4 u_para_ambient_color;
uniform vec4 u_para_attacked_cured;//{is_attacked, is_cured, not used, not used}

/* 与DDConfig.h内配置要同步 */
#define DD_EXPAND_SHADOW 1.0
#define DD_EXPAND_NORMAL 0.5
#define DD_LIGHT_NUM 5
#define DD_ANIMATION_LINE_MAX 10 //每行有多少帧，从左上开始计数

uniform int u_building_animation_index;

//uniform float u_height;
uniform vec4 u_lights[DD_LIGHT_NUM];//{pos.x,pos.y,quantity,height}//这里的pox是光源->我的相对坐标，我们还需要就每个pixel的偏移做出修正。通过quantity取0来不显示阴影。
uniform vec4 u_lights_color[DD_LIGHT_NUM];//光源颜色

const float shininess = 16.0;
const float screenGamma = 2.2; // Assume the monitor is calibrated to the sRGB color space


vec3 computeLightingAst(vec3 normalVector, vec3 lightDirection, vec3 lightColor, float attenuation)
{
    float diffuse = dot(normalVector, lightDirection);
    vec3 diffuseColor = lightColor  * diffuse * attenuation;

    return diffuseColor;
}

vec3 computeLighting(vec3 ambientColor, vec3 diffuseColor, vec3 specularColor, vec3 normalVectorRaw, vec3 lightDirectionRaw, float lightRadio)// 注意这个lightDir是从Point到光源的！
{
    vec3  normal = normalize(normalVectorRaw);
    vec3 lightDir = normalize(lightDirectionRaw);

    float lambertian = max(dot(lightDir, normal), 0.0);
    /*
    float specular = 0.0;
    if (lambertian > 0.0) {
        vec3 viewDir = vec3(0.0,0.0,-1.0);

        // this is blinn phong
        vec3 halfDir = normalize(lightDir + viewDir);
        float specAngle = max(dot(halfDir, normal), 0.0);
        specular = pow(specAngle, shininess);
    }*/
    vec3 colorLinear = ambientColor + lambertian * diffuseColor * lightRadio ;//+ specular * specularColor * lightRadio;
    return colorLinear;
//    vec3 colorGammaCorrected = pow(colorLinear, vec3(1.0/screenGamma));

//    return colorGammaCorrected;
}

vec3 computeLightingPerLight(vec3 diffuseColor, vec3 specularColor, vec3 normal, vec3 lightDir, float lightRadio)// norlmal和lightDir已经归一化，注意这个lightDir是从Point到光源的！
{
    float lambertian = max(dot(lightDir, normal), 0.0);
    float specular = 0.0;

    if (lambertian > 0.0) {
        vec3 viewDir = vec3(0.0,0.0,1.0);

        // this is blinn phong
        vec3 halfDir = normalize(lightDir + viewDir);
        float specAngle = max(dot(halfDir, normal), 0.0);
        specular = pow(specAngle, shininess);
    }
    vec3 colorLinear = (lambertian * diffuseColor + specular * specularColor) * lightRadio;

    return colorLinear;
}

vec3 computeLightingForAllLights(vec3 ambientColor, vec3 diffuseColor, vec3 normal)
{
    vec3 colorRes = vec3(0.0,0.0,0.0);
    for (int i = 0; i < DD_LIGHT_NUM; i++) {
        vec4 lightPara = u_lights[i];
        if (lightPara.z > 0.0) { //通过light的quanty来判断是否点亮
            vec2 newTexcoord = vec2(v_texcoord.x, 1.0 - v_texcoord.y);
            vec3 lightRelativePos = vec3(lightPara.xy + (newTexcoord - vec2(0.5,0.5))*(DD_EXPAND_NORMAL*2.0 + 1.0), -lightPara.w);
            float len = length(lightRelativePos);
            float lightRadio = max((1.0 - len/lightPara.z), 0.0);
            vec3 lightDir = normalize(-lightRelativePos);

            colorRes = colorRes + computeLightingPerLight(diffuseColor, u_lights_color[i].rgb, normal, lightDir, lightRadio);
        }
    }
    vec3 colorLinear = ambientColor + colorRes;
    vec3 colorGammaCorrected = pow(colorLinear, vec3(1.0/screenGamma));
    return colorGammaCorrected;
}

vec2 calcRelativeTexcoordTexture(vec2 texcoord)
{

    texcoord = texcoord * (1.0/8.0);
    float y = (1.0/8.0) * float(u_building_animation_index/8 * 2);
    float x = (1.0/8.0) * float(u_building_animation_index - (u_building_animation_index/8)*8);
    texcoord = vec2(x,y) + texcoord;
    return texcoord;
}

vec2 calcRelativeTexcoordNormal(vec2 texcoord)
{

    texcoord = texcoord * (1.0/8.0);
    float y = (1.0/8.0) * float(u_building_animation_index/8 * 2 + 1);
    float x = (1.0/8.0) * float(u_building_animation_index - (u_building_animation_index/8)*8);
    texcoord = vec2(x,y) + texcoord;
    return texcoord;
}

vec3 fetchNormalVector(vec2 texcoord)
{

    vec4 texcol = texture2D(u_texture_building_normalmap, calcRelativeTexcoordNormal(texcoord));
    return normalize(texcol.rgb - vec3(0.5,0.5,0.5));
}

void main()
{

    vec4 resColor = vec4(0.0,0.0,0.0,0.0);
    vec4 tmpColor = resColor;
    //buildingAnimation TODO 动画
    tmpColor = texture2D(u_texture_building_animation, calcRelativeTexcoordTexture(v_texcoord));
    resColor = mix(resColor, tmpColor, tmpColor.a);

    //法向贴图 TODO通过光源计算
    vec3 normalVector = fetchNormalVector(v_texcoord);
    vec3 lightRet = computeLightingForAllLights(vec3(0.0,0.0,0.0), resColor.rgb, normalVector);
    resColor = vec4(lightRet, resColor.a);
    
    /*
    resColor = vec4(resColor.rgb * 0.5 + computeLightingAst(normalVector, vec3(-1,0,-1), vec3(1.0,1.0,1.0), 1.0)*0.5, resColor.a);
    vec3 lightingRet = computeLighting(vec3(0.1,0.1,0.1), resColor.rgb, vec3(1,1,1), normalVector, vec3(1,0,1), 1.0);
    resColor = vec4(lightingRet, resColor.a);*/

    /*
    tmpColor = texture2D(u_texture_building_normalmap, v_texcoord);
    resColor = mix(resColor, tmpColor, tmpColor.a);*/
    //被攻击
    tmpColor = texture2D(u_texture_be_attacked, v_texcoord);
    resColor = mix(resColor, tmpColor, tmpColor.a*u_para_attacked_cured.x);
    //被治疗
    tmpColor = texture2D(u_texture_be_cured, v_texcoord);
    resColor = mix(resColor, tmpColor, tmpColor.a*u_para_attacked_cured.y);

    gl_FragColor = resColor;// mix(fieldColor, buildingColor, buildingColor.a);//vec4(0.7*u_color.xyz + computeLighting(normal, vec3(0,0,1), vec3(1,1,1), 0.3), radio*0.65);
}