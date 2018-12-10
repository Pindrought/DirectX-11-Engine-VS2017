#pragma once
class Color
{
public:
	constexpr Color()
		:
		value()
	{}
	constexpr Color(unsigned int val)
		:
		value(val)
	{}
	constexpr Color(unsigned char r, unsigned char g, unsigned char b)
		:
		Color((r << 16u) | (g << 8u) | b)
	{}
	constexpr Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
		:
		Color((a << 24u) | (r << 16u) | (g << 8u) | b)
	{}
	constexpr Color(const Color& src)
		:
		value(src.value)
	{}
	Color& operator=(const Color& src)
	{
		value = src.value;
		return *this;
	}
	bool operator==(const Color& rhs) const
	{
		return (value << 8u) == (rhs.value << 8u);
	}
	bool operator!=(const Color& rhs) const
	{
		return !(*this == rhs);
	}

	constexpr unsigned char GetA() const
	{
		return (value >> 24u);
	}
	void SetA(unsigned char a)
	{
		value = (value & 0x00FFFFFF) | (a << 24u);
	}

	constexpr unsigned char GetR() const
	{
		return (value >> 16u) & 0xFFu;
	}
	void SetR(unsigned char r)
	{
		value = (value & 0xFF00FFFF) | (r << 16u);
	}

	constexpr unsigned char GetG() const
	{
		return (value >> 8u) & 0xFFu;
	}
	void SetG(unsigned char g)
	{
		value = (value & 0xFFFF00FF) | (g << 8u);
	}

	constexpr unsigned char GetB() const
	{
		return (value) & 0xFFu;
	}
	void SetB(unsigned char b)
	{
		value = (value & 0xFFFFFF00) | b;
	}

	constexpr unsigned int GetValue() const
	{
		return value;
	}
	constexpr void SetValue(unsigned int newval)
	{
		value = newval;
	}
private:
	unsigned int value;
};

namespace Colors
{
	const Color UnloadedTextureColor(100, 100, 100);
	const Color UnhandledTextureColor(250, 0, 0);
}