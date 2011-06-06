/*
 * Resource.cpp
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

#include "Resource.h"
#include "Logger.h"
#include "Server.h"
#include <fstream>
#include <sys/stat.h>
#include <iostream>

pf::ResourceMap *pf::Resource::resources = new pf::ResourceMap();
#ifdef PLATFORMER_SERVER
pf::Server *pf::Resource::server = NULL;
#endif

pf::Resource *pf::Resource::GetResource(char *name) {
    ResourceMap::iterator iter = resources->find(std::string(name));
    
    if (iter == resources->end())
        return NULL;
    
    return iter->second;
};

pf::Resource *pf::Resource::GetOrLoadResource(char *name) {
    pf::Resource *resource = GetResource(name);
    if (resource) return resource;
    
    struct stat results;
    int length;
    
    if (stat(name, &results) == 0)
        length = results.st_size;
    else {
        pf::Logger::LogError("Failed to stat file: %s", name);
        return NULL;
    }
    
    char *buffer = new char[length];
    
    std::ifstream *in_stream = new std::ifstream(name, std::ifstream::in | std::ifstream::binary);
    
    if (!in_stream->read(buffer, length)) {
        pf::Logger::LogError("Failed to read file: %s", name);
        delete [] buffer;
        in_stream->close();
        delete in_stream;
        return NULL;
    }
    
    in_stream->close();
    delete in_stream;
    
    pf::Logger::LogInfo("Loaded resource: \"%s\" ( %d bytes )", name, length);
    
    return new Resource(name, buffer, length);
};

pf::Resource::Resource(char *filename, char *data, int length) {
    this->filename = filename;
    this->length = length;
    this->data = data;
    resources->insert(std::pair<std::string, pf::Resource*>(std::string(filename), this));
#ifdef PLATFORMER_SERVER
    if (server) server->RequireResource(this);
#endif
}

pf::Resource::~Resource() {
    resources->erase(filename);
    delete [] filename;
    delete [] data;
}

char *pf::Resource::GetFilename() {
    return filename;
}

char *pf::Resource::GetData() {
    return data;
}

int pf::Resource::GetLength() {
    return length;
}

#ifdef PLATFORMER_SERVER
void pf::Resource::SetServer(pf::Server *server) {
    pf::Resource::server = server;
}
#endif