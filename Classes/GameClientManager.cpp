#include "GameClientManager.h"
#include "DataManager.h"

GameClientManager* GameClientManager::s_instance = NULL;

string GameClientManager::s_urlProfile = string("");
string GameClientManager::s_urlDevice = string("");
string GameClientManager::s_urlFriend = string("");
string GameClientManager::s_urlScore = string("");

//

void GameClientManager::setUrls( string urlProfile, string urlDevice, string urlFriend, string urlScore )
{
	CCLOG("SET URL");
	GameClientManager::s_urlProfile = string(urlProfile);
	GameClientManager::s_urlDevice = string(urlDevice);
	GameClientManager::s_urlFriend = string(urlFriend);
	GameClientManager::s_urlScore = string(urlScore);
}

//


void GameClientManager::sendRequest( const char* url, CCObject* callbackObject, SEL_HttpResponse pSelector, const char* data )
{
	CCHttpRequest* request = new CCHttpRequest();
	request->setRequestType(CCHttpRequest::kHttpPost);

	request->setUrl(url);
	request->setResponseCallback(callbackObject, (SEL_CallFuncND)pSelector);

	CCLOG("Data: %s", data);
	std::string s = encodeBeforeSend(data);
	request->setRequestData(data, strlen(data));

	CCHttpClient::getInstance()->send(request);
	request->release();
}


//////////////////////////////////////////////////////////////////////////

void GameClientManager::sendUserProfile( std::string fbId, std::string fbName, std::string email )
{
	CCAssert(s_urlProfile.length() > 0, "Not set s_urlProfile yet");
	CCHttpRequest* request = new CCHttpRequest();
	request->setRequestType(CCHttpRequest::kHttpPost);

	request->setUrl(s_urlProfile.c_str());
	request->setTag("sendPlayerFbProfile");
	request->setResponseCallback(this, httpresponse_selector(GameClientManager::_onSendUserProfileCompleted));

	// write the post data
	CCString* strData = CCString::createWithFormat("data={ \"fbId\": \"%s\", \"fbName\": \"%s\", \"email\": \"%s\" }&method=set",
		fbId.c_str(),
		fbName.c_str(),
		email.c_str());

	CCLOG("Data: %s", strData->getCString());
	std::string s = encodeBeforeSend(strData->getCString());
	request->setRequestData(s.c_str(), strlen(s.c_str()));


	CCHttpClient::getInstance()->send(request);
	request->release();
}

void GameClientManager::_onSendUserProfileCompleted( CCHttpClient *sender, CCHttpResponse *response )
{
	if (!response || m_clientDelegate == NULL)
	{
		return;
	}

	//Show info
	CCLOG("------- BEGIN [%s] -------", response->getHttpRequest()->getTag());
	CCLOG("Status: [%i]", response->getResponseCode());

	if (!response->isSucceed())
	{
		CCLOG("Request failed: [%s]", response->getErrorBuffer());
		m_clientDelegate->onSendUserProfileCompleted(false);
	}
	else
	{
		std::vector<char> *buffer = response->getResponseData();
		std::string str(buffer->begin(), buffer->end());
		
		CCLOG("Content: [%s]", str.c_str());
		m_clientDelegate->onSendUserProfileCompleted(true);
	}

	CCLOG("------- END [%s] -------", response->getHttpRequest()->getTag());
}

void GameClientManager::getUserProfile(std::string fbId )
{
	CCAssert(s_urlProfile.length() > 0, "Not set s_urlProfile yet");
	CCHttpRequest* request = new CCHttpRequest();
	request->setRequestType(CCHttpRequest::kHttpPost);

	request->setUrl(s_urlProfile.c_str());
	request->setTag("getPlayerFbProfile");
	request->setResponseCallback(this, httpresponse_selector(GameClientManager::_onGetUserProfileCompleted));

	// write the post data
	CCString* strData = CCString::createWithFormat("data={ \"fbId\": \"%s\" }&method=get",
		fbId.c_str()
		);


	CCLOG("Data: %s", strData->getCString());
	std::string s = encodeBeforeSend(strData->getCString());
	request->setRequestData(s.c_str(), strlen(s.c_str()));


	CCHttpClient::getInstance()->send(request);
	request->release();
}

void GameClientManager::_onGetUserProfileCompleted( CCHttpClient *sender, CCHttpResponse *response )
{
	if (!response || m_clientDelegate == NULL)
	{
		return;
	}

	//Show info
	CCLOG("------- BEGIN [%s] -------", response->getHttpRequest()->getTag());
	CCLOG("Status: [%i]", response->getResponseCode());

	if (!response->isSucceed())
	{
		CCLOG("Request failed: [%s]", response->getErrorBuffer());
		m_clientDelegate->onGetUserProfileCompleted(false, NULL);
	}
	else
	{
		std::vector<char> *buffer = response->getResponseData();
		std::string str(buffer->begin(), buffer->end());

		CCLOG("Content: [%s]", str.c_str());

		//get score from response
		json_t *root;
		json_error_t error;
		json_t *fbId;
		json_t *fbName;
		json_t *email;
		//json_t *score;


		root = json_loads(str.c_str(), strlen(str.c_str()), &error);
		fbId = json_object_get(root, "fbId");
		fbName = json_object_get(root, "fbName");
		email = json_object_get(root, "email");
		//score = json_object_get(root, "score");

		UserProfile* acc = new UserProfile(json_string_value(fbId), json_string_value(fbName), std::string(json_string_value(email)), -1);

		m_clientDelegate->onGetUserProfileCompleted(true, acc);

	}

	CCLOG("------- END [%s] -------", response->getHttpRequest()->getTag());
}

//

void GameClientManager::sendFriendList(std::string fbId, CCArray* arrFriends )
{
	CCAssert(s_urlFriend.length() > 0, "Not set s_urlFriend yet");
	CCHttpRequest* request = new CCHttpRequest();
	request->setRequestType(CCHttpRequest::kHttpPost);

	request->setUrl(s_urlFriend.c_str());
	request->setTag("sendFriendList");
	request->setResponseCallback(this, httpresponse_selector(GameClientManager::_onSendFriendListCompleted));

	// write the post data

	//parse arrFriendList to json
	std::string strFriendList = std::string("");
	int count = arrFriends->count();

	for (int i = 0; i < count; ++i)
	{
		UserProfile* fbFriend = (UserProfile*)arrFriends->objectAtIndex(i);

		strFriendList.append("\"");
		strFriendList.append(fbFriend->m_fbId);
		strFriendList.append("\"");
		if (i != count - 1)
		{
			strFriendList.append(",");
		}
	}	

	CCString* strData = CCString::createWithFormat("data={ \"fbId\": \"%s\", \"list\": [%s]}&method=set",
		fbId.c_str(),
		strFriendList.c_str());


	CCLOG("Data: %s", strData->getCString());
	std::string s = encodeBeforeSend(strData->getCString());
	request->setRequestData(s.c_str(), strlen(s.c_str()));


	CCHttpClient::getInstance()->send(request);
	request->release();
}

void GameClientManager::_onSendFriendListCompleted( CCHttpClient *sender, CCHttpResponse *response )
{
	if (!response || m_clientDelegate == NULL)
	{
		return;
	}

	//Show info
	CCLOG("------- BEGIN [%s] -------", response->getHttpRequest()->getTag());
	CCLOG("Status: [%i]", response->getResponseCode());

	if (!response->isSucceed())
	{
		CCLOG("Request failed: [%s]", response->getErrorBuffer());
		m_clientDelegate->onSendFriendListCompleted(false);
	}
	else
	{
		std::vector<char> *buffer = response->getResponseData();
		std::string str(buffer->begin(), buffer->end());

		CCLOG("Content: [%s]", str.c_str());
		m_clientDelegate->onSendFriendListCompleted(true);		
	}

	CCLOG("------- END [%s] -------", response->getHttpRequest()->getTag());
}

void GameClientManager::getFriendList( std::string appId, std::string fbId )
{
	CCAssert(s_urlFriend.length() > 0, "Not set s_urlFriend yet");
	CCHttpRequest* request = new CCHttpRequest();
	request->setRequestType(CCHttpRequest::kHttpPost);

	request->setUrl(s_urlFriend.c_str());
	request->setTag("getFriendList");
	request->setResponseCallback(this, httpresponse_selector(GameClientManager::_onGetFriendListCompleted));

	// write the post data
	CCString* strData = CCString::createWithFormat("data={ \"appId\": \"%s\", \"fbId\": \"%s\" }&method=get",
		appId.c_str(),
		fbId.c_str()
		);


	CCLOG("Data: %s", strData->getCString());
	std::string s = encodeBeforeSend(strData->getCString());
	request->setRequestData(s.c_str(), strlen(s.c_str()));


	CCHttpClient::getInstance()->send(request);
	request->release();
}

void GameClientManager::_onGetFriendListCompleted( CCHttpClient *sender, CCHttpResponse *response )
{
	if (!response || m_clientDelegate == NULL)
	{
		return;
	}

	//Show info
	CCLOG("------- BEGIN [%s] -------", response->getHttpRequest()->getTag());
	CCLOG("Status: [%i]", response->getResponseCode());

	if (!response->isSucceed())
	{
		CCLOG("Request failed: [%s]", response->getErrorBuffer());
		m_clientDelegate->onGetFriendListCompleted(false, NULL);
	}
	else
	{
		std::vector<char> *buffer = response->getResponseData();
		std::string str(buffer->begin(), buffer->end());

		CCLOG("Content: [%s]", str.c_str());

		//get score from response
		json_t *root;
		json_error_t error;
		json_t *friendList;

		root = json_loads(str.c_str(), strlen(str.c_str()), &error);
		friendList = json_object_get(root, "list");

		//foreach to get all friend, insert to list
		int count = json_array_size(friendList);
		CCArray* arrFriends = CCArray::create();
		//arrFriends->retain();

		for(int i = 0; i < count; i++)
		{
			json_t *fbFriend = json_array_get(friendList, i);

			json_t* fbId;
			json_t* fbName;
			//json_t* email;
			json_t* score;


			fbId = json_object_get(fbFriend, "fbId");
			fbName = json_object_get(fbFriend, "fbName");
			//email = json_object_get(fbFriend, "email");
			score = json_object_get(fbFriend, "score");

			UserProfile* acc = new UserProfile(json_string_value(fbId), json_string_value(fbName), std::string(), (int)atof(json_string_value(score)));
			arrFriends->addObject(acc);
		}

		//GameClientManager::SortFriendScore(arrFriends);
		m_clientDelegate->onGetFriendListCompleted(true, arrFriends);
	}

	CCLOG("------- END [%s] -------", response->getHttpRequest()->getTag());
}

//

void GameClientManager::sendDeviceProfile( std::string fbId, std::string deviceId, std::string deviceToken, std::string deviceConfig, std::string devicePhoneNumber )
{
	CCAssert(s_urlDevice.length() > 0, "Not set s_urlDevice yet");
	CCHttpRequest* request = new CCHttpRequest();
	request->setRequestType(CCHttpRequest::kHttpGet);

	request->setUrl(s_urlDevice.c_str());
	request->setTag("sendDeviceProfile");
	request->setResponseCallback(this, httpresponse_selector(GameClientManager::_onSendDeviceProfileCompleted));

	// write the post data
	CCString* strData = CCString::createWithFormat("data={ \"fbId\": \"%s\", \"deviceId\": \"%s\", \"deviceToken\": \"%s\", \"deviceConfig\": \"%s\", \"devicePhoneNumber\": \"%s\" }&method=set",
		fbId.c_str(),
		deviceId.c_str(),
		deviceToken.c_str(),
		deviceConfig.c_str(),
		devicePhoneNumber.c_str()
		);


	CCLOG("Data: %s", strData->getCString());
	std::string s = encodeBeforeSend(strData->getCString());
	request->setRequestData(s.c_str(), strlen(s.c_str()));


	CCHttpClient::getInstance()->send(request);
	request->release();
}

void GameClientManager::_onSendDeviceProfileCompleted( CCHttpClient *sender, CCHttpResponse *response )
{
	if (!response || m_clientDelegate == NULL)
	{
		return;
	}

	//Show info
	CCLOG("------- BEGIN [%s] -------", response->getHttpRequest()->getTag());
	CCLOG("Status: [%i]", response->getResponseCode());

	if (!response->isSucceed())
	{
		CCLOG("Request failed: [%s]", response->getErrorBuffer());
		m_clientDelegate->onSendDeviceProfileCompleted(false);
	}
	else
	{
		std::vector<char> *buffer = response->getResponseData();
		std::string str(buffer->begin(), buffer->end());

		CCLOG("Content: [%s]", str.c_str());
		m_clientDelegate->onSendDeviceProfileCompleted(true);
	}

	CCLOG("------- END [%s] -------", response->getHttpRequest()->getTag());
}

void GameClientManager::getDeviceProfile(std::string fbId, std::string deviceId )
{
	CCAssert(s_urlDevice.length() > 0, "Not set s_urlDevice yet");
	CCHttpRequest* request = new CCHttpRequest();
	request->setRequestType(CCHttpRequest::kHttpPost);

	request->setUrl(s_urlDevice.c_str());
	request->setTag("getDeviceProfile");
	request->setResponseCallback(this, httpresponse_selector(GameClientManager::_onGetDeviceProfileCompleted));

	// write the post data
	CCString* strData = CCString::createWithFormat("data={ \"fbId\": \"%s\", \"deviceId\": \"%s\" }&method=get",
		fbId.c_str(),
		deviceId.c_str()
		);


	CCLOG("Data: %s", strData->getCString());
	std::string s = encodeBeforeSend(strData->getCString());
	request->setRequestData(s.c_str(), strlen(s.c_str()));


	CCHttpClient::getInstance()->send(request);
	request->release();
}

void GameClientManager::_onGetDeviceProfileCompleted( CCHttpClient *sender, CCHttpResponse *response )
{
	if (!response || m_clientDelegate == NULL)
	{
		return;
	}

	//Show info
	CCLOG("------- BEGIN [%s] -------", response->getHttpRequest()->getTag());
	CCLOG("Status: [%i]", response->getResponseCode());

	if (!response->isSucceed())
	{
		CCLOG("Request failed: [%s]", response->getErrorBuffer());
		m_clientDelegate->onGetDeviceProfileCompleted(false, NULL);
	}
	else
	{
		std::vector<char> *buffer = response->getResponseData();
		std::string str(buffer->begin(), buffer->end());

		CCLOG("Content: [%s]", str.c_str());

		//get score from response
		json_t *root;
		json_error_t error;
		json_t *fbId;
		json_t *deviceId;
		json_t *deviceToken;
		json_t *devicePhoneNumber;
		json_t *deviceConfig;

		root = json_loads(str.c_str(), strlen(str.c_str()), &error);

		fbId = json_object_get(root, "fbId");
		deviceId = json_object_get(root, "deviceId");
		deviceToken = json_object_get(root, "deviceToken");
		deviceConfig = json_object_get(root, "deviceConfig");
		devicePhoneNumber = json_object_get(root, "devicePhoneNumber");

		DeviceProfile* acc = new DeviceProfile(
			json_string_value(deviceId), 
			json_string_value(deviceToken), 
			json_string_value(deviceConfig),
			json_string_value(devicePhoneNumber));

		m_clientDelegate->onGetDeviceProfileCompleted(true, acc);

	}

	CCLOG("------- END [%s] -------", response->getHttpRequest()->getTag());
}

//

void GameClientManager::sendScore( std::string appId, std::string fbId, int score )
{
	CCAssert(s_urlScore.length() > 0, "Not set s_urlScore yet");
	CCHttpRequest* request = new CCHttpRequest();
	request->setRequestType(CCHttpRequest::kHttpPost);

	request->setUrl(s_urlScore.c_str());
	request->setTag("sendScore");
	request->setResponseCallback(this, httpresponse_selector(GameClientManager::_onSendScoreCompleted));

	// write the post data
	CCString* strData = CCString::createWithFormat("data={ \"appId\": \"%s\", \"fbId\": \"%s\", \"score\": \"%d\" }&method=set",
		appId.c_str(),
		fbId.c_str(),
		score
		);


	CCLOG("Data: %s", strData->getCString());
	std::string s = encodeBeforeSend(strData->getCString());
	request->setRequestData(s.c_str(), strlen(s.c_str()));


	CCHttpClient::getInstance()->send(request);
	request->release();
}

void GameClientManager::_onSendScoreCompleted( CCHttpClient *sender, CCHttpResponse *response )
{
	if (!response || m_clientDelegate == NULL)
	{
		return;
	}

	//Show info
	CCLOG("------- BEGIN [%s] -------", response->getHttpRequest()->getTag());
	CCLOG("Status: [%i]", response->getResponseCode());

	if (!response->isSucceed())
	{
		CCLOG("Request failed: [%s]", response->getErrorBuffer());
		m_clientDelegate->onSendScoreCompleted(false);
	}
	else
	{
		std::vector<char> *buffer = response->getResponseData();
		std::string str(buffer->begin(), buffer->end());

		CCLOG("Content: [%s]", str.c_str());

		//get score from response
		json_t *root;
		json_error_t error;
		json_t *isSuccess;
		json_t *score;

		root = json_loads(str.c_str(), strlen(str.c_str()), &error);
		isSuccess = json_object_get(root, "isSuccess");
		bool success = json_is_true(isSuccess);

		m_clientDelegate->onSendScoreCompleted(success);
	}

	CCLOG("------- END [%s] -------", response->getHttpRequest()->getTag());
}

void GameClientManager::getScore( std::string appId, std::string fbId )
{
	CCAssert(s_urlScore.length() > 0, "Not set s_urlScore yet");
	CCHttpRequest* request = new CCHttpRequest();
	request->setRequestType(CCHttpRequest::kHttpPost);

	request->setUrl(s_urlScore.c_str());
	request->setTag("getScore");
	request->setResponseCallback(this, httpresponse_selector(GameClientManager::_onGetScoreCompleted));

	// write the post data
	CCString* strData = CCString::createWithFormat("data={ \"appId\": \"%s\", \"fbId\": \"%s\" }&method=get",
		appId.c_str(),
		fbId.c_str()
		);


	CCLOG("Data: %s", strData->getCString());
	std::string s = encodeBeforeSend(strData->getCString());
	request->setRequestData(s.c_str(), strlen(s.c_str()));


	CCHttpClient::getInstance()->send(request);
	request->release();
}

void GameClientManager::_onGetScoreCompleted( CCHttpClient *sender, CCHttpResponse *response )
{
	if (!response || m_clientDelegate == NULL)
	{
		return;
	}

	//Show info
	CCLOG("------- BEGIN [%s] -------", response->getHttpRequest()->getTag());
	CCLOG("Status: [%i]", response->getResponseCode());

	if (!response->isSucceed())
	{
		CCLOG("Request failed: [%s]", response->getErrorBuffer());
		m_clientDelegate->onGetScoreCompleted(false, -1, std::string());
	}
	else
	{
		std::vector<char> *buffer = response->getResponseData();
		std::string str(buffer->begin(), buffer->end());

		CCLOG("Content: [%s]", str.c_str());

		//get score from response
		json_t *root;
		json_error_t error;
		json_t *score;
		json_t *time;

		root = json_loads(str.c_str(), strlen(str.c_str()), &error);
		score = json_object_get(root, "score");
		time = json_object_get(root, "time");

		m_clientDelegate->onGetScoreCompleted(true, (int)atof(json_string_value(score)), json_string_value(time));
	}

	CCLOG("------- END [%s] -------", response->getHttpRequest()->getTag());
}

void GameClientManager::SendQuestResults( CCArray* arrQuestResults )
{
	//parse arrFriendList to json
	std::string strQuestList = std::string("");
	int count = arrQuestResults->count();

	for (int i = 0; i < count; ++i)
	{
		QuestionResult* questResult = (QuestionResult*)arrQuestResults->objectAtIndex(i);
		CCString* s = CCString::createWithFormat("{ \"question_id\": %d, \"is_true\": %d, \"time\": %d }", 
			questResult->m_questId, questResult->m_isRight, questResult->m_time);
		strQuestList.append(s->getCString());

		if (i != count - 1)
		{
			strQuestList.append(",");
		}
	}	

	CCString* strData;
	string userId = DataManager::sharedDataManager()->GetUserID();
	if (userId.length() > 0)
	{
		strData = CCString::createWithFormat("data={ \"user_id\": \"%s\", \"questions\": [%s]}", 
			userId.c_str(), strQuestList.c_str());
	}
	else
	{
		strData = CCString::createWithFormat("data={\"questions\": [%s]}", 
			strQuestList.c_str());
	}

	CCLOG("--SEND--: %s", strData->getCString());

	MY_SEND_REQUEST(CONF_STR(URL_LOG_QUEST_RESULTS), 
		this, 
		GameClientManager::SendQuestResultsCompleted, 
		strData->getCString());
}

void GameClientManager::SendQuestResultsCompleted( CCHttpClient *sender, CCHttpResponse *response )
{
	if (!response)
	{
		return;
	}

	//Show info
	CCLOG("------- BEGIN [%s] -------", response->getHttpRequest()->getTag());
	CCLOG("Status: [%i]", response->getResponseCode());

	if (!response->isSucceed())
	{
		CCLOG("Request failed: [%s]", response->getErrorBuffer());
	}
	else
	{
		std::vector<char> *buffer = response->getResponseData();
		std::string str(buffer->begin(), buffer->end());

		CCLOG("Content: %s", str.c_str());

		//get score from response
		json_t *root;
		json_error_t error;
		json_t *user_id;

		root = json_loads(str.c_str(), strlen(str.c_str()), &error);
		user_id = json_object_get(root, "user_id");

		string userId = DataManager::sharedDataManager()->GetUserID();;
		if (userId.length() == 0)
		{
			string newUserId = (string)json_string_value(user_id);
			CCLOG("SET USERID: %s", newUserId.c_str());

			DataManager::sharedDataManager()->SetUserID(newUserId);
		}
	}

	CCLOG("------- END [%s] -------", response->getHttpRequest()->getTag());
}
