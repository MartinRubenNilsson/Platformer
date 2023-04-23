#pragma once
#include <string>
#include <memory>


namespace FMOD
{
	namespace Studio
	{
		class EventDescription;
		class EventInstance;
	}
}


enum class PlaybackState
{
	Playing,
	Sustaining,
	Stopped,
	Starting,
	Stopping
};


class Event
{
public:
	using Description = FMOD::Studio::EventDescription;
	using Instance = FMOD::Studio::EventInstance;

	bool Create(const Description*);
	bool Create(const std::string& id);
	void Start();
	void Stop(bool allowFadeout = true);

	bool IsValid() const;
	bool IsOneshot() const;

	Description* GetDescription() const;
	PlaybackState GetPlaybackState() const;
	int GetTimelinePosition() const;

	void SetParameter(const std::string& aName, float aValue);

private:
	struct Releaser
	{
		void operator()(Instance*) const;
	};

	std::unique_ptr<Instance, Releaser> myInstance;
};