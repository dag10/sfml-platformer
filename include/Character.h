/*
 * Character.h
 * Manages a character, its animation, and state
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

#ifndef CHARACTER_H
#define CHARACTER_H

#include <SFML/Graphics.hpp>
#include <PhysicsEntity.h>

namespace pf {
    class Animation;

    class Character : public pf::PhysicsEntity {
        public:
            // Direction
            const static int LEFT = -1;
            const static int RIGHT = 1;

            Character(pf::World *world);
            ~Character();

            void Tick(float frametime);
            void Render(sf::RenderTarget& target);

            int GetDirection();
            bool IsWalking();

            void WalkRight();
            void WalkLeft();
            void StopWalking();
            void StartWalking();
            void FaceRight();
            void FaceLeft();

        private:
            sf::Image *spriteSheet;
            pf::Animation *animationWalking;

            float speed;
            bool walking;
            int direction;
    };
}; // namespace pf

#endif // CHARACTER_H
