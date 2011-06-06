/*
  * CharacterSkin.cpp
  * Manages character skins
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

#include "CharacterSkin.h"
#include "Resource.h"
#include <SFML/Graphics.hpp>

pf::CharacterSkinMap *pf::CharacterSkin::characterSkins = new pf::CharacterSkinMap();

pf::CharacterSkin::CharacterSkin(char *name, pf::Resource *skin, int framerate, int frames) {
    this->name = name;
    this->skin = skin;
    this->framerate = framerate;
    this->frames = frames;
    
    sf::Image *tempImage = new sf::Image();
    tempImage->LoadFromMemory(skin->GetData(), skin->GetLength());
    width = tempImage->GetWidth() / frames;
    height = tempImage->GetHeight();
    delete tempImage;
    
    characterSkins->insert(std::pair<std::string, pf::CharacterSkin*>(std::string(name), this));
}

pf::CharacterSkin::~CharacterSkin() {
    delete [] name;
    characterSkins->erase(name);
}

char *pf::CharacterSkin::GetName() {
    return name;
}

pf::Resource *pf::CharacterSkin::GetResource() {
    return skin;
}

int pf::CharacterSkin::GetWidth() {
    return width;
}

int pf::CharacterSkin::GetHeight() {
    return height;
}

int pf::CharacterSkin::GetFramerate() {
    return framerate;
}

int pf::CharacterSkin::GetFrames() {
    return frames;
}

pf::CharacterSkin *pf::CharacterSkin::GetCharacterSkin(char *name) {
    CharacterSkinMap::iterator iter = characterSkins->find(std::string(name));
    
    if (iter == characterSkins->end())
        return NULL;
    
    return iter->second;
}

pf::CharacterSkinMap *pf::CharacterSkin::GetCharacterSkinMap() {
    return characterSkins;
}