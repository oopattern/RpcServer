#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <functional>
#include "timer.h"

using std::cout;
using std::endl;

void CTimer::Run()
{
    m_callback();
}