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

void App::MainLoop()
{
	auto testConfig = ConfigLoader::GetInstance().GetTestConfig();
	mRunning = true;
	mCommandThread = std::thread([this]() {
		while (mRunning)
		{
			std::lock_guard<std::mutex> lock(mCommandLock);

			if (false == ProcessCommand())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}
		});

	while (mRunning)
	{
		if (!mCommandQueue.empty())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		mDirectWindow->Draw();

		if (mTimer.Elapsed<std::milli>() > 1000)
		{
			mTimer.Update();
		}
	}
}

void App::Shutdown()
{
	mRunning = false;
	if (mCommandThread.joinable())
	{
		mCommandThread.join();
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

		client->SetSessionID(mClientList.size());

		mClientList.push_back(client);
		mDirectWindow->PushClient(client);

		auto& clientConfig = ConfigLoader::GetInstance().GetClientConfig();
		printf_s("Server Address: %s\n", clientConfig.mServerAddress.c_str());
		printf_s("Server Port: %d\n", clientConfig.mServerPort);

		client->PostConnect(clientConfig.mServerAddress, clientConfig.mServerPort);
	}
	break;
	case CommandType::StartTest:
	{
		auto& testConfig = ConfigLoader::GetInstance().GetTestConfig();
		printf_s("Start Test: %d\n", testConfig.mClientCount);

		auto pushCommand = [&]() {
			PushCommand(Command(CommandType::Connect, nullptr));
			};

		mTimer.SetCallback(pushCommand);
		mTimer.Reset();
	}
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
