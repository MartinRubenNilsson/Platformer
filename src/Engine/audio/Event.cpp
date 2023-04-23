#include "pch.h"
#include "audio/Event.h"
#include "interface/FMODInterface.h"
#include <fmod/fmod_studio.hpp>

using namespace FMOD::Studio;


// Public methods

bool Event::Create(const Description* aDesc)
{
	Instance* instance = nullptr;
	if (aDesc)
		aDesc->createInstance(&instance);
	myInstance.reset(instance);
	return instance;
}

bool Event::Create(const std::string& id)
{
	Description* desc = nullptr;
	FMODInterface::system->getEvent(("event:/" + id).c_str(), &desc);
	return Create(desc);
}

void Event::Start()
{
	if (myInstance)
		myInstance->start();
}

void Event::Stop(bool allowFadeout)
{
	if (myInstance)
		myInstance->stop(allowFadeout ? FMOD_STUDIO_STOP_ALLOWFADEOUT : FMOD_STUDIO_STOP_IMMEDIATE);
}


// Queries

bool Event::IsValid() const
{
	return myInstance ? myInstance->isValid() : false;
}

bool Event::IsOneshot() const
{
	bool oneshot = true;
	if (Description* desc = GetDescription())
		desc->isOneshot(&oneshot);
	return oneshot;
}


// Getters

Event::Description* Event::GetDescription() const
{
	Description* desc = nullptr;
	if (myInstance)
		myInstance->getDescription(&desc);
	return desc;
}

PlaybackState Event::GetPlaybackState() const
{
	FMOD_STUDIO_PLAYBACK_STATE state = FMOD_STUDIO_PLAYBACK_STOPPED;
	if (myInstance)
		myInstance->getPlaybackState(&state);
	return static_cast<PlaybackState>(state);
}

int Event::GetTimelinePosition() const
{
	int position = 0;
	if (myInstance)
		myInstance->getTimelinePosition(&position);
	return position;
}

void Event::SetParameter(const std::string& aName, float aValue)
{
	if (myInstance)
		myInstance->setParameterByName(aName.c_str(), aValue);
}


// Private classes

void Event::Releaser::operator()(Instance* anInstance) const
{
	anInstance->release();
}
