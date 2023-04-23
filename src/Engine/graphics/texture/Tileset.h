#pragma once
#include "math/Rect.h"
#include "graphics/texture/Image.h"
#include <string>
#include <vector>


struct Frame
{
	Rect frame; // pixels
	unsigned duration; // ms
};

enum class AnimationDirection
{
	Forward,
	Reverse,
	PingPong
};

struct FrameTag
{
	std::string name;
	unsigned from; // frame
	unsigned to; // frame
	AnimationDirection direction;
};

struct SliceKey
{
	Rect bounds; // pixels
	unsigned frame;
};

struct Slice
{
	std::string name;
	std::vector<SliceKey> keys;
};


class Tileset : public Image
{
public:
	Tileset();
	~Tileset();

	bool LoadFile(const fs::path&) override;

	bool HasTag(const std::string& aName) const;
	bool HasSlice(const std::string& aName) const;

	Rect GetFrame(const std::string& aTag, unsigned aTime) const;
	Rect GetFrame(char c) const; // Assuming tileset is a font.
	Rect GetSlice(const std::string& aSlice, const std::string& aTag, unsigned aTime) const;

private:
	bool LoadJsonFile(const fs::path&);

	unsigned CalculateDuration(const FrameTag&) const;
	unsigned CalculateFrame(const FrameTag&, unsigned aTime) const;

	const FrameTag* FindTag(const std::string& aName) const;
	const Slice* FindSlice(const std::string& aName) const;

	std::vector<Frame> myFrames;
	std::vector<FrameTag> myTags;
	std::vector<Slice> mySlices;
};