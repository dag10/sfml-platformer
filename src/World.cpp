/*
 * World.cpp
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

#define xy(x,y) (y)*(this->width)+(x)

#include "World.h"
#include "Tileset.h"
#include "Entity.h"
#include "PhysicsEntity.h"
#include "Platform.h"
#include <vector>

pf::World::World(char *level) {
    // Load level layout
    levelImage = new sf::Image();
    if (!(levelImage->LoadFromFile(level))) return;

    // Set level width
    width = levelImage->GetWidth();
    height = levelImage->GetHeight();

    // Initialize platform array
    platforms = (pf::Platform**)new void*[width * height];
    if (!platforms) return;

    // Load platform tileset
    sf::Image *tileset = new sf::Image();
    tileset->LoadFromFile("resources/tileset.bmp");
    if (!tileset) return;
    tileset->CreateMaskFromColor(sf::Color::Magenta);

    // Create tiles
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            platforms[xy(x, y)] = NULL;
            sf::Color levelColor = levelImage->GetPixel(x, y);
            for (int i = 0; i < TilesetCount; i++) {
                if (Tileset[i].levelColor == levelColor) {
                    platforms[xy(x, y)] = new pf::Platform(
                                            this,
                                            *tileset,
                                            Tileset[i].coords,
                                            x * TILE_SIZE,
                                            y * TILE_SIZE,
                                            Tileset[i].alpha,
                                            Tileset[i].liquid);
                    platforms[xy(x, y)]->SetSolid(Tileset[i].solid);
                    break;
                }
            }
        }
    }

    // Initialize entities
    entities = new std::vector<pf::Entity*>();
}

void pf::World::Tick(float frametime) {
    for (int i = 0; i < entities->size(); i++)
        entities->at(i)->Tick(frametime);
}

void pf::World::Render(sf::RenderTarget& target) {
    std::vector<pf::Platform*> liquidPlatforms;
    
    // Draw platforms
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
            if (platforms[xy(x, y)])
                if (platforms[xy(x, y)]->IsLiquid())
                    liquidPlatforms.push_back(platforms[xy(x, y)]);
                else
                    target.Draw((sf::Sprite)*(platforms[xy(x, y)]));

    // Draw renderable entities
    for (int i = 0; i < entities->size(); i++) {
        pf::IRenderable *ent = dynamic_cast<IRenderable*>(entities->at(i));
        if (ent) ent->Render(target);
    }
    
    // Draw front-most platforms
    for (int i = 0; i < liquidPlatforms.size(); i++)
        target.Draw((sf::Sprite)*(liquidPlatforms.at(i)));
}

std::vector<pf::Entity*> pf::World::HitsLevel(pf::Entity& entity) {
    return HitsLevel(entity.GetX(),
                     entity.GetY(),
                     entity.GetWidth(),
                     entity.GetHeight(),
                     &entity);
}

std::vector<pf::Entity*> pf::World::HitsLevel(float x, float y, float width, float height, pf::Entity *skip) {
    std::vector<pf::Entity*> retVec;

    // Starting test tile for top-left of entity
    int baseX = x / TILE_SIZE, baseY = y / TILE_SIZE;

    // Loop through all entities and check for collisions
    for (int i = 0; i < entities->size(); i++) {
        pf::PhysicsEntity *ent = dynamic_cast<pf::PhysicsEntity*>(entities->at(i));
        if (ent && ent != skip && ent->HitTest(x, y, width, height))
                retVec.push_back((pf::Entity*)ent);
    }

    // Loop through all tile indices that entity it touching
    for (int i = 0; i < width / TILE_SIZE + 2; i++) {
        for (int j = 0; j < height / TILE_SIZE + 2; j++) {
            if (xy(baseX + i, baseY + j) < 0 || xy(baseX + i, baseY + j) >= this->width * this->height)
                continue;
            pf::Platform *platform = platforms[xy(baseX + i, baseY + j)];
            if (platform && platform != skip && platform->HitTest(x, y, width, height))
                retVec.push_back((pf::Entity*)platform);
        }
    }

    return retVec;
}

std::vector<pf::Entity*> pf::World::HitsLevel(float x, float y, pf::Entity *skip) {
    std::vector<pf::Entity*> retVec;

    // Starting test tile for top-left of entity
    int baseX = x / TILE_SIZE, baseY = y / TILE_SIZE;

    // Loop through all entities and check for collisions
    for (int i = 0; i < entities->size(); i++) {
        pf::PhysicsEntity *ent = dynamic_cast<pf::PhysicsEntity*>(entities->at(i));
        if (ent && ent != skip && ent->HitTest(x, y))
                retVec.push_back((pf::Entity*)ent);
    }

    // Loop through all tile indices that entity it touching
    for (int i = 0; i < width / TILE_SIZE + 2; i++) {
        for (int j = 0; j < height / TILE_SIZE + 2; j++) {
            if (xy(baseX + i, baseY + j) < 0 || xy(baseX + i, baseY + j) >= this->width * this->height)
                continue;
            pf::Platform *platform = platforms[xy(baseX + i, baseY + j)];
            if (platform && platform != skip && platform->HitTest(x, y))
                retVec.push_back((pf::Entity*)platform);
        }
    }

    return retVec;
}

std::vector<pf::Entity*> pf::World::HitsPlatform(pf::Entity& entity) {
    return HitsPlatform(entity.GetX(),
                        entity.GetY(),
                        entity.GetWidth(),
                        entity.GetHeight());
}

std::vector<pf::Entity*> pf::World::HitsPlatform(float x, float y, float width, float height) {
    std::vector<pf::Entity*> retVec;

    // Starting test tile for top-left of entity
    int baseX = x / TILE_SIZE, baseY = y / TILE_SIZE;

    // Loop through all tile indices that entity it touching
    for (int i = 0; i < width / TILE_SIZE + 2; i++) {
        for (int j = 0; j < height / TILE_SIZE + 2; j++) {
            if (xy(baseX + i, baseY + j) < 0 || xy(baseX + i, baseY + j) >= this->width * this->height)
                continue;
            pf::Platform *platform = platforms[xy(baseX + i, baseY + j)];
            if (platform && platform->HitTest(x, y, width, height))
                retVec.push_back((pf::Entity*)platform);
        }
    }

    return retVec;
}

std::vector<pf::Entity*> pf::World::HitsPlatform(float x, float y) {
    std::vector<pf::Entity*> retVec;

    // Starting test tile for top-left of entity
    int baseX = x / TILE_SIZE, baseY = y / TILE_SIZE;

    // Loop through all tile indices that entity it touching
    for (int i = 0; i < width / TILE_SIZE + 2; i++) {
        for (int j = 0; j < height / TILE_SIZE + 2; j++) {
            if (xy(baseX + i, baseY + j) < 0 || xy(baseX + i, baseY + j) >= this->width * this->height)
                continue;
            pf::Platform *platform = platforms[xy(baseX + i, baseY + j)];
            if (platform && platform->HitTest(x, y))
                retVec.push_back((pf::Entity*)platform);
        }
    }

    return retVec;
}

void pf::World::AddEntity(pf::Entity& entity) {
    if (GetEntity(entity.GetID()))
        return;

    entities->push_back(&entity);
}

pf::Entity *pf::World::GetEntity(int id) {
    for (int i = 0; i < entities->size(); i++)
        if (entities->at(i)->GetID() == id)
            return entities->at(i);

    return NULL;
}

pf::World::~World() {
    if (entities) {
        entities->empty();
        delete entities;
        entities = NULL;
    }
    if (levelImage) {
        delete levelImage;
        levelImage = NULL;
    }
    if (platforms) {
        delete [] platforms;
        platforms = NULL;
    }
}
