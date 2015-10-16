
varying  vec2 v_texcoord;

uniform sampler2D u_texture_shadow;
uniform sampler2D u_texture_action_scope_line;
uniform sampler2D u_texture_placement;
uniform sampler2D u_texture_selection;
uniform sampler2D u_texture_boss;
uniform sampler2D u_texture_low_blood;

uniform vec4 u_para_action_scope_line;//top,right,below,left
uniform vec4 u_para_placement;//{r,g,b,isshow}
uniform vec4 u_para_selectioin_boss_lowblood;//{isselect,isboss,islowblood, not used}

#define DD_EXPAND_SHADOW 1.0
#define DD_EXPAND_NORMAL 0.5
#define DD_LIGHT_NUM 5

uniform vec4 u_lights[DD_LIGHT_NUM];//{pos.x,pos.y,quantity,height}//这里的pox是光源->我的相对坐标，我们还需要就每个pixel的偏移做出修正。通过quantity取0来不显示阴影。

vec4 computeShadow(vec2 relativePosition, float quantity, float height)
{
    vec2 newTexcoord = vec2(v_texcoord.x, 1.0 - v_texcoord.y);
    vec3 lightRelativePos = vec3(relativePosition + (newTexcoord - vec2(0.5,0.5))*(DD_EXPAND_SHADOW*2.0 + 1.0), -height);// 算精细的距离，而不是到cube中心的距离。
    relativePosition.x = -relativePosition.x; // 由于texture坐标系与普通笛卡尔坐标系的差异。
    float detailLen = length(lightRelativePos);
    float len = length(relativePosition);
    float alphaRadio = min(detailLen, quantity) / quantity;//越远 阴影越来越弱，超过范围的无阴影。
    float shadowRadio = height/len;
    if (len <= 0.001) {
        return vec4(0.0,0.0,0.0,0.0);//在建筑头顶没投影
    } else {
        float cosf = relativePosition.y / len;
        float sinf = relativePosition.x / len;
        vec2 centeredTexcoord = vec2(v_texcoord.x - 0.5, v_texcoord.y - 0.5);
        vec2 newCenteredCoord = vec2(centeredTexcoord.x * cosf - centeredTexcoord.y * sinf,
                                centeredTexcoord.y * cosf + centeredTexcoord.x * sinf);
        newCenteredCoord.y = shadowRadio*newCenteredCoord.y;
        vec2 newRealCoord = newCenteredCoord + vec2(0.5,0.5);
        vec4 textCol = texture2D(u_texture_shadow, newRealCoord);
        return vec4(textCol.rgb, textCol.a * (1.0- alphaRadio));
    }
}
void main()
{
    vec4 resColor = vec4(0.0,0.0,0.0,0.0);
    vec4 tmpColor = resColor;
    float normalTextureRadio = (1.0+2.0*DD_EXPAND_SHADOW)/(1.0+2.0*DD_EXPAND_NORMAL);
    vec2 texcoord_normal = (v_texcoord - vec2(0.5,0.5)) * normalTextureRadio + vec2(0.5,0.5);
    //低血光环
    tmpColor = texture2D(u_texture_low_blood, texcoord_normal);
    resColor = mix(resColor, tmpColor, tmpColor.a*u_para_selectioin_boss_lowblood.z);
    //BOSS光环
    tmpColor = texture2D(u_texture_boss, texcoord_normal);
    resColor = mix(resColor, tmpColor, tmpColor.a*u_para_selectioin_boss_lowblood.y);
    //选中光环
    tmpColor = texture2D(u_texture_selection, texcoord_normal);
    resColor = mix(resColor, tmpColor, tmpColor.a*u_para_selectioin_boss_lowblood.x);
    //建筑可放性
    tmpColor = texture2D(u_texture_placement, texcoord_normal);
    tmpColor = vec4(u_para_placement.rgb*tmpColor.r, tmpColor.a*u_para_placement.a);
    resColor = mix(resColor, tmpColor, tmpColor.a);
    //机能范围线-上
    tmpColor = texture2D(u_texture_action_scope_line, texcoord_normal);
    resColor = mix(resColor, tmpColor, tmpColor.a*u_para_action_scope_line.r);
    //机能范围线-右
    tmpColor = texture2D(u_texture_action_scope_line, vec2(texcoord_normal.y, texcoord_normal.x));
    resColor = mix(resColor, tmpColor, tmpColor.a*u_para_action_scope_line.g);
    //机能范围线-下
    tmpColor = texture2D(u_texture_action_scope_line, vec2(1.0-texcoord_normal.x, 1.0 - texcoord_normal.y));
    resColor = mix(resColor, tmpColor, tmpColor.a*u_para_action_scope_line.b);
    //机能范围线-左
    tmpColor = texture2D(u_texture_action_scope_line, vec2(1.0-texcoord_normal.y, 1.0-texcoord_normal.x));
    resColor = mix(resColor, tmpColor, tmpColor.a*u_para_action_scope_line.a);

    //阴影
    for (int i = 0; i < DD_LIGHT_NUM; i++) {
        vec4 lightPara = u_lights[i];
        tmpColor = computeShadow(lightPara.xy, lightPara.z, lightPara.w);
        resColor = mix(resColor, tmpColor, tmpColor.a);
    }

    gl_FragColor = resColor;// mix(fieldColor, buildingColor, buildingColor.a);//vec4(0.7*u_color.xyz + computeLighting(normal, vec3(0,0,1), vec3(1,1,1), 0.3), radio*0.65);
}