#include "Include.h"

CMemory* g_pMemory = new CMemory();
CGlobals* g_pGlobals = new CGlobals();

/*
	Coded by Anna Salvatòre (legenden)
	Links: [
		https://t.me/@tickcount
		https://github.com/tickcount
		https://yougame.biz/members/11619/
	]
*/

EXECUTE WINAPI Init() {
	g_pGlobals->Init("LyntTag", "sociofob");

	g_pMemory->Process("csgo.exe");
	g_pMemory->DLL.Module = g_pMemory->getModule("engine.dll");
	g_pMemory->DLL.SizeOF = g_pMemory->getModuleSize("engine.dll");

	printf("| [%s] Tag Activated: %s \n", g_pMemory->Tag, g_pMemory->ActiveTag);

	while (true)
	{
		g_pMemory->Process("csgo.exe", false);
		g_pMemory->pushTag(g_pGlobals->Tags[g_pGlobals->WNumber % g_pGlobals->Tags.size()], 800);

		if (g_pGlobals->WNumber >= g_pGlobals->Tags.size())
			g_pGlobals->WNumber = NULL;
		else
			++g_pGlobals->WNumber;
	}
}

void CGlobals::setList()
{
	Tags.push_back("sociofob     ");
	Tags.push_back(" sociofob    ");
	Tags.push_back("  sociofob   ");
	Tags.push_back("   sociofob  ");
	Tags.push_back("    sociofob ");
	Tags.push_back("     sociofob");
	Tags.push_back("    sociofob ");
	Tags.push_back("   sociofob  ");
	Tags.push_back("  sociofob   ");
	Tags.push_back(" sociofob    ");
}
