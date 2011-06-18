/*
 * Resource.h
 * Manages resource files
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

#ifndef RESOURCE_H
#define RESOURCE_H

#include <map>
#include <string>

namespace pf {
    class Resource;
#ifdef PLATFORMER_SERVER
    class Server;
#endif

    typedef std::map<std::string, Resource*> ResourceMap;

    class Resource {
    public:
        Resource(char *filename, char *data, int length);
        ~Resource();

        char *GetFilename();
        char *GetData();
        int GetLength();

#ifdef PLATFORMER_SERVER
        static void SetServer(pf::Server *server);
#endif
#ifdef _WIN32
        static char *ConvertToWindowsPath(char *path);
#endif

        static Resource *GetResource(char *name);
        static Resource *GetOrLoadResource(char *name);

    private:
        static ResourceMap *resources;
#ifdef PLATFORMER_SERVER
        static pf::Server *server;
#endif

        char *filename;
        char *data;
        int length;
    };
}; // namespace pf

#endif // RESOURCE_H
