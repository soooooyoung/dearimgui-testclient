#pragma once

#include "App.h"
#include "ConfigLoader.h"
int main(int, char**)
{
	if (false == ConfigLoader::GetInstance().Load())	
	{
		return -1;
	}

	App app;
	if (app.Initialize())
	{
		app.MainLoop();
	}

	return 0;
}
