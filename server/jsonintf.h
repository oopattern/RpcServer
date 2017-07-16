#ifndef __JSON_INTF_H__
#define  __JSON_INTF_H__

#include <stdio.h>
#include <string>
#include <vector>
#include "json.h"

class CJsonIntf
{
public:
    CJsonIntf(const std::string& jsonStr);

    // overload
    CJsonIntf(const Json::Value& jsonVal);

    // json convert to string
    std::string JsonToString();

    // print json tree 
    void PrintValueTree();

    // get json[key] = int
    int GetIntFromJson(const char* key, int& nIntVal, int nDefaultInt);

    // get json[key] = string
    int GetStringFromJson(const char* key, std::string& nStringVal, std::string& nDefaultString);

    // get json[key] = object
    int GetObjectFromJson(const char* key, Json::Value& jsonVal);

    // get json[key] = array[int]
    int GetIntArrayFromJson(const char* key, std::vector<int>& vecInt);

    // get json[key] = array[string]
    int GetStringArrayFromJson(const char* key, std::vector<std::string>& vecStr);

    // get json[key] = array[object]
    int GetObjectArrayFromJson(const char* key, std::vector<Json::Value>& vecJson);
private:
    bool IsJson();
private:
    Json::Reader m_reader;
    Json::Value m_value;
    bool m_bIsJson;
};

#endif