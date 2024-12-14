#pragma once
#include <memory>
#include <thread>
#include <queue>
#include <iostream>
#include <vector>
#include "Command.h"


class NetworkManager;
class NetworkClient;
class DirectWindow;
class App
{
public:
	App();
	virtual ~App();
	bool Initialize();
	void Run();
	void Shutdown();

	bool ProcessCommand();
	void PushCommand(Command&& command);

	void MainLoop();
private:
	std::unique_ptr<NetworkManager> mNetworkManager;
	std::unique_ptr<DirectWindow> mDirectWindow;

	std::queue<Command> mCommandQueue;
	//std::thread mCommandThread;

	std::vector<std::shared_ptr<NetworkClient>> mClientList;

	std::thread mMainThread;

	bool mRunning = false;
};