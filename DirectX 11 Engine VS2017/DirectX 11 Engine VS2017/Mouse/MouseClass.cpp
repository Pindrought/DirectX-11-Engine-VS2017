#include "MouseClass.h"

void MouseClass::OnLeftPressed(int x, int y)
{
	leftIsDown = true;
	MouseEvent me(MouseEvent::EventType::LPress, x, y);
	eventBuffer.push(me);
}

void MouseClass::OnLeftReleased(int x, int y)
{
	leftIsDown = false;
	eventBuffer.push(MouseEvent(MouseEvent::EventType::LRelease, x, y));
}

void MouseClass::OnRightPressed(int x, int y)
{
	rightIsDown = true;
	eventBuffer.push(MouseEvent(MouseEvent::EventType::RPress, x, y));
}

void MouseClass::OnRightReleased(int x, int y)
{
	rightIsDown = false;
	eventBuffer.push(MouseEvent(MouseEvent::EventType::RRelease, x, y));
}

void MouseClass::OnMiddlePressed(int x, int y)
{
	mbuttonDown = true;
	eventBuffer.push(MouseEvent(MouseEvent::EventType::MPress, x, y));
}

void MouseClass::OnMiddleReleased(int x, int y)
{
	mbuttonDown = false;
	eventBuffer.push(MouseEvent(MouseEvent::EventType::MRelease, x, y));
}

void MouseClass::OnWheelUp(int x, int y)
{
	eventBuffer.push(MouseEvent(MouseEvent::EventType::WheelUp, x, y));
}

void MouseClass::OnWheelDown(int x, int y)
{
	eventBuffer.push(MouseEvent(MouseEvent::EventType::WheelDown, x, y));
}

void MouseClass::OnMouseMove(int x, int y)
{
	x = x;
	y = y;
	eventBuffer.push(MouseEvent(MouseEvent::EventType::Move, x, y));
}

void MouseClass::OnMouseMoveRaw(int x, int y)
{
	eventBuffer.push(MouseEvent(MouseEvent::EventType::RAW_MOVE, x, y));
}

bool MouseClass::IsLeftDown()
{
	return leftIsDown;
}

bool MouseClass::IsMiddleDown()
{
	return mbuttonDown;
}

bool MouseClass::IsRightDown()
{
	return rightIsDown;
}

int MouseClass::GetPosX()
{
	return x;
}

int MouseClass::GetPosY()
{
	return y;
}

MousePoint MouseClass::GetPos()
{
	return{ x, y };
}

bool MouseClass::EventBufferIsEmpty()
{
	return eventBuffer.empty();
}

MouseEvent MouseClass::ReadEvent()
{
	if (eventBuffer.empty())
	{
		return MouseEvent();
	}
	else
	{
		MouseEvent e = eventBuffer.front(); //Get first event from buffer
		eventBuffer.pop(); //Remove first event from buffer
		return e;
	}
}