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

	auto commandCallback = [this](Command&& command) {
		PushCommand(std::move(command)); 
		};

	mDirectWindow->mCommandCallback = commandCallback;

	if (false == mNetworkManager->CreateNetwork())
	{
		return false;
	}
	if (false == mDirectWindow->Initialize())
	{
		return false;
	}

	return true;
}

void App::Run()
{
	mRunning = true;
	mCommandThread = std::thread(&App::CommandThread, this);
	mDirectWindow->RunLoop();
}

void App::Shutdown()
{
	mRunning = false;
	if (mCommandThread.joinable())
	{
		mCommandThread.join();
	}
}

void App::CommandThread()
{
	while (mRunning)
	{
		if (mCommandQueue.empty())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			printf("thread sleeping\n");	
			continue;
		}

		auto& command = mCommandQueue.front();

		switch (command.Type)
		{
		case CommandType::Send:
		{
			if (!mNetworkManager->Send(command.Data))
			{
				printf("Failed to send message\n");
			}
		}
			break;
		default:
			break;
		}

		mCommandQueue.pop();
	}

}

void App::PushCommand(Command&& command)
{
	mCommandQueue.push(std::move(command));
}
