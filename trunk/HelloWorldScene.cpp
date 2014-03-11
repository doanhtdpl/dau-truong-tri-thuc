//
//  HelloWorldScene.cpp
//  EasyNDK-for-cocos2dx
//
//  Created by Amir Ali Jiwani on 23/02/2013.
//
//

#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "NDKHelper.h"
#include <string>
using namespace std;

using namespace cocos2d;
using namespace CocosDenshion;



CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                        "CloseNormal.png",
                                        "CloseSelected.png",
                                        this,
                                        menu_selector(HelloWorld::menuCloseCallback) );
    
    // add a "play" icon to simulate a data passing example
    CCMenuItemImage *pNextItem = CCMenuItemImage::create(
                                        "play.png",
                                        "play.png",
                                        this,
                                        menu_selector(HelloWorld::menuNextCallback)
                                            );
    
    pCloseItem->setPosition( ccp(CCDirector::sharedDirector()->getWinSize().width - 20, 20) );
    
    // Adjust Chartboost button
    pNextItem->setPosition( ccp(CCDirector::sharedDirector()->getWinSize().width - 80, 20) );

    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pCloseItem, pNextItem, NULL);
    pMenu->setPosition( CCPointZero );
    this->addChild(pMenu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    this->pLabel = CCLabelTTF::create("Hello World", "Thonburi", 34);

    // ask director the window size
    CCSize size = CCDirector::sharedDirector()->getWinSize();

    // position the label on the center of the screen
    this->pLabel->setPosition( ccp(size.width / 2, size.height - 20) );

    // add the label as a child to this layer
    this->addChild(this->pLabel, 1);

    // add "HelloWorld" splash screen"
    CCSprite* pSprite = CCSprite::create("HelloWorld.png");

    // position the sprite on the center of the screen
    pSprite->setPosition( ccp(size.width/2, size.height/2) );

    // add the sprite as a child to this layer
    this->addChild(pSprite, 0);
    
    // Adds a selector association to be called from native end
    NDKHelper::AddSelector("HelloWorldSelectors",
                           "ChangeLabelSelector",
                           callfuncND_selector(HelloWorld::ChangeLabelSelector),
                           this);
    
    return true;
}

// Selector that will change the label text
void HelloWorld::ChangeLabelSelector(CCNode* sender, void *data)
{
    this->pLabel->setString("World Changed!");
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    // Register a selector in a global space
    // So that when our native environment will call the method with the string
    // It can respond to the selector
    // Note : Group name is there for ease of removing the selectors
    NDKHelper::AddSelector("HelloWorldSelectors",
                           "SampleSelector",
                           callfuncND_selector(HelloWorld::SampleSelector),
                           this);
    
    // Making parameters for message to be passed to native language
    // For the ease of use, i am sending the method to be called name from C++
    CCDictionary* prms = CCDictionary::create();
    prms->setObject(CCString::create("SampleSelector"), "to_be_called");
    
    // Finally call the native method in current environment
    SendMessageWithParams(string("SampleSelector"), prms);
}

void HelloWorld::menuNextCallback(CCObject* pSender)
{
    // Register a selector in a global space
    // So that when our native environment will call the method with the string
    // It can respond to the selector
    // Note : Group name is there for ease of removing the selectors
    NDKHelper::AddSelector("HelloWorldSelectors",
                           "SampleSelectorWithData",
                           callfuncND_selector(HelloWorld::SampleSelectorWithData),
                           this);
    
    // Making parameters for message to be passed to native language
    // For the ease of use, i am sending the method to be called name from C++
    CCDictionary* prms = CCDictionary::create();
    prms->setObject(CCString::create("SampleSelectorWithData"), "to_be_called");
    
    // Finally call the native method in current environment
    SendMessageWithParams(string("SampleSelectorWithData"), prms);
}

// A selector that will respond to us, when native language will call it
void HelloWorld::SampleSelector(CCNode *sender, void *data)
{
    CCLog("Called from native environment");
}

// A selector with data that will respond to us, when native language will call it
void HelloWorld::SampleSelectorWithData(CCNode *sender, void *data)
{
    CCLog("Called from native environment");
    
    if (data != NULL)
    {
        CCDictionary *convertedData = (CCDictionary *)data;
        
        CCDictionary *sampleDictionary = (CCDictionary *)convertedData->objectForKey("sample_dictionary");
        CCArray *sampleArray = (CCArray *)sampleDictionary->objectForKey("sample_array");
        
        CCLog("Printing Array in Dictionary");
        
        for (int i = 0; i < sampleArray->count(); i++)
        {
            CCString* str = (CCString *)sampleArray->objectAtIndex(i);
            CCLOG("%s", str->getCString());
        }
            
        CCString* sampleInteger = (CCString*)sampleDictionary->objectForKey("sample_integer");
        
        CCString* sampleFloat = (CCString*)sampleDictionary->objectForKey("sample_float");
        
        CCString* sampleString = (CCString*)sampleDictionary->objectForKey("sample_string");
        
        CCLog("Printing Integer : %d", sampleInteger->intValue());
        CCLog("Printing Float : %.3f", sampleFloat->floatValue());
        CCLog("Printing String : %s", sampleString->getCString());
    }
}

HelloWorld::~HelloWorld()
{
    // Remove the associated selector group from the global space,
    // Because we are destroying this instance
    NDKHelper::RemoveSelectorsInGroup("HelloWorldSelectors");
}
