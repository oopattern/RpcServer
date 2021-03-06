#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "jsonintf.h"

CJsonIntf::CJsonIntf(const std::string& jsonStr)
{
    m_bIsJson = ((m_reader.parse(jsonStr, m_value)) ? (true) : (false));
}

// overload
CJsonIntf::CJsonIntf(const Json::Value& jsonVal)
{
    m_value = jsonVal;
    m_bIsJson = true;
}

// json convert to string
std::string CJsonIntf::JsonToString()
{
    std::string strVal = "";
    if(IsJson() && m_value.isObject())
    {
        Json::FastWriter writer;
        strVal = writer.write(m_value);
    }
    return strVal;
}

// print json tree 
void CJsonIntf::PrintValueTree(void)
{
    Json::StyledWriter styleWriter;
    std::cout << styleWriter.write(m_value);
}

// get json[key] = int
int CJsonIntf::GetIntFromJson(const char* key, int& nIntVal, int nDefaultInt)
{
    if(IsJson() && m_value.isMember(key) && m_value[key].isInt())
    {
        nIntVal = m_value[key].asInt();
        return 0;
    }
    nIntVal = nDefaultInt;
    return -1;
}

// get json[key] = string
int CJsonIntf::GetStringFromJson(const char* key, std::string& nStringVal, std::string& nDefaultString)
{
    if(IsJson() && m_value.isMember(key) && m_value[key].isString())
    {
        nStringVal = m_value[key].asString();
        return 0;    
    }
    nStringVal = nDefaultString;
    return -1;
}

// get json[key] = object
int CJsonIntf::GetObjectFromJson(const char* key, Json::Value& jsonVal)
{
    if(IsJson() && m_value.isMember(key) && m_value[key].isObject())
    {
        jsonVal = m_value[key];
        return 0;
    }
    return -1;
}

// get json[key] = array[int]
int CJsonIntf::GetIntArrayFromJson(const char* key, std::vector<int>& vecInt)
{
    vecInt.clear();
    if(IsJson() && m_value.isMember(key) && m_value[key].isArray())
    {
        for(int i = 0; i < (int)(m_value[key].size()); i++)
        {
            if(m_value[key][i].isInt())
            {
                vecInt.push_back(m_value[key][i].asInt());
            }
        }
    }
    return 0;
}

// get json[key] = array[string]
int CJsonIntf::GetStringArrayFromJson(const char* key, std::vector<std::string>& vecStr)
{
    vecStr.clear();
    if(IsJson() && m_value.isMember(key) && m_value[key].isArray())
    {
        for(int i = 0; i < (int)(m_value[key].size()); i++)
        {
            if(m_value[key][i].isString())
            {
                vecStr.push_back(m_value[key][i].asString());
            }
        }
    }
    return 0;
}

// get json[key] = array[object]
int CJsonIntf::GetObjectArrayFromJson(const char* key, std::vector<Json::Value>& vecJson)
{
    vecJson.clear();
    if(IsJson() && m_value.isMember(key) && m_value[key].isArray())
    {
        for(int i = 0; i < (int)m_value[key].size(); i++)
        {
            if(m_value[key][i].isObject())
            {
                vecJson.push_back(m_value[key][i]);
            }
        }
    }
    return 0;
}

bool CJsonIntf::IsJson()
{
    return m_bIsJson;
}

// example for json
void TestJson(void)
{
    int age = 0;
    Json::Value relJson;
    std::string defStr = "";
    std::string name = "";
    std::string father = "", mother = "";
    std::vector<int> scoreVec;
    std::vector<std::string> telVec;
    std::vector<Json::Value> carVec;

    std::string jsonStr = "{\"age\":18, \"name\":\"oath\", \"car\":[{\"city\":\"GD\",\"num\":9986},{\"city\":\"HB\",\"num\":5566}], \"relation\":{\"father\":\"dick\", \"mother\":\"lucy\"}, \"score\":[100,200,300,400], \"tel\":[\"135333\",\"1569208\"]}";
    CJsonIntf nJsonUser(jsonStr);

    nJsonUser.PrintValueTree();
    nJsonUser.GetIntFromJson("age", age, 0);
    nJsonUser.GetStringFromJson("name", name, defStr);
    nJsonUser.GetIntArrayFromJson("score", scoreVec);
    nJsonUser.GetStringArrayFromJson("tel", telVec);
    nJsonUser.GetObjectArrayFromJson("car", carVec);
    nJsonUser.GetObjectFromJson("relation", relJson);
    CJsonIntf nRelation(relJson);
    std::string relStr = nRelation.JsonToString();
    nRelation.GetStringFromJson("father", father, defStr);
    nRelation.GetStringFromJson("mother", mother, defStr);
    
    printf("Start to Test Json...\n");
    printf("relation json str: %s\n", relStr.c_str());
    for(int i = 0; i < (int)carVec.size(); i++)
    {
        CJsonIntf nCar(carVec[i]);
        std::string city = "";
        std::string carStr = "";
        int num = 0;
        carStr = nCar.JsonToString();
        nCar.GetStringFromJson("city", city, defStr);
        nCar.GetIntFromJson("num", num, 0);
        printf("car city[%s] num[%d]\n", city.c_str(), num);
        printf("car json str: %s\n", carStr.c_str());
    }
    printf("relation father[%s] mother[%s]\n", father.c_str(), mother.c_str()); 
    printf("user name[%s] age[%d]\n", name.c_str(), age);
    for(int i = 0; i < (int)scoreVec.size(); i++)
    {
        printf("score[%d]=%d\n", i, scoreVec[i]);
    }
    for(int i = 0; i < (int)telVec.size(); i++)
    {
        printf("tel[%d]=%s\n", i, telVec[i].c_str());
    }
}