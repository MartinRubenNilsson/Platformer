#include "pch.h"
#include "audio/Bank.h"
#include "interface/FMODInterface.h"


// Public methods

bool Bank::LoadFile(const fs::path& aPath)
{
	FMOD::Studio::Bank* bank = nullptr;
	FMOD_RESULT result = FMODInterface::system->loadBankFile(
		aPath.string().c_str(),
		FMOD_STUDIO_LOAD_BANK_NORMAL,
		&bank
	);
	myBank.reset(bank);
	return result == FMOD_OK;
}

bool Bank::IsValid() const
{
	return myBank ? myBank->isValid() : false;
}


// Private classes

void Bank::Unloader::operator()(FMOD::Studio::Bank* aBank) const
{
	aBank->unload();
}
