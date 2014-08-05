// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CLogger.h"

CLogger::CLogger()
	: LogLevel(ELL_INFORMATION)
{
}

//! Returns the current set log level.
ELOG_LEVEL CLogger::getLogLevel() const
{
	return LogLevel;
}

//! Sets a new log level.
void CLogger::setLogLevel(ELOG_LEVEL ll)
{
	LogLevel = ll;
}

//! Prints out a text into the log
void CLogger::log(const c8* text, ELOG_LEVEL ll)
{
	if (ll < LogLevel)
		return;
	Printer::print(text);
}


//! Prints out a text into the log
void CLogger::log(const c8* text, const c8* hint, ELOG_LEVEL ll)
{
	if (ll < LogLevel)
		return;

	stringc s = text;
	s += ": ";
	s += hint;
	log (s.c_str(), ll);
}

//! Prints out a text into the log
void CLogger::log(const wchar_t* text, ELOG_LEVEL ll)
{
	if (ll < LogLevel)
		return;

	Printer::print(text);
}


//! Prints out a text into the log
void CLogger::log(const wchar_t* text, const wchar_t* hint, ELOG_LEVEL ll)
{
	if (ll < LogLevel)
		return;

	stringw s = text;
	s += L": ";
	s += hint;
	log (s.c_str(), ll);
}


