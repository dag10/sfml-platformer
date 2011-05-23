/*
 * BouncyParticle.cpp
 * A particle that bounces off of objects
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

#include "BouncyParticle.h"

pf::BouncyParticle::BouncyParticle(pf::World *world)
: pf::Particle(world) {
    Init();
}

pf::BouncyParticle::BouncyParticle(pf::World *world, float x, float y)
: pf::Particle(world, x, y) {
    Init();
}

pf::BouncyParticle::BouncyParticle(pf::World *world, pf::Animation *image)
: pf::Particle(world, image) {
    Init();
}

pf::BouncyParticle::BouncyParticle(pf::World *world, pf::Animation *image, float x, float y)
: pf::Particle(world, image, x, y) {
    Init();
}

pf::BouncyParticle::~BouncyParticle() {
    
}

void pf::BouncyParticle::Init() {
    SetGravityEnabled(false);
    SetPushable(false);
    SetSolid(true);
    //SetVelocity(10.f, 10.f);
}
