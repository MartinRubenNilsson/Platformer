#pragma once
#include <bitset>


class IntMask
{
public:
	using bitset_t = std::bitset<8>;

	constexpr IntMask() noexcept = default;
	constexpr IntMask(const bitset_t& aBitset) noexcept : myBitset(aBitset) {}

	constexpr bool operator[](size_t pos) const { return myBitset[pos]; }
	constexpr IntMask operator~() const noexcept { return ~myBitset; };

private:
	bitset_t myBitset;
};


namespace Mask
{
	enum
	{
		None		 = 0b00001,
		Dirt		 = 0b00010,
		Ladder		 = 0b00100,
		Stone		 = 0b01000,
		Destructible = 0b10000
	};

	constexpr IntMask solids(Dirt | Stone | Destructible);
	constexpr IntMask ladder(Ladder);
}