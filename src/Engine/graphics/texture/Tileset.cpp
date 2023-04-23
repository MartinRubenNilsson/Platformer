#include "pch.h"
#include "graphics/texture/Tileset.h"

#define ZERO_RECT { { 0.f, 0.f }, { 0.f, 0.f } }


using json = nlohmann::json;


void from_json(const json& j, Rect& aRect)
{
    j.at("x").get_to(aRect.min.x);
    j.at("y").get_to(aRect.min.y);
    j.at("w").get_to(aRect.max.x);
    j.at("h").get_to(aRect.max.y);
    aRect.max += aRect.min;
}

void to_json(json& j, const Rect& aRect)
{
    j["x"] = aRect.min.x;
    j["y"] = aRect.min.y;
    j["w"] = aRect.GetWidth();
    j["h"] = aRect.GetHeight();
}

void from_json(const json& j, AnimationDirection& aDirection)
{
    const std::string str = j.get<std::string>();

    if (str == "forward")
        aDirection = AnimationDirection::Forward;
    else if (str == "reverse")
        aDirection = AnimationDirection::Reverse;
    else if (str == "pingpong")
        aDirection = AnimationDirection::PingPong;
}

void to_json(json& j, const AnimationDirection& aDirection)
{
    switch (aDirection)
    {
    case AnimationDirection::Forward:
        j["direction"] = "forward";
        break;
    case AnimationDirection::Reverse:
        j["direction"] = "reverse";
        break;
    case AnimationDirection::PingPong:
        j["direction"] = "pingpong";
        break;
    }
}


NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Frame, frame, duration);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(FrameTag, name, from, to, direction);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SliceKey, bounds, frame);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Slice, name, keys);



// Constructor & destructor

Tileset::Tileset() = default;

Tileset::~Tileset() = default;


// Public methods

bool Tileset::LoadFile(const fs::path& aPath)
{
    if (!Image::LoadFile(aPath))
        return false;

    fs::path jsonPath = aPath;
    jsonPath.replace_extension("json");

    LoadJsonFile(jsonPath);

    return true;
}


// Queries

bool Tileset::HasTag(const std::string& aName) const
{
    return rn::count(myTags, aName, &FrameTag::name);
}

bool Tileset::HasSlice(const std::string& aName) const
{
    return rn::count(mySlices, aName, &Slice::name);
}


// Getters

Rect Tileset::GetFrame(const std::string& aTag, unsigned aTime) const
{
    if (auto tag = FindTag(aTag))
        return myFrames[CalculateFrame(*tag, aTime)].frame;
    
    return ZERO_RECT;
}

Rect Tileset::GetFrame(char c) const
{
    std::string tagName;
    unsigned offset = 0;

    if ('0' <= c && c <= '9')
    {
        tagName = "digits";
        offset = c - '0';
    }
    else if ('A' <= c && c <= 'Z')
    {
        tagName = "uppercase";
        offset = c - 'A';
    }
    else if ('a' <= c && c <= 'z')
    {
        tagName = "lowercase";
        offset = c - 'a';
    }
    else // assume c is punctuation
    {
        tagName = c;
        offset = 0;
    }

    if (!HasTag(tagName))
    {
        tagName = " ";
        offset = 0;
    }

    if (auto tag = FindTag(tagName))
        return myFrames[tag->from + offset].frame;

    return ZERO_RECT;
}

Rect Tileset::GetSlice(const std::string& aSlice, const std::string& aTag, unsigned aTime) const
{
    auto slice = FindSlice(aSlice);
    if (!slice)
        return ZERO_RECT;

    auto tag = FindTag(aTag);
    if (!tag)
        return ZERO_RECT;

    unsigned frame = CalculateFrame(*tag, aTime);

    for (const SliceKey& key : slice->keys)
    {
        if (frame >= key.frame)
            return key.bounds;
    }

    return slice->keys.front().bounds;
}


// Private methods

bool Tileset::LoadJsonFile(const fs::path& aPath)
{
    if (aPath.extension() != ".json")
        return false;

    std::ifstream i(aPath);
    if (!i.is_open())
        return false;

    json j;
    i >> j;

    assert(j.at("frames").is_array());
    j.at("frames").get_to(myFrames);

    const json& meta = j.at("meta");

    if (meta.contains("frameTags"))
        meta.at("frameTags").get_to(myTags);

    if (meta.contains("slices"))
        meta.at("slices").get_to(mySlices);

    return true;
}

unsigned Tileset::CalculateDuration(const FrameTag& aTag) const
{
    unsigned duration = 0;

    for (unsigned frame = aTag.from; frame <= aTag.to; ++frame)
        duration += myFrames[frame].duration;

    if (aTag.direction == AnimationDirection::PingPong)
    {
        duration *= 2;
        duration -= myFrames[aTag.from].duration;
        duration -= myFrames[aTag.to].duration;
    }

    return duration;
}

unsigned Tileset::CalculateFrame(const FrameTag& aTag, unsigned aTime) const
{
    aTime %= CalculateDuration(aTag);

    if (aTag.direction == AnimationDirection::Reverse)
        goto REVERSE_PASS;

// FORWARD_PASS:
    for (unsigned frame = aTag.from; frame <= aTag.to; ++frame)
    {
        if (aTime < myFrames[frame].duration)
            return frame;
        else
            aTime -= myFrames[frame].duration;
    }

    assert(aTag.direction != AnimationDirection::Forward);

REVERSE_PASS:
    for (unsigned frame = aTag.to - 1; frame >= aTag.from; --frame)
    {
        if (aTime < myFrames[frame].duration)
            return frame;
        else
            aTime -= myFrames[frame].duration;
    }

    assert(false);
    return 0;
}

const FrameTag* Tileset::FindTag(const std::string& aName) const
{
   auto tag = rn::find(myTags, aName, &FrameTag::name);
   return tag != myTags.end() ? &*tag : nullptr;
}

const Slice* Tileset::FindSlice(const std::string& aName) const
{
    auto slice = rn::find(mySlices, aName, &Slice::name);
    return slice != mySlices.end() ? &*slice : nullptr;
}
