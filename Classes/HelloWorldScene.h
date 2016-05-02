#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Box2D/Box2D.h"

#include "GLESRender.hpp"

#define PTM_RATIO 32.0f



class HelloWorld : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    virtual void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags) override;


    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
    
    
    void update(float dt);
    
private:
    b2World* world;
    cocos2d::Size size;
    b2BodyDef bodyDef;
    b2FixtureDef fixtureDef;
    GLESDebugDraw* debugDraw;
    
    bool isshowsp;
    bool isshowdebugdraw;
    uint32 shownone;
    uint32 showall;
    void addbrick(cocos2d::Vec2 position);
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMoved(cocos2d::Touch* touch,cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch,cocos2d::Event* event);
    
    
};

#endif // __HELLOWORLD_SCENE_H__
