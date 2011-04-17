/*
 * Game.h
 * Manages a game session. Contains level, player(s), and entities
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

#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>

namespace pf {
    class Character;
    class PhysicsEntity;
    class World;

    class Game {
        public:
            Game();
            ~Game();

            void Render(sf::RenderTarget& target, int renderWidth, int renderHeight);
            void Tick(sf::Input& input, float frametime);

        private:
            pf::World *world;
            sf::View *view;
            float viewX, viewY;
            float viewSpeed;
            sf::RenderTarget *lastTarget;
            pf::Character *mainCharacter;
            pf::PhysicsEntity *box;
    };
}; // namespace pf

#endif // GAME_H