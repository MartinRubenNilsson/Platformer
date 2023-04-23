#pragma once
#include "math/vec2.hpp"
#include "math/Direction.h"
#include <vector>
#include <filesystem>
#include <limits>

namespace fs = std::filesystem;

class IntMask;

template <class T>
struct Ray
{
	vec2<T> origin;
	Direction direction;
};

template <class T>
struct Hit
{
	T distance = std::numeric_limits<T>::max();
	int value = 0;
};


class IntGrid
{
public:
	IntGrid();

	void SetCellSize(uint2 aGridSize) { myCellSize = aGridSize; }
	bool LoadCSVFile(const fs::path& aPath);

	uint2 GetSize() const { return (uint2)mySize; }
	uint2 GetCellSize() const { return myCellSize; }

	bool Contains(int2 aCell) const;

	int GetValue(int2) const;
	int GetValue(int2, IntMask) const;
	int GetValue(float2, IntMask) const;
	void SetValue(int2, int aValue);
	void SetValue(float2, int aValue);

	bool Raycast(Ray<int>, Hit<int>&, IntMask) const;
	bool Raycast(Ray<float>, Hit<float>&, IntMask) const;
	void SignedRaycast(Ray<float>, Hit<float>&, IntMask) const;

private:
	int2 GetContainingCell(float2) const;

	int2 mySize; // Using int2 avoids certain undesired implicit casts to unsigned.
	uint2 myCellSize;
	std::vector<int> myValues;
};