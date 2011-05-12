/*
 * Particle.h
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

#ifndef PARTICLE_H
#define PARTICLE_H

#include "PhysicsEntity.h"
#include "IRenderable.h"

namespace pf {
    class Animation;
    
    class Particle : public pf::PhysicsEntity {
    public:
        Particle(pf::World *world);
        Particle(pf::World *world, pf::Animation *image);
        Particle(pf::World *world, float x, float y);
        Particle(pf::World *world, pf::Animation *image, float x, float y);
        ~Particle();
        
        void Init();
        
        void Tick(float frametime);
        void Render(sf::RenderTarget& target);
        
    protected:
        pf::Animation *image;
    };
}; // namespace pf

#endif // PARTICLE_H
