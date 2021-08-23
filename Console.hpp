#pragma once
#include <Windows.h>
#include <string>
#include "Vector.hpp"
#include "QAngle.hpp"

extern HANDLE _out;
extern HANDLE _old_out;
extern HANDLE _err;
extern HANDLE _old_err;
extern HANDLE _in;
extern HANDLE _old_in;

namespace Con
{
	void Attach();
	bool Raw(const char* fmt, ...);
	bool Out(const char* fmt, ...);
	bool OutNumber(int x, bool raw = false);
	bool OutNumber(float x, bool raw = false);
	bool Load(const char* fmt, int delay = 100);
	bool OutVector(SDK::Vector v);
	bool OutQAngle(SDK::QAngle q);
	bool Label(const char* fmt, float toprint);
	bool Label(const char* fmt, int toprint);
	bool Label(const char* fmt, SDK::Vector toprint);
	bool Label(const char* fmt, bool toprint);
}