#include "LevelManager.h"
#include "cocos2d.h"
#include "jansson/jansson.h"

USING_NS_CC;
using namespace std;

LevelManager* LevelManager::s_instance = NULL;


LevelManager::LevelManager(void)
{
	//m_dict = CCDictionary::createWithContentsOfFile(LEVEL_FILE);
	//m_dict->retain();

	CCString* ss = CCString::createWithContentsOfFile("questions.json");
	string s = string(ss->getCString());
	CCLOG("READ: %s", ss->getCString());
	CCLOG("--------- BEGIN ---------");

	//get score from response
	json_t *questionList;
	json_error_t error;
	//json_t *questionList;

	questionList = json_loads(s.c_str(), strlen(s.c_str()), &error);
	//questionList = json_object_get(questionList, "list");

	//foreach to get all friend, insert to list
	int number = json_array_size(questionList);
	CCLOG("NUMBER = %d", number);

	CCArray* arrFriends = new CCArray();
	arrFriends->retain();

	for(int i = 0; i < number; i++)
	{
		json_t *question = json_array_get(questionList, i);

		json_t* quest;
		json_t* answers;
		json_t* right;

		quest = json_object_get(question, "quest");
		right = json_object_get(question, "right");

		CCLOG("Quest: %s", json_string_value(quest));
		CCLOG("Right: %d", (int)json_number_value(right));


		answers = json_object_get(question, "answers");
		for (int j = 0; j < 4; ++j)
		{
			json_t* answer = json_array_get(answers, j);
			CCLOG("Answer:%d: %s", j, json_string_value(answer));
		}
	}

	CCLOG("--------- END ---------");
}

LevelManager* LevelManager::shareLevelLoader()
{
	if (s_instance == NULL)
	{
		s_instance = new LevelManager();
	}
	
	return s_instance;
}

LevelData* LevelManager::getLevel( int level )
{
	string sLevel = CCString::createWithFormat("%d", level)->getCString();
	CCDictionary* question = (CCDictionary*)m_dict->objectForKey(sLevel);
	
	if (question != NULL)
	{
		string quest = ((CCString*)question->objectForKey("quest"))->getCString();
		string a = ((CCString*)question->objectForKey("a"))->getCString();
		string b = ((CCString*)question->objectForKey("b"))->getCString();
		string c = ((CCString*)question->objectForKey("c"))->getCString();
		string d = ((CCString*)question->objectForKey("d"))->getCString();
		int answer = ((CCString*)question->objectForKey("answer"))->intValue();

		LevelData* ld = LevelData::create(quest, a, b, c, d, answer);
		return ld;
	} 
	else
	{
		return NULL;
	}
}
