#pragma once
#include "window/MessageHandler.h"
#include <chrono>


class Clock : public MessageHandler
{
public:
	Clock(bool on = true);

	bool Handle(const WindowMessage&) override;
	void Update();

	void SetOn(bool on);
	void Start() { SetOn(true); }
	void Stop() { SetOn(false); }
	void Pause(float aDuration);
	void Repopulate() { *this = Clock(myOn); }

	bool IsOn() const { return myOn; }
	bool IsPaused() const { return myPauseTime < myPauseDuration; }

	float GetTotalTime() const { return myTotalTime.count(); }
	float GetDeltaTime() const { return myDeltaTime.count(); }
	float GetPauseProgress() const { return myPauseTime / myPauseDuration; }

private:
	using clock = std::chrono::steady_clock;
	using time_point = clock::time_point;
	using seconds = std::chrono::duration<float>;

	bool myOn;
	time_point myNow;
	seconds myTotalTime;
	seconds myDeltaTime;
	seconds myPauseTime;
	seconds myPauseDuration;
};