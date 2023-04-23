#pragma once
#include <filesystem>

namespace fs = std::filesystem;

namespace FMOD
{
	namespace Studio
	{
		class System;
	}
}

struct FMOD_GUID;


class FMODInterface
{
public:
	static FMOD::Studio::System* system;

	FMODInterface();
	~FMODInterface();

	static void PlayEvent(const FMOD_GUID*);
	static void StopAllEvents();

	bool Init();
	void Update();
	void ImGuiEditor();
};