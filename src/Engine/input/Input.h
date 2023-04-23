#pragma once
#include "window/MessageHandler.h"
#include "window/Key.h"
#include <unordered_map>
#include <string>
#include <vector>


class Input : public MessageHandler
{
public:
	bool KeyDown(Key);
	bool KeyHeld(Key);
	bool KeyUp(Key);

	bool ButtonDown(const std::string& aName);
	bool ButtonHeld(const std::string& aName);
	bool ButtonUp(const std::string& aName);

	float GetAxis(const std::string& aName);

	void CreateButton(const std::string& aName, const std::vector<Key>& someKeys);
	void CreateAxis(const std::string& aName, Key aPositiveKey, Key aNegativeKey);

	bool Handle(const WindowMessage&) override;
	void NewFrame();
	void EndFrame();

private:
	enum State
	{
		None = 0,
		Down = 1 << 0,
		Held = 1 << 1,
		Up   = 1 << 2
	};

	struct Button
	{
		std::vector<Key> keys;
		State state;
	};

	struct Axis
	{
		Key positiveKey;
		Key negativeKey;
		float value;
	};

	std::unordered_map<Key, State> myKeys;
	std::unordered_map<std::string, Button> myButtons;
	std::unordered_map<std::string, Axis> myAxes;
};
