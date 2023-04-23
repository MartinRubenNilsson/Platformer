#include "pch.h"
#include "grid/IntGrid.h"
#include "grid/IntMask.h"
#include "math/math_funcs.h"


// Constructor

IntGrid::IntGrid()
	: mySize{ 0, 0 }
	, myCellSize{ 0, 0 }
	, myValues()
{
}


// Public methods

bool IntGrid::LoadCSVFile(const fs::path& aFile)
{
	assert(aFile.extension() == ".csv");

	std::ifstream i(aFile);
	if (!i.is_open())
		return false;

	size_t x = 0;
	size_t y = 0;
	for (std::string line; std::getline(i, line); ++y)
	{
		x = 0;
		for (std::istringstream ss(line); ss >> myValues.emplace_back(); ss.ignore(), ++x) {}
		myValues.pop_back();
	}

	mySize = { static_cast<int>(x), static_cast<int>(y) };

	return true;
}

bool IntGrid::Contains(int2 aCell) const
{
	return 0 <= aCell.x && aCell.x < mySize.x && 0 <= aCell.y && aCell.y < mySize.y;
}

int IntGrid::GetValue(int2 aCell) const
{
	if (Contains(aCell))
		return myValues[aCell.x + mySize.x * aCell.y];
	else
		return 0;
}

int IntGrid::GetValue(int2 aCell, IntMask aMask) const
{
	int value = GetValue(aCell);
	if (value && aMask[value])
		return value;
	else
		return 0;
}

int IntGrid::GetValue(float2 aPos, IntMask aMask) const
{
	return GetValue(GetContainingCell(aPos), aMask);
}

void IntGrid::SetValue(int2 aCell, int aValue)
{
	if (Contains(aCell) && aValue >= 0)
		myValues[aCell.x + mySize.x * aCell.y] = aValue;
}

void IntGrid::SetValue(float2 aPos, int aValue)
{
	SetValue(GetContainingCell(aPos), aValue);
}

bool IntGrid::Raycast(Ray<int> aRay, Hit<int>& anOutHit, IntMask aMask) const
{
	int cellDistance = 0;

	switch (aRay.direction)
	{
	case Direction::Right:
		if (aRay.origin.x >= mySize.x || aRay.origin.y < 0 || aRay.origin.y >= mySize.y)
			return false;
		if (aRay.origin.x < 0)
		{
			cellDistance = -aRay.origin.x;
			aRay.origin.x = 0;
		}
		break;
	case Direction::Left:
		if (aRay.origin.x < 0 || aRay.origin.y < 0 || aRay.origin.y >= mySize.y)
			return false;
		if (aRay.origin.x >= mySize.x)
		{
			cellDistance = aRay.origin.x - mySize.x + 1;
			aRay.origin.x = mySize.x - 1;
		}
		break;
	case Direction::Down:
		if (aRay.origin.y >= mySize.y || aRay.origin.x < 0 || aRay.origin.x >= mySize.x)
			return false;
		if (aRay.origin.y < 0)
		{
			cellDistance = -aRay.origin.y;
			aRay.origin.y = 0;
		}
		break;
	case Direction::Up:
		if (aRay.origin.y < 0 || aRay.origin.x < 0 || aRay.origin.x >= mySize.x)
			return false;
		if (aRay.origin.y >= mySize.y)
		{
			cellDistance = aRay.origin.y - mySize.y + 1;
			aRay.origin.y = mySize.y - 1;
		}
		break;
	}

	const int2 dirVec = (int2)GetUnitVectorInDirection(aRay.direction);

	for (int2 cell = aRay.origin; Contains(cell); cell += dirVec, ++cellDistance)
	{
		int value = GetValue(cell);
		if (aMask[value])
		{
			anOutHit.value = value;
			anOutHit.distance = cellDistance;
			return true;
		}
	}

	return false;
}

bool IntGrid::Raycast(Ray<float> aRay, Hit<float>& aHit, IntMask aMask) const
{
	Hit<int> cellHit = {};
	if (!Raycast({ GetContainingCell(aRay.origin), aRay.direction }, cellHit, aMask))
		return false;

	aHit.value = cellHit.value;

	if (cellHit.distance == 0)
	{
		aHit.distance = 0.f;
		return true;
	}

	const float2 cellSize = (float2)myCellSize;
	const float2 dirVec = GetUnitVectorInDirection(aRay.direction);
	const float extent = abs(dot(cellSize, dirVec));

	switch (aRay.direction)
	{
	case Direction::Right:
	case Direction::Down:
		aHit.distance = cellHit.distance * extent;
		break;
	case Direction::Left:
	case Direction::Up:
		aHit.distance = (cellHit.distance - 1) * extent;
		break;
	}

	const float2 remainder = PositiveRemainder(aRay.origin, cellSize);
	aHit.distance -= dot(remainder, dirVec);

	return true;
}

void IntGrid::SignedRaycast(Ray<float> aRay, Hit<float>& aHit, IntMask aMask) const
{
	if (!Raycast(aRay, aHit, aMask) || aHit.distance > 0.f)
		return;

	const int value = aHit.value;

	if (!Raycast({ aRay.origin, GetOppositeDirection(aRay.direction) }, aHit, ~aMask))
		return;

	aHit.distance *= -1.f;
	aHit.value = value;
}


// Private methods

int2 IntGrid::GetContainingCell(float2 aPosition) const
{
	return ::GetContainingCell(aPosition, myCellSize);
}
