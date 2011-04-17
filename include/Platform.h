/*
 * Platform.h
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

#ifndef PLATFORM_H
#define PLATFORM_H

#include <SFML/Graphics.hpp>
#include <Entity.h>

namespace pf {
    class Platform : public pf::Entity, public sf::Sprite {
        public:
            Platform(pf::World* world, sf::Image& image, sf::IntRect rect);
            Platform(pf::World* world, sf::Image& image, sf::IntRect rect, int x, int y);
            ~Platform();

            void SetSolid(bool solid);
            bool IsSolid();

            // HitTest should be overridden for more complex Platforms that
            // derive off of this class (e.g. slants, stairs, doors, etc)
            bool HitTest(pf::Entity& entity);
            bool HitTest(float x, float y, float width, float height);
            bool HitTest(float x, float y);

        private:
            void Init(sf::Image& image, sf::IntRect rect, int x, int y);
            bool solid;
    };
}; // namespace pf

#endif // PLATFORM_H
