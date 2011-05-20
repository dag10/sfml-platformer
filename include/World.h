/*
 * World.h
 * Manages a world, including its entities, platforms, and related resources.
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

#ifndef WORLD_H
#define WORLD_H

#include "IRenderable.h"
#include "Resource.h"
#include <vector>

namespace pf {
    class Entity;
    class Platform;

    class World : public pf::IRenderable {
        public:
            const static int TILE_SIZE = 16;
            const static int STEP_HEIGHT = 6;

            World(Resource *levelImageResource);
            ~World();

            void Tick(float frametime);
            void Render(sf::RenderTarget& target);
            std::vector<pf::Entity*> HitsLevel(pf::Entity& entity);
            std::vector<pf::Entity*> HitsLevel(float x, float y, float width, float height, pf::Entity *skip);
            std::vector<pf::Entity*> HitsLevel(float x, float y, pf::Entity *skip);
            std::vector<pf::Entity*> HitsPlatform(pf::Entity& entity);
            std::vector<pf::Entity*> HitsPlatform(float x, float y, float width, float height);
            std::vector<pf::Entity*> HitsPlatform(float x, float y);

            void AddEntity(pf::Entity& entity);
            pf::Entity *GetEntity(int id);
            std::map<int, pf::Entity*> *getEntityMap();
        
            int GetPixelWidth();
            int GetPixelHeight();
            int GetWidth();
            int GetHeight();

        protected:
            int width, height;
            sf::Image *levelImage;
            pf::Platform **platforms;
            std::vector<pf::Entity*> *entities;
            std::map<int, pf::Entity*> *entityMap;
    };
}; // namespace pf

#endif // WORLD_H
