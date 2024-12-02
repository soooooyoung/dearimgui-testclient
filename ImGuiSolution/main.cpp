#pragma once
#include <WinSock2.h>
#include <iostream>


#include "App.h"

int main(int, char**)
{
	App app;
	if (app.Initialize())
	{
		app.Run();
	}

	return 0;
}
