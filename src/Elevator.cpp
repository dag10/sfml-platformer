/*
 * Elevator.cpp
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

#include "Elevator.h"

pf::Elevator::Elevator(pf::World *world, pf::Animation *image, float x, float y)
: pf::PhysicsEntity(world, image, x, y) {
    speed = 50;
    direction = 1;
    
    SetGravityEnabled(false);
    SetPushable(false);
}

pf::Elevator::~Elevator() {
    
}

void pf::Elevator::Tick(float frametime) {
    //if (world->HitsLevel(*this).size() > 0) {
    if (GetVelocityY() == 0) {
        direction *= -1;
        SetVelocityY(direction * speed);
        //pf::PhysicsEntity::Move(GetVelocityX(), GetVelocityY());
    }
    
    SetVelocityY(direction * speed);
    pf::PhysicsEntity::Tick(frametime);
}
