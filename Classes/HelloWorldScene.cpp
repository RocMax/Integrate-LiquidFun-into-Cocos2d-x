#include "HelloWorldScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"




USING_NS_CC;

using namespace cocostudio::timeline;

HelloWorld::~HelloWorld(){
    delete world;
    delete debugDraw;
}

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    
    if ( !Layer::init() )
    {
        return false;
    }
    
    //auto rootNode = CSLoader::createNode("MainScene.csb");
    //addChild(rootNode);
    size=Director::getInstance()->getWinSize();
    world =new b2World(b2Vec2(0,-10.0));
    debugDraw=new GLESDebugDraw(PTM_RATIO);
    showall = 0;
    showall += b2Draw::e_shapeBit;
    showall += b2Draw::e_jointBit;
//    showall += b2Draw::e_aabbBit;
    showall += b2Draw::e_pairBit;
    showall += b2Draw::e_centerOfMassBit;
    showall +=b2Draw::e_particleBit;
    
    debugDraw->SetFlags(showall);
    shownone=0;
    world->SetDebugDraw(debugDraw);
    world->SetAllowSleeping(true);
    isshowsp=true;
    isshowdebugdraw=true;
    
    b2BodyDef groundBodyDef;
    
    b2Body* groundBody = world->CreateBody(&groundBodyDef);
    b2PolygonShape groundBox;
    
    //四面的外框
    groundBox.SetAsBox(size.width / PTM_RATIO/2, 1/PTM_RATIO, b2Vec2(size.width / PTM_RATIO/2,1/PTM_RATIO), 0);
    groundBody->CreateFixture(&groundBox, 0);
    groundBox.SetAsBox(size.width / PTM_RATIO/2, 1/PTM_RATIO, b2Vec2(size.width / PTM_RATIO/2,size.height/PTM_RATIO-1/PTM_RATIO), 0);
    groundBody->CreateFixture(&groundBox, 0);
    groundBox.SetAsBox(1/PTM_RATIO, size.height / PTM_RATIO/2, b2Vec2(1/PTM_RATIO,size.height / PTM_RATIO/2), 0);
    groundBody->CreateFixture(&groundBox, 0);
    groundBox.SetAsBox(1/PTM_RATIO,  size.height / PTM_RATIO/2, b2Vec2(size.width/PTM_RATIO-1/PTM_RATIO,size.height / PTM_RATIO/2), 0);
    groundBody->CreateFixture(&groundBox, 0);
    
    
    //按钮是否显示sprite
    auto showsplabel=Label::createWithSystemFont("Show Sprite", "Arial", 30);
//    showsplabel->setPosition(Vec2(size.width-150, size.height-50));
//    addChild(showsplabel);
    auto showspmenuitem=MenuItemLabel::create(showsplabel,
                                              [&](Ref *sender){
                                                  if (isshowsp) {
                                                      for(b2Body* b = world->GetBodyList(); b; b = b->GetNext())
                                                      {
                                                          if(b->GetUserData() != NULL)
                                                          {
                                                              auto sprite = (Sprite*)b->GetUserData();
                                                              sprite->setVisible(false);
                                                          }
                                                      }
                                                      isshowsp=false;
                                                  }
                                                  else {
                                                      for(b2Body* b = world->GetBodyList(); b; b = b->GetNext())
                                                      {
                                                          if(b->GetUserData() != NULL)
                                                          {
                                                              auto sprite = (Sprite*)b->GetUserData();
                                                              sprite->setVisible(true);
                                                          }
                                                      }
                                                      isshowsp=true;
                                                  }
                                                  
                                              });
    showspmenuitem->setPosition(Vec2(size.width-150,size.height-50));
    
    //是否显示debugdraw
    auto showddlabel=Label::createWithSystemFont("Show DebugDraw", "Arial", 30);
    auto showddmenuitem=MenuItemLabel::create(showddlabel,
                                              [&](Ref *sender){
                                                  if (isshowdebugdraw) {
                                                      debugDraw->SetFlags(shownone);
                                                      world->SetDebugDraw(debugDraw);
                                                      isshowdebugdraw=false;
                                                  }
                                                  else{
                                                      debugDraw->SetFlags(showall);
                                                      world->SetDebugDraw(debugDraw);
                                                      isshowdebugdraw=true;
                                                  }
                                              });
    showddmenuitem->setPosition(Vec2(size.width-150,size.height-100));
    auto btnmenu=Menu::create(showspmenuitem,showddmenuitem, NULL);
    btnmenu->setPosition(Vec2(0, 0));
    addChild(btnmenu);
    
    
    
    //添加粒子
    b2ParticleSystemDef particleSystemDef;
    
    particleSystemDef.radius = 8.0/PTM_RATIO;

    b2ParticleSystem * particleSystem = world->CreateParticleSystem(&particleSystemDef);
    
    auto pos = Vec2(size.width/2, size.height/2);

    b2PolygonShape* shape = new b2PolygonShape();
    shape->SetAsBox(size.width*0.25/PTM_RATIO, size.height*0.1/PTM_RATIO);
    
    b2ParticleGroupDef groupDef;
    groupDef.shape = shape;
    
    groupDef.flags = b2_waterParticle;
//    groupDef.color = b2ParticleColor(100,150,255,255);
    groupDef.position.Set(pos.x / PTM_RATIO, pos.y / PTM_RATIO);
    particleSystem->CreateParticleGroup(groupDef);
    
    
    auto touchListener=EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan= CC_CALLBACK_2(HelloWorld::onTouchBegan,this);
    touchListener->onTouchMoved=CC_CALLBACK_2(HelloWorld::onTouchMoved , this);
    touchListener->onTouchEnded=CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
    
    scheduleUpdate();
    
    return true;
}

void HelloWorld::update(float dt)
{
    
    int velocityIterations = 6;
    int positionIterations = 2;
    
    world->Step(dt, velocityIterations, positionIterations);
    
    
    for(b2Body* b = world->GetBodyList(); b; b = b->GetNext())
    {
        if(b->GetUserData() != NULL)
        {
            auto sprite = (Sprite*)b->GetUserData();
            sprite->setPosition(Vec2(b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO));
            sprite->setRotation( -1 * CC_RADIANS_TO_DEGREES(b->GetAngle()) );
            if (!isshowsp) {
                sprite->setVisible(false);
            }
        }
    }
}

void HelloWorld::addbrick(cocos2d::Vec2 position){
    auto bricksp = Sprite::create("brick.png");
    addChild(bricksp);
    
//    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(position.x/ PTM_RATIO, position.y/ PTM_RATIO);
    bodyDef.userData = bricksp;
    bodyDef.angularVelocity=5.0f;
    b2Body * body = world->CreateBody(&bodyDef);
    
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(0.5f, 0.5f);
    
//    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.friction = 0.3f;
    fixtureDef.density = 1.0;
    body->CreateFixture(&fixtureDef);

}

bool HelloWorld::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event){
    addbrick(touch->getLocation());
    return true;
}
void HelloWorld::onTouchMoved(cocos2d::Touch* touch,cocos2d::Event* event){
    
}
void HelloWorld::onTouchEnded(cocos2d::Touch* touch,cocos2d::Event* event){
    
}

void HelloWorld::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags){
    Layer::draw(renderer, transform, _transformUpdated);
    Director* director = Director::getInstance();
    
    GL::enableVertexAttribs( cocos2d::GL::VERTEX_ATTRIB_FLAG_POSITION );
    director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    world->DrawDebugData();
    director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}


