/*
 * Entity.cpp
 * Base class for any entity in the world
 * Used with
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

#include "Entity.h"
#include "World.h"

unsigned int pf::Entity::NEXT_ENT_ID = 0;

pf::Entity::Entity(pf::World *world) {
    id = NEXT_ENT_ID++;
    Init(world, 0, 0, 0, 0);
}

pf::Entity::Entity(pf::World *world, float x, float y, int width, int height) {
    id = NEXT_ENT_ID++;
    Init(world, x, y, width, height);
}

pf::Entity::~Entity() {
    if (world) world->RemoveEntity(*this);
}

void pf::Entity::Init(pf::World *world, float x, float y, int width, int height) {
    //id = NEXT_ENT_ID++;

    this->world = world;
    SetPosition(x, y);
    SetSize(width, height);
}

void pf::Entity::SetPosition(float x, float y) {
    this->x = x;
    this->y = y;
}

void pf::Entity::SetSize(int width, int height) {
    this->width = width;
    this->height = height;
}

void pf::Entity::SetX(float x) {
    this->x = x;
}

void pf::Entity::SetY(float y) {
    this->y = y;
}

void pf::Entity::SetWidth(int width) {
    this->width = width;
}

void pf::Entity::SetHeight(int height) {
    this->height = height;
}

void pf::Entity::SetID(int id) {
    this->id = id;
    if (id > NEXT_ENT_ID)
        NEXT_ENT_ID = id + 1;
}

float pf::Entity::GetX() {
    return x;
}

float pf::Entity::GetY() {
    return y;
}

int pf::Entity::GetWidth() {
    return width;
}

int pf::Entity::GetHeight() {
    return height;
}

int pf::Entity::GetID() {
    return this->id;
}