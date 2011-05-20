/*
 * Platform.cpp
 * Manages a platform in the world
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

#include "Platform.h"
#include "World.h"

pf::Platform::Platform(pf::World* world, sf::Image& image, sf::IntRect rect, float alpha, bool liquid)
    : pf::Entity(world, 0, 0, pf::World::TILE_SIZE, pf::World::TILE_SIZE) {
    Init(image, rect, 0, 0, alpha, liquid);
}

pf::Platform::Platform(pf::World* world, sf::Image& image, sf::IntRect rect, int x, int y, float alpha, bool liquid)
    : pf::Entity(world, (float)x, (float)y, pf::World::TILE_SIZE, pf::World::TILE_SIZE) {
    Init(image, rect, x, y, alpha, liquid);
}

void pf::Platform::Init(sf::Image& image, sf::IntRect rect, int x, int y, float alpha, bool liquid) {
    this->solid = true;
    this->liquid = liquid;

    char tintR, tintG, tintB;
    if (liquid)
        tintR = tintG = tintB = (200 - (int)(((float)y / (float)world->GetPixelHeight()) * (float)200)) + 55;
    else
        tintR = tintG = tintB = 255;
    
    this->SetImage(image);
    image.SetSmooth(false);
    this->SetSubRect(rect);
    this->Resize(pf::World::TILE_SIZE, pf::World::TILE_SIZE);
    this->SetColor(sf::Color(tintR, tintG, tintB, (int)(alpha * 255)));

    this->sf::Sprite::SetPosition(x, y);
}

pf::Platform::~Platform() {

}

void pf::Platform::SetSolid(bool solid) {
    this->solid = solid;
}

bool pf::Platform::IsSolid() {
    return this->solid;
}

bool pf::Platform::IsLiquid() {
    return this->liquid;
}

bool pf::Platform::HitTest(pf::Entity& entity) {
    return HitTest(entity.GetX(),
                   entity.GetY(),
                   entity.GetWidth(),
                   entity.GetHeight());
}

bool pf::Platform::HitTest(float x, float y, float width, float height) {
    if (!solid) return false;

    int myX = this->GetPosition().x, myY = this->GetPosition().y;

    if (x >= myX + pf::World::TILE_SIZE
        || y >= myY + pf::World::TILE_SIZE)
        return false;

    if (x + width <= myX || y + height <= myY)
        return false;

    return true;
}

bool pf::Platform::HitTest(float x, float y) {
    if (!solid) return false;

    return (!(x >= this->GetPosition().x + pf::World::TILE_SIZE
        || y >= this->GetPosition().y + pf::World::TILE_SIZE));
}
