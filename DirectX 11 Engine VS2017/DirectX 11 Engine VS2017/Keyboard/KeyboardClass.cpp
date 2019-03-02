#include "KeyboardClass.h"

KeyboardClass::KeyboardClass()
{
	for (int i = 0; i < 256; i++)
		keyStates[i] = false; //Initialize all key states to off (false)
}

bool KeyboardClass::KeyIsPressed(const unsigned char keycode)
{
	return keyStates[keycode];
}

bool KeyboardClass::KeyBufferIsEmpty()
{
	return keyBuffer.empty();
}

bool KeyboardClass::CharBufferIsEmpty()
{
	return charBuffer.empty();
}

KeyboardEvent KeyboardClass::ReadKey()
{
	if (keyBuffer.empty()) //If no keys to be read?
	{
		return KeyboardEvent(); //return empty keyboard event
	}
	else
	{
		KeyboardEvent e = keyBuffer.front(); //Get first Keyboard Event from queue
		keyBuffer.pop(); //Remove first item from queue
		return e; //Returns keyboard event
	}
}

unsigned char KeyboardClass::ReadChar()
{
	if (charBuffer.empty()) //If no keys to be read?
	{
		return 0u; //return 0 (NULL char)
	}
	else
	{
		unsigned char e = charBuffer.front(); //Get first char from queue
		charBuffer.pop(); //Remove first char from queue
		return e; //Returns char
	}
}

void KeyboardClass::OnKeyPressed(const unsigned char key)
{
	keyStates[key] = true;
	keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Press, key));
}

void KeyboardClass::OnKeyReleased(const unsigned char key)
{
	keyStates[key] = false;
	keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Release, key));
}

void KeyboardClass::OnChar(const unsigned char key)
{
	charBuffer.push(key);
}

void KeyboardClass::EnableAutoRepeatKeys()
{
	autoRepeatKeys = true;
}

void KeyboardClass::DisableAutoRepeatKeys()
{
	autoRepeatKeys = false;
}

void KeyboardClass::EnableAutoRepeatChars()
{
	autoRepeatChars = true;
}

void KeyboardClass::DisableAutoRepeatChars()
{
	autoRepeatChars = false;
}

bool KeyboardClass::IsKeysAutoRepeat()
{
	return autoRepeatKeys;
}

bool KeyboardClass::IsCharsAutoRepeat()
{
	return autoRepeatChars;
}