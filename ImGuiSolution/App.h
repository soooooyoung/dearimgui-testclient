#pragma once
#include <memory>
#include <thread>
#include <queue>
#include <iostream>
#include "Command.h"


class NetworkManager;
class DirectWindow;
class App
{
public:
	App();
	virtual ~App();
	bool Initialize();
	void Run();
	void Shutdown();
	void CommandThread();

	void PushCommand(Command&& command);
private:
	std::unique_ptr<NetworkManager> mNetworkManager;
	std::unique_ptr<DirectWindow> mDirectWindow;

	std::queue<Command> mCommandQueue;
	std::thread mCommandThread;

	bool mRunning = false;
};