#include "App.h"
#include "NetworkClient.h"
#include "NetworkManager.h"
#include "DirectWindow.h"
#include "ConfigLoader.h"

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
	mMainThread = std::thread(&App::MainLoop, this);
}

void App::MainLoop()
{
	while (mRunning)
	{
		if (!ProcessCommand())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		mDirectWindow->Draw();
	}
}

void App::Shutdown()
{
	mRunning = false;
	if (mMainThread.joinable())
	{
		mMainThread.join();
	}
}

bool App::ProcessCommand()
{
	if (mCommandQueue.empty())
	{
		return false;
	}

	auto& command = mCommandQueue.front();

	switch (command.Type)
	{
	case CommandType::Connect:
	{
		auto client = mNetworkManager->AddClient();

		if (nullptr == client)
		{
			printf("Failed to add client\n");
		}

		mClientList.push_back(client);
		mDirectWindow->PushClient(client);

		auto& clientConfig = ConfigLoader::GetInstance().GetClientConfig();
		client->PostConnect(clientConfig.mServerAddress, clientConfig.mServerPort);
	}
	break;
	//case CommandType::Receive:
	//{
	//	mDirectWindow->PushChat(command.Data);
	//}
	//break;
	//case CommandType::Send:
	//{

	//}
	break;
	default:
		break;
	}

	mCommandQueue.pop();

	return true;
}

void App::PushCommand(Command&& command)
{
	mCommandQueue.push(std::move(command));
}
