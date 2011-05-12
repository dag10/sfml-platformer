/*
 * Particle.cpp
 * The base class for renderable particles
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

#include "Particle.h"
#include "Animation.h"

pf::Particle::Particle(pf::World *world)
: pf::PhysicsEntity(world) {
    image = NULL;
    
    Init();
}

pf::Particle::Particle(pf::World *world, float x, float y)
: pf::PhysicsEntity(world, x, y, 0, 0) {
    image = NULL;
    
    Init();
}

pf::Particle::Particle(pf::World *world, pf::Animation *image)
    : pf::PhysicsEntity(world, 0, 0, 0, 0) {
    this->image = image;
    
    Init();
}

pf::Particle::Particle(pf::World *world, pf::Animation *image, float x, float y)
: pf::PhysicsEntity(world, x, y, 0, 0) {
    this->image = image;
    
    Init();
}

void pf::Particle::Init() {
    if (image) image->Play();
}

pf::Particle::~Particle() {
    
}

void pf::Particle::Tick(float frametime) {
    image->Tick(frametime);
    pf::PhysicsEntity::Tick(frametime);
}

void pf::Particle::Render(sf::RenderTarget &target) {
    image->SetPosition(x, y);
    image->Render(target);
}