// ServiceWithServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "svccontrol.h"
#include "svcconfig.h"

void __cdecl initSvcForSCM();
VOID SvcInstall();

TCHAR szCommand[10];
TCHAR szSvcName[80];

int _tmain(int argc, _TCHAR* argv[])
{
	if (lstrcmpi(argv[1], TEXT("install")) == 0) {
		SvcInstall();
		return 0;
	}

	if (argc >= 2) {
		StringCchCopy(szCommand, 10, argv[1]);
	}
	if (argc == 2) {
		StringCchCopy(szSvcName, 80, L"svcTerminal");
	}

	if (argc == 2) {
		if (lstrcmpi(szCommand, TEXT("start")) == 0) {
			DoStartSvc();
			return 0;
		}
		else if (lstrcmpi(szCommand, TEXT("dacl")) == 0) {
			DoUpdateSvcDacl();
			return 0;
		}
		else if (lstrcmpi(szCommand, TEXT("stop")) == 0) {
			DoStopSvc();
			return 0;
		}
		else if (lstrcmpi(szCommand, TEXT("query")) == 0) {
			DoQuerySvc();
			return 0;
		}
		else if (lstrcmpi(szCommand, TEXT("describe")) == 0) {
			DoUpdateSvcDesc();
			return 0;
		}
		else if (lstrcmpi(szCommand, TEXT("disable")) == 0) {
			DoDisableSvc();
			return 0;
		}
		else if (lstrcmpi(szCommand, TEXT("enable")) == 0) {
			DoEnableSvc();
			return 0;
		}
		else if (lstrcmpi(szCommand, TEXT("delete")) == 0) {
			DoDeleteSvc();
			return 0;
		}
		else {
			_tprintf(TEXT("Unknown command (%s)\n\n"), szCommand);
			DisplayUsage();
		}
	}

	initSvcForSCM();

	return 0;
}
