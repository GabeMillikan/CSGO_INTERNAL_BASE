#include "Console.hpp"

HANDLE _out = NULL;
HANDLE _old_out = NULL;
HANDLE _err = NULL;
HANDLE _old_err = NULL;
HANDLE _in = NULL;
HANDLE _old_in = NULL;

void Con::Attach()
{
	_old_out = GetStdHandle(STD_OUTPUT_HANDLE);
	_old_err = GetStdHandle(STD_ERROR_HANDLE);
	_old_in = GetStdHandle(STD_INPUT_HANDLE);

	::AllocConsole() && ::AttachConsole(GetCurrentProcessId());

	_out = GetStdHandle(STD_OUTPUT_HANDLE);
	_err = GetStdHandle(STD_ERROR_HANDLE);
	_in = GetStdHandle(STD_INPUT_HANDLE);

	SetConsoleMode(_out,
		ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);

	SetConsoleMode(_in,
		ENABLE_INSERT_MODE | ENABLE_EXTENDED_FLAGS |
		ENABLE_PROCESSED_INPUT | ENABLE_QUICK_EDIT_MODE);
}

bool Con::Raw(const char* fmt, ...)
{
	if (!_out)
		return false;

	char buf[1024];
	va_list va;

	va_start(va, fmt);
	_vsnprintf_s(buf, 1024, fmt, va);
	va_end(va);

	return !!WriteConsoleA(_out, buf, static_cast<DWORD>(strlen(buf)), nullptr, nullptr);
}

bool Con::Out(const char* fmt, ...)
{
	return (Raw(fmt) && Raw("\n"));
}

bool Con::OutNumber(int x, bool raw)
{
	if (raw)
		return Raw(std::to_string(x).c_str());
	return Out(std::to_string(x).c_str());
}

bool Con::OutNumber(float x, bool raw)
{
	if (raw)
		return Raw(std::to_string(x).c_str());
	return Out(std::to_string(x).c_str());
}

bool Con::OutVector(SDK::Vector v)
{
	return (\
		Raw("(") &&
		OutNumber(v.x, true) &&
		Raw(", ") &&
		OutNumber(v.y, true) &&
		Raw(", ") &&
		OutNumber(v.z, true) &&
		Raw(")\n")\
		);
}

bool Con::OutQAngle(SDK::QAngle qa)
{
	return (\
		Raw("(") &&
		OutNumber(qa.pitch, true) &&
		Raw(", ") &&
		OutNumber(qa.yaw, true) &&
		Raw(", ") &&
		OutNumber(qa.roll, true) &&
		Raw(")\n")\
		);
}

bool Con::Load(const char* fmt, int delay)
{
	//fake loads are cool right? 

	if (!Raw(fmt))
		return false;

	Sleep((int)(delay / 4));
	if (!Raw("."))
		return false;

	Sleep((int)(delay / 4));
	if (!Raw("."))
		return false;

	Sleep((int)(delay / 4));
	if (!Out("."))
		return false;

	Sleep((int)(delay / 4));
	return true;
}


bool Con::Label(const char* fmt, float toprint)
{
	return (Con::Raw(fmt) &&
		Con::OutNumber(toprint));
}
bool Con::Label(const char* fmt, SDK::Vector toprint)
{
	return (Con::Raw(fmt) &&
		Con::OutVector(toprint));
}
bool Con::Label(const char* fmt, int toprint)
{
	return (Con::Raw(fmt) &&
		Con::OutNumber(toprint));
}
bool Con::Label(const char* fmt, bool toprint)
{
	bool x =  (Con::Raw(fmt) &&
		Con::OutNumber(toprint, true) &&
		Con::Raw(" - "));
	if (toprint)
		return (x && Con::Out("True"));
	else
		return (x && Con::Out("False"));
}