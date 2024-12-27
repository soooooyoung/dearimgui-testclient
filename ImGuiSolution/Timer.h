#pragma once

#include <functional>
#include <chrono>

class Timer
{
public:
	Timer() : mStart(std::chrono::high_resolution_clock::now()) {}
	Timer(std::function<void()> callback) : mStart(std::chrono::high_resolution_clock::now()), mCallback(callback) {}
	~Timer() {}

	void Reset() { mStart = std::chrono::high_resolution_clock::now(); }
	
	template<typename T>
	float Elapsed() const
	{
		return std::chrono::duration<float, T>(std::chrono::high_resolution_clock::now() - mStart).count();
	};

	void SetCallback(std::function<void()> callback)
	{
		mCallback = callback;
	}

	void Update()
	{
		if (mCallback)
		{
			mCallback();
		}
	}

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> mStart;
	std::function<void()> mCallback;
};