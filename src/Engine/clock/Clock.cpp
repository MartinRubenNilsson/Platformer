#include "pch.h"
#include "clock/Clock.h"
#include "window/WindowMessage.h"


// Constructor

Clock::Clock(bool on)
	: myOn(on)
	, myNow(clock::now())
	, myTotalTime(0.f)
	, myDeltaTime(0.f)
	, myPauseTime(FLT_MIN)
	, myPauseDuration(FLT_MIN)
{
}


// Public methods

bool Clock::Handle(const WindowMessage& aMessage)
{
	switch (aMessage.type)
	{
	case MessageType::Activate:
		SetOn(aMessage.active);
		break;
	case MessageType::EnterSizeMove:
		Stop();
		break;
	case MessageType::ExitSizeMove:
		Start();
		break;
	}

	return false;
}

void Clock::Update()
{
	const time_point before = myNow;

	myNow = clock::now();
	myDeltaTime = seconds::zero();

	const seconds trueDeltaTime = myNow - before;

	if (myOn)
	{
		myDeltaTime = trueDeltaTime;
		myTotalTime += trueDeltaTime;
	}
	else if (IsPaused())
	{
		myPauseTime += trueDeltaTime;
		if (myPauseTime >= myPauseDuration)
			Start();
	}
}

void Clock::SetOn(bool on)
{
	myOn = on;
	myNow = clock::now();
	myDeltaTime = seconds::zero();
	myPauseTime = seconds(FLT_MIN);
	myPauseDuration = seconds(FLT_MIN);
}

void Clock::Pause(float aDuration)
{
	if (aDuration > 0.f)
	{
		Stop();
		myPauseDuration = seconds(aDuration);
	}
}
