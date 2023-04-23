#include "pch.h"
#include "audio/Listener.h"
#include "interface/FMODInterface.h"


bool Listener::Set() const
{
	FMOD_3D_ATTRIBUTES attributes = {};
	attributes.position = { position.x, -position.y, 0.f };
	attributes.velocity = { velocity.x, -velocity.y, 0.f };
	attributes.forward = { 0.f, 0.f, 1.f };
	attributes.up = { 0.f, 1.f, 0.f };
	FMOD_RESULT result = FMODInterface::system->setListenerAttributes(0, &attributes);
	return result == FMOD_OK;
}
