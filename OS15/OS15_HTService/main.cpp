#include "OS15_HTService.h"

int main()
{
	WCHAR ServiceName[] = SERVICENAME;

	SERVICE_TABLE_ENTRY ServiceStartTable[] = {
		{ServiceName, (LPSERVICE_MAIN_FUNCTION)ServiceMain},
		{NULL, NULL}
	};

	if (!StartServiceCtrlDispatcher(ServiceStartTable)) // регистрация службы
	{
		char temp[121];
		sprintf_s(temp, "StartServiceCtrlDispatcher failed, error code = %d\n", GetLastError());
		trace(temp, std::ofstream::out);
	}

	return 0;
}