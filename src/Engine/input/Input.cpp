#include "pch.h"
#include "input/Input.h"
#include "window/WindowMessage.h"


// Queries

bool Input::KeyDown(Key aCode)
{
    return myKeys[aCode] & Down;
}

bool Input::KeyHeld(Key aCode)
{
    return myKeys[aCode] & Held;
}

bool Input::KeyUp(Key aCode)
{
    return myKeys[aCode] & Up;
}


bool Input::ButtonDown(const std::string& aName)
{
    return myButtons[aName].state & Down;
}

bool Input::ButtonHeld(const std::string& aName)
{
    return myButtons[aName].state & Held;
}

bool Input::ButtonUp(const std::string& aName)
{
    return myButtons[aName].state & Up;
}


float Input::GetAxis(const std::string& aName)
{
    return myAxes[aName].value;
}


// Creation methods

void Input::CreateButton(const std::string& aName, const std::vector<Key>& someKeys)
{
    myButtons[aName] = { someKeys, None };
}

void Input::CreateAxis(const std::string& aName, Key aPositiveKey, Key aNegativeKey)
{
    myAxes[aName] = { aPositiveKey, aNegativeKey, 0.f };
}


// Other public methods

bool Input::Handle(const WindowMessage& aMessage)
{
    switch (aMessage.type)
    {
    case MessageType::KeyDown:
        myKeys[aMessage.key] = (State)(Down | Held);
        return true;
    case MessageType::KeyUp:
        myKeys[aMessage.key] = Up;
        return true;
    default:
        return false;
    }
}

void Input::NewFrame()
{
    for (auto& [name, button] : myButtons)
    {
        button.state = None;

        for (const Key key : button.keys)
        {
            const State state = myKeys[key];

            if (state & Down)
            {
                button.state = (State)(Down | Held);
                break;
            }
            else if (state == Held)
                button.state = Held;
            else if (button.state != Held && state == Up)
                button.state = Up;
        }
    }

    for (auto& [name, state] : myAxes)
        state.value = (float)KeyHeld(state.positiveKey) - (float)KeyHeld(state.negativeKey);
}

void Input::EndFrame()
{
    for (auto& [code, state] : myKeys)
    {
        if (state & Down)
            state = (State)(state & ~Down);

        if (state & Up)
            state = (State)(state & ~Up);
    }
}
