/*
 * Logger.h
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

#ifndef LOGGER_H
#define LOGGER_H

namespace pf {
    namespace Logger {
        void PrintTimestamp();
        
        void LogInfo(const char *format, ...);
        void LogWarning(const char *format, ...);
        void LogError(const char *format, ...);
        void LogFatal(const char *format, ...);
    };
}; // namespace pf

#endif // LOGGER_H
