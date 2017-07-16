#include <stdio.h>
#include <stdlib.h>
#include "jsonintf.h"

// example for json
static void TestJson(void)
{
    int age = 0;
    Json::Value relJson;
    std::string defStr = "";
    std::string name = "";
    std::string father = "", mother = "";
    std::vector<int> scoreVec;
    std::vector<std::string> telVec;
    std::vector<Json::Value> carVec;

    printf("Start to Test Json...\n");

    std::string jsonStr = "{\"age\":18, \"name\":\"oath\", \"car\":[{\"city\":\"GD\",\"num\":9986},{\"city\":\"HB\",\"num\":5566}], \"relation\":{\"father\":\"dick\", \"mother\":\"lucy\"}, \"score\":[100,200,300,400], \"tel\":[\"135333\",\"1569208\"]}";
    CJsonIntf nJsonUser(jsonStr);
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

int main(void)
{
    TestJson();
    return 0;
}