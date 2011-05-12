/*
 * Elevator.h
 * A PhysicsEntity that moves up and down until it hits something
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

#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <SFML/Graphics.hpp>
#include "PhysicsEntity.h"

namespace pf {
    class Animation;
    
    class Elevator : public pf::PhysicsEntity {
    public:
        Elevator(pf::World *world, pf::Animation *image, float x, float y);
        ~Elevator();
        
        void Tick(float frametime);
    
    private:
        int direction;
        float speed;
    };
}; // namespace pf

#endif // ELEVATOR_H
