#pragma once

struct DLLInfo
{
	DWORD Module;
	DWORD SizeOF;
};

class CGlobals {
public:
	int WNumber = NULL;

	std::vector<const char*> Tags;

	void Init(const char* cTag, const char* cActiveTag);
	void setList();
};

class CMemory {
public:
	CMemory();
	~CMemory();

	DWORD PID;
	HANDLE process;
	DLLInfo DLL;

	const char* Tag = "", *ActiveTag = "";

	void Process(char* name, bool Debug = true);

	virtual DWORD getModule(char* name);
	virtual DWORD getModuleSize(char* name);

	void pushTag(const char* tag, int Delay);
	DWORD getSignature(DWORD base, DWORD size, BYTE* sign, char* mask);
};