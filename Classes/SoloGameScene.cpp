﻿#include "SoloGameScene.h"
#include "AudioManager.h"
#include "MenuScene.h"
#include "DataManager.h"
#include <time.h>
#include "PauseDialog.h"
#include "LevelManager.h"
#include "AdviseFacebookDialog.h"
#include "ConfigLoader.h"

USING_NS_CC;

int SoloGameScene::QUEST_RESULTS_NUMBER = 0;


CCScene* SoloGameScene::scene()
{
    CCScene *scene = CCScene::create();
    SoloGameScene *layer = SoloGameScene::create();
    scene->addChild(layer);
    return scene;
}

bool SoloGameScene::init()
{
    if ( !CCLayerColor::initWithColor(ccc4(255, 255, 255, 255)) )
    {
        return false;
    }
	
	SoloGameScene::QUEST_RESULTS_NUMBER = CONF_INT(QUEST_RESULTS_NUMBER);

	m_curScore = DataManager::sharedDataManager()->GetSoloScore();
	m_curQuestionNumber = DataManager::sharedDataManager()->GetSoloQuestionNumber();
	m_curRightAnswer = -1;
	m_clockCounter = CONF_INT(SOLO_TIME_FOR_QUESTION);
	m_isRight = false;
	m_curDisableChoose = 4;
	m_isUsedInfiniteTime = false;

	MY_ADD_SPRITE(bg, "game_background.png", ccp(400, 640));
	
	MY_CREATE_SPRITE(defaultAvatar, "avatar.png", ccp(86, 1280-86));
	m_defaultAvatar = defaultAvatar;
	this->addChild(m_defaultAvatar);
	
	string photoPath = DataManager::sharedDataManager()->GetFbPhotoPath();
	if (photoPath.length() > 0)
	{
		MY_CREATE_SPRITE(fbAvatar, photoPath.c_str(), ccp(86, 1280-86));
		m_fbAvatar = fbAvatar;
		this->addChild(m_fbAvatar);
		m_fbAvatar->setScale((m_defaultAvatar->getContentSize().width - 7) / CONF_INT(AVATAR_SIZE));
	}
	
	string name = DataManager::sharedDataManager()->GetName();
	name = MY_LIMIT_STR(name, 22, "");
	MY_ADD_LABELTTF(lbName, name.c_str(), CONF_STR(FONT_NORMAL), 36, ccBLACK, ccp(12, 1280-195));
	m_lbName = lbName;
	m_lbName->setAnchorPoint(ANCHOR_LEFT);

	MY_ADD_SPRITE(score, "score.png", ccp(203, 1280-41));
	MY_ADD_LABELTTF(_lbScore, CCString::createWithFormat("%d", m_curScore)->getCString(), CONF_STR(FONT_NORMAL), 48, ccBLACK, ccp(240, 1280-46));
	_lbScore->setAnchorPoint(ANCHOR_LEFT);
	m_lbScore = _lbScore;

	MY_ADD_SPRITE(sprDiamond, "diamond.png", ccp(203, 1280-100));
	int diamond = DataManager::sharedDataManager()->GetDiamond();
	MY_ADD_LABELTTF(_lbDiamond, CCString::createWithFormat("%d", diamond)->getCString(), CONF_STR(FONT_NORMAL), 48, ccBLACK, ccp(240, 1280-100));
	_lbDiamond->setAnchorPoint(ANCHOR_LEFT);
	m_lbDiamond = _lbDiamond;
	
	MY_ADD_SPRITE(sprClock, "clock.png", ccp(400, 1280-200));

	MY_ADD_LABELTTF( _lbClock, CCString::createWithFormat("%d", (int)m_clockCounter)->getCString(), 
		CONF_STR(FONT_NORMAL), 64, ccBLACK, sprClock->getPosition());
	_lbClock->setAnchorPoint(ANCHOR_CENTER);
	m_lbClock = _lbClock;

	m_menu = CCMenu::create();
	m_menu->setPosition(CCPointZero);
	this->addChild(m_menu);

	MY_ADD_MENU_ITEM(itBack, "back.png", "backDown.png", "back.png", SoloGameScene::menuCallback, ccp(60, 60));
	m_itBack = itBack;

	//HELP

	MY_CREATE_MENU_ITEM(itHelp1, "help1.png", "help1Down.png", "help1Down.png", SoloGameScene::itHelp1Callback, ccp(740, 1280-60));
	m_menu->addChild(itHelp1);
	MY_CREATE_MENU_ITEM(itHelp2, "help2.png", "help2Down.png", "help2Down.png", SoloGameScene::itHelp2Callback, ccp(640, 1280-60));
	m_menu->addChild(itHelp2);
	MY_CREATE_MENU_ITEM(itHelp3, "help3.png", "help3Down.png", "help3Down.png", SoloGameScene::itHelp3Callback, ccp(540, 1280-60));
	m_menu->addChild(itHelp3);

	initQuestionItems();

	//RESULT //////////////////////////////////////////////////////////////////////////

	MY_ADD_SPRITE(_sprGameResult, "game_result.png", ccp(400, 640));
	m_sprGameResult = _sprGameResult;
	m_sprGameResult->setVisible(false);

	//+10 (-5)
	CCLabelTTF* changeScore = CCLabelTTF::create("", CONF_STR(FONT_NORMAL), 120);
	changeScore->setFontFillColor(ccBLACK);
	changeScore->setColor(ccBLACK);
	changeScore->setAnchorPoint(ANCHOR_RIGHT);
	changeScore->setPosition(ccp(380, 1280-880));
	changeScore->setTag(1);
	m_sprGameResult->addChild(changeScore);

	//Right, wrong
	CCLabelTTF* rightWrong = CCLabelTTF::create("", CONF_STR(FONT_NORMAL), 72);
	rightWrong->setFontFillColor(ccBLACK);
	rightWrong->setColor(ccBLACK);
	rightWrong->setAnchorPoint(ANCHOR_RIGHT);
	rightWrong->setPosition(ccp(380, 1280-722));
	rightWrong->setTag(2);
	m_sprGameResult->addChild(rightWrong);
	
	MY_CREATE_MENU_ITEM(itNext, "next.png", "nextDown.png", "nextDown.png", SoloGameScene::checkBeforeNextQuestion, ccp(400, 1280-879));
	itNext->setAnchorPoint(ANCHOR_LEFT);
	CCMenu* nextMenu = CCMenu::create(itNext, NULL);
	nextMenu->setPosition(CCPointZero);
	m_sprGameResult->addChild(nextMenu);

	nextQuestion(NULL);
	
	PLAY_BACKGROUND_MUSIC;
    return true;
}

void SoloGameScene::onEnterTransitionDidFinish()
{
	this->schedule(schedule_selector(SoloGameScene::scheduleClock), 0.5f);
	this->setKeypadEnabled(true);
}

void SoloGameScene::menuCallback(CCObject* pSender)
{
	PLAY_BUTTON_EFFECT;

	CCScene *pScene = CCTransitionFade::create(0.5, MenuScene::scene());
	CCDirector::sharedDirector()->replaceScene(pScene);
}

void SoloGameScene::keyBackClicked()
{
	menuCallback(NULL);
}

void SoloGameScene::answerCallback( CCObject* pSender )
{
	this->unschedule(schedule_selector(SoloGameScene::scheduleClock));
	
	CCMenuItemImage* it = (CCMenuItemImage*) pSender;
	it->selected();
	int tag = it->getTag();
	
	if (tag == m_curRightAnswer)
	{
		PLAY_GET_BOMB_EFFECT;
		m_curScore += DataManager::sharedDataManager()->GetSoloAddScore();
		m_isRight = true;
	}
	else
	{
		PLAY_OUT_PORP_EFFECT;
		m_curScore += DataManager::sharedDataManager()->GetSoloSubScore();
		if(m_curScore < 0) m_curScore = 0;
		m_isRight = false;
	}

	animationRightChoose();
	m_lbScore->setString(CCString::createWithFormat("%d", m_curScore)->getCString());
	
	DataManager::sharedDataManager()->SetSoloScore(m_curScore);
}

void SoloGameScene::checkBeforeNextQuestion( CCObject* pSender )
{
	//send to server
	CCArray* questResults = DataManager::sharedDataManager()->GetQuestResults();

	int isRight = (m_isRight == true) ? 1 : 0;
	int answerTime = CONF_INT(SOLO_TIME_FOR_QUESTION) - (int)(m_clockCounter);
	QuestionResult* result = new QuestionResult(m_curQuest->m_id, isRight, answerTime);
	questResults->addObject(result);
		
	if (questResults->count() >= QUEST_RESULTS_NUMBER - 1)
	{
		CCLOG("CLEAR--SEND--");
		//clear
		DataManager::sharedDataManager()->SetQuestResults(NULL);

		//send to server
		GameClientManager::sharedGameClientManager()->SendQuestResults(questResults);

	}
	else
	{
		//save again
		DataManager::sharedDataManager()->SetQuestResults(questResults);
	}

 	nextQuestion(NULL);
}

void SoloGameScene::nextQuestion(CCObject* pSender)
{
	//UI
	m_sprGameResult->setVisible(false);
	m_menu->setEnabled(true);
	for (int i = 0; i < 4; ++i)
	{
		m_itAnswers[i]->setEnabled(true);
		m_itAnswers[i]->setVisible(true);
		m_itAnswers[i]->unselected();
	}
	
	m_curQuestionNumber++;
	DataManager::sharedDataManager()->SetSoloQuestionNumber(m_curQuestionNumber);
	m_lbNumber->setString(CCString::createWithFormat("%d", m_curQuestionNumber)->getCString());
	
	initRandomLevel(m_curQuestionNumber);

	m_curDisableChoose = 0;
	m_isUsedInfiniteTime = false;
	m_clockCounter = CONF_INT(SOLO_TIME_FOR_QUESTION);
	this->schedule(schedule_selector(SoloGameScene::scheduleClock), 0.5f);
}

void SoloGameScene::initQuestionItems()
{
	int boxW = 774;

	CCScale9Sprite* quesContent = CCScale9Sprite::create("dialog.png");
	quesContent->setPosition(ccp(400, 1280-588));
	quesContent->setContentSize(CCSizeMake(boxW, 412));
	this->addChild(quesContent);

	CCScale9Sprite* quesNumber = CCScale9Sprite::create("dialog.png");
	quesNumber->setPosition(ccp(400, 1280-345));
	quesNumber->setContentSize(CCSizeMake(390, 128));
	this->addChild(quesNumber);

	
	MY_ADD_LABELTTF( _lbNumber, "", CONF_STR(FONT_NORMAL), 64, ccBLACK, ccp(400, 1280-340));
	m_lbNumber = _lbNumber;

	m_lbQuestion = CCLabelTTF::create("", 
		CONF_STR(FONT_NORMAL), 
		48, 
		CCSizeMake(9.0f * boxW / 10.0f, 0), 
		kCCTextAlignmentCenter, 
		kCCVerticalTextAlignmentCenter);
	m_lbQuestion->setFontFillColor(ccBLACK);
	m_lbQuestion->setColor(ccBLACK);
	m_lbQuestion->setPosition(ccp(400, 1280-600));
	this->addChild(m_lbQuestion);

	for (int i = 0; i < 4; ++i)
	{
		m_itAnswers[i] = CCMenuItemImage::create("answer.png", "answer_down.png", "answer_disable.png", this, menu_selector(SoloGameScene::answerCallback));
		m_itAnswers[i]->setPosition(ccp(400, 1280-843 - i*94));
		m_itAnswers[i]->setTag(i);
		m_menu->addChild(m_itAnswers[i]);
	}

	for (int i = 0; i < 4; ++i)
	{
		m_lbAnswers[i] = CCLabelTTF::create("", CONF_STR(FONT_NORMAL), 48);
		m_lbAnswers[i]->setFontFillColor(ccBLACK);
		m_lbAnswers[i]->setColor(ccBLACK);
		m_lbAnswers[i]->setAnchorPoint(ANCHOR_LEFT);
		m_lbAnswers[i]->setPosition(ccp(140, 1280-840 - i*94));
		this->addChild(m_lbAnswers[i]);
	}
}

void SoloGameScene::initRandomLevel(int number)
{
	m_curQuest = LevelManager::shareLevelLoader()->getLevelInRandom(number);
	
	if (m_curQuest != NULL)
	{
		m_lbQuestion->setString(m_curQuest->m_quest.c_str());
		m_curRightAnswer = m_curQuest->m_right; //0 -> 3
		
		m_lbAnswers[0]->setString(string("A. ").append(m_curQuest->m_arrChoice[0].c_str()).c_str());
		m_lbAnswers[1]->setString(string("B. ").append(m_curQuest->m_arrChoice[1].c_str()).c_str());
		m_lbAnswers[2]->setString(string("C. ").append(m_curQuest->m_arrChoice[2].c_str()).c_str());
		m_lbAnswers[3]->setString(string("D. ").append(m_curQuest->m_arrChoice[3].c_str()).c_str());
	} 
	else
	{
		CCMessageBox("Bạn đã hoàn thành game!", "Chúc mừng");
	}
}

void SoloGameScene::itHelp1Callback( CCObject* pSender )
{
	int diamond = DataManager::sharedDataManager()->GetDiamond();
	int diamond_for_exclusive = DataManager::sharedDataManager()->GetDiamondForExclusive();
	if (diamond < diamond_for_exclusive)
	{
		PLAY_OUT_PORP_EFFECT;
		m_lbDiamond->runAction(CCSequence::createWithTwoActions(
			CCScaleTo::create(0.2f, 1.5f),
			CCScaleTo::create(0.2f, 1.0f)
			));
		return;
	}
	
	if (m_curDisableChoose >= 2)
	{
		PLAY_OUT_PORP_EFFECT;
		CCMessageBox("Chỉ được bỏ tối đa 2 câu!", "Thông tin");
		return;
	}
	

	bool isOK = false;
	while(isOK == false)
	{
		int rd = (int)(CCRANDOM_0_1() * 4);
		
		if (rd != m_curRightAnswer)
		{
			if (m_itAnswers[rd]->isEnabled())
			{
				m_itAnswers[rd]->setEnabled(false);
				m_curDisableChoose++;
				
				DataManager::sharedDataManager()->AddDiamond(- diamond_for_exclusive);
				m_lbDiamond->setString(CCString::createWithFormat("%d", DataManager::sharedDataManager()->GetDiamond())->getCString());
				PLAY_GET_BOMB_EFFECT;
				isOK = true;
			}			
		}
	}
}

void SoloGameScene::itHelp2Callback( CCObject* pSender )
{
	if (m_isUsedInfiniteTime)
	{
		PLAY_OUT_PORP_EFFECT;
		m_lbClock->runAction(CCSequence::createWithTwoActions(
			CCScaleTo::create(0.2f, 1.5f),
			CCScaleTo::create(0.2f, 1.0f)
			));
		return;
	}
	
	int diamond = DataManager::sharedDataManager()->GetDiamond();
	int diamond_for_infite = DataManager::sharedDataManager()->GetDiamondForInfinite();
	if (diamond < diamond_for_infite)
	{
		PLAY_OUT_PORP_EFFECT;
		m_lbDiamond->runAction(CCSequence::createWithTwoActions(
			CCScaleTo::create(0.2f, 1.5f),
			CCScaleTo::create(0.2f, 1.0f)
			));
		return;
	}

	DataManager::sharedDataManager()->AddDiamond(- diamond_for_infite);
	m_lbDiamond->setString(CCString::createWithFormat("%d", DataManager::sharedDataManager()->GetDiamond())->getCString());
	PLAY_GET_BOMB_EFFECT;

	m_isUsedInfiniteTime = true;
	m_lbClock->setString("X");
	this->unschedule(schedule_selector(SoloGameScene::scheduleClock));
}

void SoloGameScene::itHelp3Callback( CCObject* pSender )
{
	int diamond = DataManager::sharedDataManager()->GetDiamond();
	int diamond_for_skip = DataManager::sharedDataManager()->GetDiamondForSkip();
	if (diamond < diamond_for_skip)
	{
		PLAY_OUT_PORP_EFFECT;
		m_lbDiamond->runAction(CCSequence::createWithTwoActions(
			CCScaleTo::create(0.2f, 1.5f),
			CCScaleTo::create(0.2f, 1.0f)
			));
		return;
	}

	DataManager::sharedDataManager()->AddDiamond(- diamond_for_skip);
	m_lbDiamond->setString(CCString::createWithFormat("%d", DataManager::sharedDataManager()->GetDiamond())->getCString());
	PLAY_GET_BOMB_EFFECT;

	nextQuestion(NULL);
}

void SoloGameScene::scheduleClock( float dt )
{
	m_clockCounter -= dt;
	m_clockCounter = (m_clockCounter < -1) ? -1 : m_clockCounter;

	CCString* s = CCString::createWithFormat("%d", (int)(m_clockCounter + 1));
	m_lbClock->setString(s->getCString());

	if (m_clockCounter == -1)
	{
		this->unschedule(schedule_selector(SoloGameScene::scheduleClock));

		m_curScore += DataManager::sharedDataManager()->GetSoloSubScore();
		if(m_curScore < 0) m_curScore = 0;
		m_isRight = false;

		PLAY_RING_EFFECT;
		animationRightChoose();
	}
}

void SoloGameScene::animationRightChoose()
{
	m_itBack->setEnabled(false);
	m_menu->setEnabled(false);

	m_itAnswers[m_curRightAnswer]->runAction(
			CCSequence::createWithTwoActions(
				CCRepeat::create(
					CCSequence::create(
						CCTintTo::create(0.15f, 0, 255, 0),
						CCTintTo::create(0.15f, 255, 255, 255),
						NULL
					),
					5
				),
				CCCallFunc::create(this, callfunc_selector(SoloGameScene::onFinishAnimationRightChoose))
			)
		);
}

void SoloGameScene::onFinishAnimationRightChoose()
{
	m_itBack->setEnabled(true);
	m_sprGameResult->setVisible(true);
	
	CCLabelTTF* changeScore = dynamic_cast<CCLabelTTF*>(m_sprGameResult->getChildByTag(1));
	CCLabelTTF* rightWrong = dynamic_cast<CCLabelTTF*>(m_sprGameResult->getChildByTag(2));

	if (NULL != changeScore)
	{
		if (m_isRight)
		{
			int solo_add_score = DataManager::sharedDataManager()->GetSoloAddScore();
			CCLOG("Add score: %d", solo_add_score);
			changeScore->setString(CCString::createWithFormat("+%d", solo_add_score)->getCString());
			rightWrong->setString("Đúng !!!");
			rightWrong->setFontFillColor(ccc3(0, 162, 255));
			rightWrong->setColor(ccc3(0, 162, 255));
		} 
		else
		{
			int solo_sub_score = DataManager::sharedDataManager()->GetSoloSubScore();
			CCLOG("Sub score: %d", solo_sub_score);
			changeScore->setString(CCString::createWithFormat("%d", solo_sub_score)->getCString());
			rightWrong->setString("Sai !!!");
			rightWrong->setFontFillColor(ccc3(193, 0, 0));
			rightWrong->setColor(ccc3(193, 0, 0));
		}
	}

	//animation

	CCPoint oldPoint = m_sprGameResult->getPosition();
	CCPoint newPoint = ccp(400, - m_sprGameResult->getContentSize().height / 2);

	m_sprGameResult->setPosition(newPoint);

	m_sprGameResult->runAction(CCEaseBackOut::create(CCMoveTo::create(0.5f, oldPoint)));
}

void SoloGameScene::refreshUserInfo()
{
	string photoPath = DataManager::sharedDataManager()->GetFbPhotoPath();
	if (photoPath.length() > 0)
	{
		MY_CREATE_SPRITE(fbAvatar, photoPath.c_str(), ccp(86, 1280-86));
		m_fbAvatar = fbAvatar;
		this->addChild(m_fbAvatar);
		m_fbAvatar->setScale((m_defaultAvatar->getContentSize().width - 7) / CONF_INT(AVATAR_SIZE));
	}

	string name = DataManager::sharedDataManager()->GetName();
	name = MY_LIMIT_STR(name, 22, "");
	m_lbName->setString(name.c_str());
}

void SoloGameScene::onOpenDialog()
{
	m_menu->setEnabled(false);
	//this->setTouchEnabled(false);
	this->setKeypadEnabled(false);
}

void SoloGameScene::onCloseDialog()
{
	m_menu->setEnabled(true);
	//this->setTouchEnabled(true);
	this->setKeypadEnabled(true);
}
