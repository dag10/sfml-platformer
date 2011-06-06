/*
 * Entity.h
 * Base class for any entity in the world
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

#ifndef ENTITY_H
#define ENTITY_H

namespace pf {
    class World;

    class Entity {
        public:
            Entity(pf::World *world);
            Entity(pf::World *world, float x, float y, int width, int height);
            ~Entity();

            virtual inline void Tick(float frametime) {};

            void SetPosition(float x, float y);
            void SetSize(int width, int height);

            void SetX(float x);
            void SetY(float y);
            void SetWidth(int width);
            void SetHeight(int height);

            float GetX();
            float GetY();
            int GetWidth();
            int GetHeight();

            int GetID();
            void SetID(int id);

        protected:
            static unsigned int NEXT_ENT_ID;

            void Init(pf::World *world, float x, float y, int width, int height);

            unsigned int id;
            pf::World *world;
            float x, y;
            int width, height;
    };
}; // namespace pf

#endif // ENTITY_H
