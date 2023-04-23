#include "pch.h"
#include "interface/FMODInterface.h"
#include <fmod/fmod_studio_guids.hpp>


// Static members

FMOD::Studio::System* FMODInterface::system = nullptr;


// Constructor & destructor

FMODInterface::FMODInterface() = default;

FMODInterface::~FMODInterface()
{
	system->release();
}


// Static methods

void FMODInterface::PlayEvent(const FMOD_GUID* id)
{
	FMOD::Studio::EventDescription* eventDesc;
	system->getEventByID(id, &eventDesc);
	FMOD::Studio::EventInstance* eventInst;
	eventDesc->createInstance(&eventInst);
	eventInst->start();
	eventInst->release();
}

void FMODInterface::StopAllEvents()
{
	FMOD::Studio::Bus* masterBus;
	system->getBusByID(&FSPRO::Bus::Master_Bus, &masterBus);
	masterBus->stopAllEvents(FMOD_STUDIO_STOP_IMMEDIATE);
}


// Nonstatic methods

bool FMODInterface::Init()
{
	FMOD_RESULT result = FMOD::Studio::System::create(&system);
	if (result != FMOD_OK)
		return false;

	result = system->initialize(256, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr);
	if (result != FMOD_OK)
		return false;

	return true;
}

void FMODInterface::Update()
{
	system->update();
}

void FMODInterface::ImGuiEditor()
{
	constexpr int pathSize = 256;

	ImGui::Begin("FMOD Studio");

	if (ImGui::Button("Stop all events"))
		StopAllEvents();

	int bankCount;
	system->getBankCount(&bankCount);
	std::vector<FMOD::Studio::Bank*> bankList(bankCount);
	system->getBankList(bankList.data(), bankCount, nullptr);

	for (auto bank : bankList)
	{
		char bankPath[pathSize];
		bank->getPath(bankPath, pathSize, nullptr);

		if (std::string(bankPath).ends_with("strings"))
			continue;

		if (ImGui::TreeNode(bankPath))
		{
			int eventCount;
			bank->getEventCount(&eventCount);
			std::vector<FMOD::Studio::EventDescription*> eventList(eventCount);
			bank->getEventList(eventList.data(), eventCount, nullptr);

			for (auto eventDesc : eventList)
			{
				char eventPath[pathSize];
				eventDesc->getPath(eventPath, pathSize, nullptr);

				bool alwaysFalse = false;
				if (ImGui::Selectable(eventPath, &alwaysFalse))
				{
					FMOD::Studio::EventInstance* eventInst;
					eventDesc->createInstance(&eventInst);
					eventInst->start();
					eventInst->release();
				}
			}

			ImGui::TreePop();
		}
	}

	ImGui::End();
}

