#include "DirectWindow.h"


int main(int, char**)
{
	DirectWindow windowManager;

	if (false == windowManager.Initialize())
	{
		return 1;
	}

	windowManager.RunLoop();

	return 0;
}
