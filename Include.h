#pragma once

#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>

#include "Classes.h"

using namespace std;
typedef void EXECUTE;

extern EXECUTE WINAPI Init();
extern CMemory* g_pMemory;
extern CGlobals* g_pGlobals;