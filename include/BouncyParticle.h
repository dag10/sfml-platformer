/*
 * BouncyParticle.h
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

#ifndef BOUNCYPARTICLE_H
#define BOUNCYPARTICLE_H

#include "Particle.h"

namespace pf {
    class BouncyParticle : public pf::Particle {
    public:
        BouncyParticle(pf::World *world);
        BouncyParticle(pf::World *world, pf::Animation *image);
        BouncyParticle(pf::World *world, float x, float y);
        BouncyParticle(pf::World *world, pf::Animation *image, float x, float y);
        ~BouncyParticle();
        
        void Init();
    };
}; // namespace pf

#endif // BOUNCYPARTICLE_H