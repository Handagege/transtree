#ifndef _EROS_WORK_INTERFACE_JSON_CONV_H 
#define _EROS_WORK_INTERFACE_JSON_CONV_H 

#include "json.h"
#include "json/json.h"


class JsonConv
{
private: 
	JsonConv() 
	{
	}
	virtual ~JsonConv() 
	{
	}

public: 
	static bool ObjC2Cpp(Json::Value& jsonValue, json_object* pJsonObj); 
	
	static string CppObj2String(Json::Value& jsonValue, const bool bStyled = false); 

	static bool String2CppObj(Json::Value& jsonValue, const char* sJsonStr); 
};



bool JsonConv::ObjC2Cpp(Json::Value& jsonValue, json_object* pJsonObj)
{
	if(!pJsonObj)
		return false; 

	Json::Reader reader; 
	
	try
	{
		jsonValue.clear(); 
		char* json_str = (char*)json_object_to_json_string(pJsonObj); 
		return reader.parse(json_str, jsonValue); 
	}
	catch(...)
	{
		return false; 
	}
}


string JsonConv::CppObj2String(Json::Value& jsonValue, const bool bStyled)
{
	string str; 
	if(bStyled)
	{
		Json::StyledWriter writer;
		str = writer.write(jsonValue);
	}
	else
	{
		Json::FastWriter writer; 
		str = writer.write(jsonValue);
		str.erase(str.length()-1, 1);
	}
	return str; 
}


bool JsonConv::String2CppObj(Json::Value& jsonValue, const char* sJsonStr)
{
	if(!sJsonStr)
		return false; 

	Json::Reader reader; 
	
	try
	{
		jsonValue.clear(); 
		return reader.parse(sJsonStr, jsonValue); 
	}
	catch(...)
	{
		return false; 
	}
}


#endif /* _EROS_WORK_INTERFACE_JSON_CONV_H */ 

