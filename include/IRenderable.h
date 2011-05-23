/*
 * IRenderable.h
 * An interface for all renderable objects
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

#ifndef IRENDERABLE_H
#define IRENDERABLE_H

#include <SFML/Graphics.hpp>

namespace pf {
    class IRenderable {
        public:
            virtual void Tick(float frametime) {};
            virtual void Render(sf::RenderTarget& target) = 0;
            virtual void RenderOverlays(sf::RenderTarget& target) {};
    };
}; // namespace pf

#endif // IRENDERABLE_H
