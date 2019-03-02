#include "Color.h"

Color::Color()
	:color(0)
{}

Color::Color(unsigned int val)
	: color(val)
{}

Color::Color(BYTE r, BYTE g, BYTE b)
	: Color(r, g, b, 255)
{
}

Color::Color(BYTE r, BYTE g, BYTE b, BYTE a)
{
	rgba[0] = r;
	rgba[1] = g;
	rgba[2] = b;
	rgba[3] = a;
}

Color::Color(const Color& src)
	:color(src.color)
{}

Color& Color::operator=(const Color& src)
{
	color = src.color;
	return *this;
}

bool Color::operator==(const Color& rhs) const
{
	return (color == rhs.color);
}

bool Color::operator!=(const Color& rhs) const
{
	return !(*this == rhs);
}

constexpr BYTE Color::GetR() const
{
	return rgba[0];
}
void Color::SetR(BYTE r)
{
	rgba[0] = r;
}

constexpr BYTE Color::GetG() const
{
	return rgba[1];
}
void Color::SetG(BYTE g)
{
	rgba[1] = g;
}

constexpr BYTE Color::GetB() const
{
	return rgba[2];
}
void Color::SetB(BYTE b)
{
	rgba[2] = b;
}

constexpr BYTE Color::GetA() const
{
	return rgba[3];
}
void Color::SetA(BYTE a)
{
	rgba[3] = a;
}