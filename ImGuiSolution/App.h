#pragma once
#include <memory>
#include <thread>

class NetworkManager;
class DirectWindow;
class App
{
public:
	App();
	virtual ~App();
	bool Initialize();
	void Run();

private:
	std::unique_ptr<NetworkManager> mNetworkManager;
	std::unique_ptr<DirectWindow> mDirectWindow;

	bool mRunning = true;
};