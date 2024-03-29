#ifndef __MY_MACRO_H__
#define __MY_MACRO_H__

#include "cocos2d.h"
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

#define DIM_COLOR		ccc4(0, 0, 0, 128)
#define ccWHITE4		ccc4(255, 255, 255, 255)
#define ccBLACK4		ccc4(0, 0, 0, 255)


#define ANCHOR_TOP		ccp(0.5f, 1.0f)
#define ANCHOR_BOTTOM	ccp(0.5f, 0.0f)

#define ANCHOR_CENTER	ccp(0.5f, 0.5f)

#define ANCHOR_LEFT		ccp(0.0f, 0.5f)
#define ANCHOR_RIGHT	ccp(1.0f, 0.5f)



#define MY_LIMIT_VALUE(x, minX, maxX) \
{\
	if(x < minX) { x = minX; }\
	if(x > maxX) { x = maxX; }\
}


static string MY_LIMIT_STR(string srcString, int maxlen, string addString)
{
	if ((int)srcString.length() > maxlen)
	{
		srcString = srcString.substr(0, maxlen);
		srcString.append(addString);
	}
	return srcString;
}


static string MY_CUT_STR(string srcString, int maxWidth)
{
 	int len = srcString.length();

	int counter = maxWidth;
	while (counter <= len - maxWidth)
	{
		while(srcString.at(counter) != ' ')
		{
			if (counter < len - 1)
			{
				counter++;
			}
			else
			{
				break;
			}
		}

		if (counter < len)
		{
			srcString = srcString.insert(counter, "\n");
			counter += maxWidth;
		}
		else
		{
			break;
		}
	}

	return srcString;
}


#define MY_RELEASE(pObj) \
do\
{\
	if((pObj) != NULL) {\
		(pObj)->release();\
		delete (pObj);\
		(pObj) = NULL;\
	}\
} while(0)




//////////////////////////////////////////////////////////////////////////
//DataManager




//Create get/set function for integer value in DataManager
#define MY_GETSET_INT_FUNC(funName, keyword, defaultValue)\
public: int Get##funName(void) {\
	return CCUserDefault::sharedUserDefault()->getIntegerForKey(#keyword, defaultValue);\
}\
public: void Set##funName(int var) {\
	CCUserDefault::sharedUserDefault()->setIntegerForKey(#keyword, var);\
	CCUserDefault::sharedUserDefault()->flush();\
}\
public: void Add##funName(int addValue) {\
	int oldValue = Get##funName();\
	CCUserDefault::sharedUserDefault()->setIntegerForKey(#keyword, (oldValue + addValue));\
	CCUserDefault::sharedUserDefault()->flush();\
}



//Create get/set function for string value in DataManager
#define MY_GETSET_STR_FUNC(funName, keyword, defaultValue)\
public: std::string Get##funName(void) {\
	return CCUserDefault::sharedUserDefault()->getStringForKey(#keyword, defaultValue);\
}\
public: void Set##funName(std::string var) {\
	CCUserDefault::sharedUserDefault()->setStringForKey(#keyword, var);\
	CCUserDefault::sharedUserDefault()->flush();\
}




//Create get/set function for float value in DataManager
#define MY_GETSET_FLOAT_FUNC(funName, keyword, defaultValue)\
public: float Get##funName(void) {\
	return CCUserDefault::sharedUserDefault()->getFloatForKey(#keyword, defaultValue);\
}\
public: void Set##funName(float var) {\
	CCUserDefault::sharedUserDefault()->setFloatForKey(#keyword, var);\
	CCUserDefault::sharedUserDefault()->flush();\
}\
public: void Add##funName(float addValue) {\
	int oldValue = Get##funName();\
	CCUserDefault::sharedUserDefault()->setFloatForKey(#keyword, (oldValue + addValue));\
	CCUserDefault::sharedUserDefault()->flush();\
}




//Create get/set function for boolean value in DataManager
#define MY_GETSET_BOOL_FUNC(funName, keyword, defaultValue)\
public: bool Get##funName(void) {\
	return CCUserDefault::sharedUserDefault()->getBoolForKey(#keyword, defaultValue);\
}\
public: void Set##funName(bool var) {\
	CCUserDefault::sharedUserDefault()->setBoolForKey(#keyword, var);\
	CCUserDefault::sharedUserDefault()->flush();\
}




//DataManager
//////////////////////////////////////////////////////////////////////////
//UI Macro



//Add a CCMenuItemImage to this
#define MY_ADD_MENU_ITEM(itName, sprNormal, sprSelect, sprDisable, callbackFunc, position)\
	CCMenuItemImage* itName = CCMenuItemImage::create(sprNormal, sprSelect, sprDisable, this, menu_selector(callbackFunc));\
	itName->setPosition(position);\
	CCMenu* menu##itName = CCMenu::create(itName, NULL);\
	menu##itName->setPosition(CCPointZero);\
	this->addChild(menu##itName);

#define MY_CREATE_MENU_ITEM(itName, sprNormal, sprSelect, sprDisable, callbackFunc, position)\
	CCMenuItemImage* itName = CCMenuItemImage::create(sprNormal, sprSelect, sprDisable, this, menu_selector(callbackFunc));\
	itName->setPosition(position);



//Add a CCMenuItemLabel to this
#define MY_ADD_MENU_ITEM_LABEL(itName, str, fontName, fontSize, color, callbackFunc, anchor, position)\
	CCLabelTTF* lb##itName = CCLabelTTF::create(str, fontName, fontSize);\
	lb##itName->setFontFillColor(color);\
	lb##itName->setColor(color);\
	CCMenuItemLabel* itName = CCMenuItemLabel::create(lb##itName, this, menu_selector(callbackFunc));\
	itName->setAnchorPoint(anchor);\
	itName->setPosition(position);\
	CCMenu* menu##itName = CCMenu::create(itName, NULL);\
	menu##itName->setPosition(CCPointZero);\
	this->addChild(menu##itName);

#define MY_CREATE_MENU_ITEM_LABEL(itName, str, fontName, fontSize, color, callbackFunc, anchor, position)\
	CCLabelTTF* lb##itName = CCLabelTTF::create(str, fontName, fontSize);\
	lb##itName->setFontFillColor(color);\
	lb##itName->setColor(color);\
	CCMenuItemLabel* itName = CCMenuItemLabel::create(lb##itName, this, menu_selector(callbackFunc));\
	itName->setAnchorPoint(anchor);\
	itName->setPosition(position);




//Add a CCSprite to this
#define MY_ADD_SPRITE(sprName, sprFile, position)\
	CCSprite* sprName = CCSprite::create(sprFile);\
	sprName->setPosition(position);\
	this->addChild(sprName);

#define MY_CREATE_SPRITE(sprName, sprFile, position)\
	CCSprite* sprName = CCSprite::create(sprFile);\
	sprName->setPosition(position);






//Add a CCLabelTTF to this
#define MY_ADD_LABELTTF(lbName, str, fontName, fontSize, color, position)\
	CCLabelTTF* lbName = CCLabelTTF::create(str, fontName, fontSize);\
	lbName->setFontFillColor(color);\
	lbName->setColor(color);\
	lbName->setPosition(position);\
	this->addChild(lbName);

#define MY_CREATE_LABELTTF(lbName, str, fontName, fontSize, color, position)\
	CCLabelTTF* lbName = CCLabelTTF::create(str, fontName, fontSize);\
	lbName->setFontFillColor(color);\
	lbName->setColor(color);\
	lbName->setPosition(position);





//////////////////////////////////////////////////////////////////////////
//							NETWORK										//
//////////////////////////////////////////////////////////////////////////



//
// string url, CCObject* callbackObject, SEL_CallFuncND pSelector, string data 
//
#define MY_SEND_REQUEST( strUrl, callbackObject, callbackFunc, strData)\
do\
{\
	CCHttpRequest* request = new CCHttpRequest();\
	request->setRequestType(CCHttpRequest::kHttpPost);\
	request->setUrl(strUrl);\
	request->setResponseCallback(callbackObject, httpresponse_selector(callbackFunc));\
	CCLOG("SendRequest: \n%s", strData);\
	request->setRequestData(strData, strlen(strData));\
	CCHttpClient::getInstance()->send(request);\
	request->release();\
} while (0)



#endif //__MY_MACRO_H__
