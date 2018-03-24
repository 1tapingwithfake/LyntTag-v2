#include "Include.h"

CMemory::CMemory(){ }
CMemory::~CMemory(){ CloseHandle(process); }

BOOL WINAPI ConsoleHandler(DWORD dwCtrlEvent)
{
	switch (dwCtrlEvent)
	{
		case CTRL_C_EVENT: 
		case CTRL_BREAK_EVENT:
		case CTRL_CLOSE_EVENT:
		case CTRL_LOGOFF_EVENT: g_pMemory->pushTag("", NULL);
		case CTRL_SHUTDOWN_EVENT:
		default: return TRUE; break;
	}
}

void main()
{
	// Main Function
	if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE))
	{
		printf("| [Security] Unable to install handler\n");
		system("pause");
		exit(NULL);
	}

	Init();
}

void CGlobals::Init(const char* cTag, const char* cActiveTag)
{
	printf("| Dynamic Tag Changer (Recode v2.1) \n");
	printf("| Developed by `legenden` (Anna Golczwart) \n");
	printf("| Contacts: /id/0x4E | vk.com/id92774 \n\n");

	printf("| Logging: \n");

	this->setList();
	g_pMemory->Tag = cTag;
	g_pMemory->ActiveTag = cActiveTag;
}

void CMemory::pushTag(const char* tag, int Delay) {
	unsigned char Shellcode[] =
		"\x51"						//push ecx 
		"\x52"						//push edx 
		"\xB9\x00\x00\x00\x00"		//mov ecx,00000000 { 0 } 
		"\xBA\x00\x00\x00\x00"		//mov edx,00000000 { 0 } 
		"\xE8\x00\x00\x00\x00"		//call 0 
		"\x83\x04\x24\x0A"			//add dword ptr [esp],0A { 10 } 
		"\x68\x00\x00\x00\x00"		//push engine.dll+9AC90 
		"\xC3"						//ret 
		"\x5A"						//pop edx 
		"\x59"						//pop ecx 
		"\xC3"						//ret 
		"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"	//reserve memory[0x10] 
		"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"	//reserve memory[0x10] 
	;

	static unsigned int SHELLCODE_SIZE = sizeof(Shellcode) - 0x21;
	unsigned int TAG_SIZE = (strlen(tag) > 15) ? 15 : strlen(tag);
	unsigned int NAME_SIZE = (strlen(tag) > 15) ? 15 : strlen(tag);
	unsigned int DATA_SIZE = TAG_SIZE + NAME_SIZE + 2;

	LPVOID pShellCodeAdress = VirtualAllocEx(process, NULL, SHELLCODE_SIZE + DATA_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	DWORD tagAdress = (DWORD)pShellCodeAdress + SHELLCODE_SIZE;
	DWORD nameAdress = (DWORD)pShellCodeAdress + SHELLCODE_SIZE + TAG_SIZE + 1;
	DWORD fnSetClanAdress = getSignature(DLL.Module, DLL.SizeOF, (PBYTE)"\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15", "xxxxxxxxx");  //Engine.dll + 0x9AC90 

	memcpy(Shellcode + 0x3, &tagAdress, sizeof(DWORD));
	memcpy(Shellcode + 0x8, &nameAdress, sizeof(DWORD));
	memcpy(Shellcode + 0x16, &fnSetClanAdress, sizeof(DWORD));
	memcpy(Shellcode + SHELLCODE_SIZE, tag, TAG_SIZE);
	memcpy(Shellcode + SHELLCODE_SIZE + TAG_SIZE + 1, tag, NAME_SIZE);

	WriteProcessMemory(process, pShellCodeAdress, Shellcode, SHELLCODE_SIZE + DATA_SIZE, NULL);

	HANDLE hThread = CreateRemoteThread(process, NULL, NULL, (LPTHREAD_START_ROUTINE)pShellCodeAdress, NULL, NULL, NULL);
	WaitForSingleObject(hThread, INFINITE);
	VirtualFreeEx(process, pShellCodeAdress, 0, MEM_RELEASE);

	Sleep(Delay);
}

void CMemory::Process(char* name, bool Debug){
	if(Debug)
		printf("| [%s] Finding Process: %s\n", Tag, name);

	PROCESSENTRY32 ProcEntry;
	HANDLE hPID = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	ProcEntry.dwSize = sizeof(ProcEntry);

	do
		if (!strcmp(ProcEntry.szExeFile, name))
		{
			PID = ProcEntry.th32ProcessID;
			CloseHandle(hPID);

			process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
			if (Debug)
				printf("| [%s] Attached to process: %s\n", Tag, name);

			return;
		} while (Process32Next(hPID, &ProcEntry));

	printf("| [%s] Process not found\n", Tag);
	system("pause");
	exit(NULL);
}

DWORD CMemory::getModule(char* name)
{
	printf("| [%s] Connecting to module: %s\n", Tag, name);

	MODULEENTRY32 mEntry;
	HANDLE hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);

	mEntry.dwSize = sizeof(mEntry);

	do
		if (!strcmp(mEntry.szModule, name))
		{
			CloseHandle(hModule);

			printf("| [%s] Attached to module: %s (Adress: %s)\n", Tag, name, std::to_string((DWORD)mEntry.modBaseAddr).c_str());
			return (DWORD)mEntry.modBaseAddr;

		} while (Module32Next(hModule, &mEntry));

	printf("| [%s] Cant attach to module\n", Tag);
	system("pause");
	exit(NULL);
}

DWORD CMemory::getModuleSize(char* name)
{
	MODULEENTRY32 mEntry;
	HANDLE hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);

	mEntry.dwSize = sizeof(mEntry);

	do
		if (!strcmp(mEntry.szModule, name))
		{
			CloseHandle(hModule);
			return (DWORD)mEntry.modBaseSize;

		} while (Module32Next(hModule, &mEntry));

	return NULL;
}

bool DataCompare(BYTE* data, BYTE* sign, char* mask)
{
	for (; *mask; mask++, sign++, data++)
	{
		if (*mask == 'x' && *data != *sign)
			return false;
	} 
	
	return true;
}

DWORD CMemory::getSignature(DWORD base, DWORD size, BYTE* sign, char* mask)
{
	DWORD offset = NULL;
	MEMORY_BASIC_INFORMATION mbi = { NULL };

	while (offset < size)
	{
		VirtualQueryEx(process, (LPCVOID)(base + offset), &mbi, sizeof(MEMORY_BASIC_INFORMATION));
		if (mbi.State != MEM_FREE)
		{
			BYTE* buffer = new BYTE[mbi.RegionSize];
			ReadProcessMemory(process, mbi.BaseAddress, buffer, mbi.RegionSize, NULL);
			for (int i = 0; i < mbi.RegionSize; i++)
			{
				if (DataCompare(buffer + i, sign, mask))
				{
					delete[] buffer;
					return (DWORD)mbi.BaseAddress + i;
				}

			} delete[] buffer;

		} offset += mbi.RegionSize;
	}
	
	return NULL;
}
