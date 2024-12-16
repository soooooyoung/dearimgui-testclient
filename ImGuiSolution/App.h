#pragma once
#include <memory>
#include <thread>
#include <queue>
#include <iostream>
#include <vector>
#include <mutex>
#include <functional>
#include <chrono>
#include <condition_variable>
#include <atomic>
#include <ratio>

#include "Command.h"
#include "Timer.h"	


class NetworkManager;
class NetworkClient;
class DirectWindow;
class App
{
public:
	App();
	virtual ~App();
	bool Initialize();
	void Shutdown();

	bool ProcessCommand();
	void PushCommand(Command&& command);

	void MainLoop();
private:
	std::unique_ptr<NetworkManager> mNetworkManager;
	std::unique_ptr<DirectWindow> mDirectWindow;

	std::mutex mCommandLock;
	std::queue<Command> mCommandQueue;
	std::thread mCommandThread;

	std::vector<std::shared_ptr<NetworkClient>> mClientList;

	Timer mTimer;
	bool mRunning = false;
};