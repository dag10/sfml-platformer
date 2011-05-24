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
#include <fstream>
#include <sys/stat.h>
#include <iostream>

pf::ResourceMap *pf::Resource::resources = new pf::ResourceMap();

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
        std::cerr << "Failed to stat file: " << name << std::endl;
        return NULL;
    }
    
    char *buffer = new char[length];
    
    std::ifstream *in_stream = new std::ifstream(name, std::ifstream::in | std::ifstream::binary);
    
    if (!in_stream->read(buffer, length)) {
        std::cerr << "Failed to read file: " << name << std::endl;
        delete [] buffer;
        in_stream->close();
        delete in_stream;
        return NULL;
    }
    
    in_stream->close();
    delete in_stream;
    
    return new Resource(name, buffer, length);
};

pf::Resource::Resource(char *filename, char *data, int length) {
    this->filename = filename;
    this->data = data;
    this->length = length;
    resources->insert(std::pair<std::string, pf::Resource*>(std::string(filename), this));
    
    std::cout << "Created resource: " << filename << " ( " << length << " bytes )" << std::endl;
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
