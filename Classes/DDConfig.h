

#ifndef DDConfig_hpp
#define DDConfig_hpp

#include "cocos2d.h"
class DDConfig
{
public:
    static cocos2d::Size getWinSize(){
        return cocos2d::Director::getInstance()->getWinSize();
        // TODO 对不同屏幕类型，进行向iphone 6的调整
    }

    static cocos2d::Vec2 relativePos(float x, float y)
    {
        auto size = getWinSize();
        return {size.width*x, size.height*y};
    }

    static float relativeScaler(float scaler)
    {
        return getWinSize().width/750 * scaler;
    }

    static float bigmapCubeWidth(){
        return getWinSize().height/20;
    }
    static float bigmapCubePixelScaler(){
        return bigmapCubeWidth()/(MINMAP_POS_OFFSET*2 + BATTLE_NUM * (2*MINMAP_POS_EXPAND+1));
    }
    constexpr static float BATTLE_FIELD_Y_RADIO = 750.f/1334;
    constexpr static float OTHER_FIELD_Y_RADIO = (1.0 - BATTLE_FIELD_Y_RADIO)/2;

    static cocos2d::Vec2 bigmapAreaCenter(){
        auto size = getWinSize();
        return {size.width/2, size.height*(BATTLE_FIELD_Y_RADIO + 1.5f*OTHER_FIELD_Y_RADIO)};
    }

    static cocos2d::Rect bigmapAreaRect(){
        auto size = getWinSize();
        return {0, size.height*(OTHER_FIELD_Y_RADIO+BATTLE_FIELD_Y_RADIO), size.width, size.height*OTHER_FIELD_Y_RADIO};
    }

    static cocos2d::Vec2 buildingAreaCenter()
    {
        auto size = getWinSize();
        return  {size.width * (0.624f), size.height * 0.176f};
    }

    static cocos2d::Rect buildingAreaRect()
    {
        auto size = getWinSize();
        cocos2d::Size mysize = {size.width * 0.752f, size.height * 0.067f};
        auto center = buildingAreaCenter();
        return {center.x - mysize.width*0.5f, center.y - mysize.height*0.5f, mysize.width, mysize.height};
    }
    static float buildingAreaSelectionWidth()
    {
        return getWinSize().width* (555.f/7.f/750.f);
    }


    constexpr static int BATTLE_NUM = 11;
    static float battleCubeWidth(){
        return battleAreaRect().size.width/BATTLE_NUM; 
    }

    static cocos2d::Vec2 battleAreaCenter(){
        auto size = getWinSize();
        return {size.width/2, size.height*(OTHER_FIELD_Y_RADIO  + 0.5f*BATTLE_FIELD_Y_RADIO)};
    }

    static cocos2d::Rect battleAreaRect()
    {
        auto size = getWinSize();
        return {0, size.height*OTHER_FIELD_Y_RADIO, size.width, size.height*BATTLE_FIELD_Y_RADIO};
    }

    static cocos2d::Size battleDialogButtonSize()
    {
        auto size = getWinSize();
        return  {size.height/5, size.height/15};
    }

    static float fontSizeRadio()
    {
        return getWinSize().height/1334.f;
    }


    ////bigmap////
    // 这些量用来算bigmap的图，与BATTLE_NUM，与shader_bigmap内中的AGENT_WIDTH等，与dungeonMapGen.py中的同名变量，要保持一致。
    constexpr static int BIGMAP_X_EXPAND = 15;// #X方向一共31格
    constexpr static int BIGMAP_Y_EXPAND = 5;//  #Y方向一共11格

    constexpr static int MINMAP_EXPAND = 5;// # 11*11
    constexpr static int MINMAP_POS_EXPAND = 2;// #每个agentPos绘制5*5像素面积
    constexpr static int MINMAP_POS_OFFSET = 3;// #各minmap之间的间隙


    /////battle field////
    constexpr static const float PERIOD_UNIT = 0.1f; //游戏内时间period单位的实际时间当量
    constexpr static const int NUM_LIGHT = 5; //*需要同时修改shader中的光源数量


    /*
     shadow为了支持阴影能够扩展到足够远的地方，所以它的expand会很大。
     为了节约显存和便于image的制作，普通的expand是较小的，也就是说普通的expand采样时，要对tex_coord进行缩放，因为texcoord会跟随expand最大的值，也就是shadow的expand大小。
     */
    constexpr static const int EXPAND_SHADOW = 1; //*要同时修改shader中的对应参数
    constexpr static const int EXPAND_NORMAL = 0.5; //*要同时修改shader中的对应参数
    constexpr static const int ANIMATION_LINE_MAX = 10; //动画纹理每行10帧

};


#endif /* DDConfig_hpp */
