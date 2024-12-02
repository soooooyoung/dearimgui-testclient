#include "App.h"
#include "NetworkManager.h"
#include "DirectWindow.h"


App::App()
{
	mNetworkManager = std::make_unique<NetworkManager>();
	mDirectWindow = std::make_unique<DirectWindow>();
}

App::~App()
{
}

bool App::Initialize()
{
	if (false == mNetworkManager->CreateNetwork())
	{
		return false;
	}
	if (false == mDirectWindow->Initialize())
	{
		return false;
	}

	mDirectWindow->RunLoop();

	return true;
}

void App::Run()
{

}