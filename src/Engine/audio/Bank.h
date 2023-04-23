#pragma once
#include <filesystem>
#include <memory>

namespace fs = std::filesystem;
namespace FMOD { namespace Studio { class Bank; } }


class Bank
{
public:
	bool LoadFile(const fs::path&);
	bool IsValid() const;

private:
	struct Unloader
	{
		void operator()(FMOD::Studio::Bank*) const;
	};

	std::unique_ptr<FMOD::Studio::Bank, Unloader> myBank;
};