/*
 * Logger.cpp
 * Handles client and server logging
 * Copyright (c) 2010-2011 Drew Gottlieb
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "Logger.h"
#include <time.h>
#include <iostream>
#include <stdarg.h>
#include <cstdio>

void pf::Logger::PrintTimestamp() {
    static char timestampString[16];
    time_t *rawtime = new time_t();
    time(rawtime);
    struct tm *timeinfo = localtime(rawtime);
    strftime(timestampString, sizeof(timestampString), " [ %X ]", timeinfo);
    std::cout << timestampString;
}

void pf::Logger::LogInfo(const char *format, ...) {
    PrintTimestamp();

    if(format[0] == '\0')
        return;
    char vstr[512];
    va_list args;

    va_start(args, format);
    vsprintf(vstr, format, args);
    va_end(args);

    std::cout << " [INFO] " << vstr << std::endl;
}

void pf::Logger::LogWarning(const char *format, ...) {
    PrintTimestamp();

    if(format[0] == '\0')
        return;
    char vstr[512];
    va_list args;

    va_start(args, format);
    vsprintf(vstr, format, args);
    va_end(args);

    std::cout << " [WARNING] " << vstr << std::endl;
}

void pf::Logger::LogError(const char *format, ...) {
    PrintTimestamp();

    if(format[0] == '\0')
        return;
    char vstr[512];
    va_list args;

    va_start(args, format);
    vsprintf(vstr, format, args);
    va_end(args);

    std::cout << " [ERROR] " << vstr << std::endl;
}

void pf::Logger::LogFatal(const char *format, ...) {
    PrintTimestamp();

    if(format[0] == '\0')
        return;
    char vstr[512];
    va_list args;

    va_start(args, format);
    vsprintf(vstr, format, args);
    va_end(args);

    std::cout << " [FATAL] " << vstr << std::endl;
}
