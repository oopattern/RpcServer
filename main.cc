#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <string>
#include <boost/bind.hpp>

using namespace std;

int main(void)
{
    std::map<int, string> a;
    a.clear();
    a[0] = "oath";
    printf("hello name[%s]\n", a[0].c_str());
    return 0;
}