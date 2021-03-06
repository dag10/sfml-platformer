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
#include "Logger.h"
#include "Resource.h"
#include "Character.h"
#include <vector>

pf::World::World(pf::Resource *levelImageResource, pf::Resource *tilesetResource) {
    // Load level layout
    levelImage = new sf::Image();
    if (!(levelImage->LoadFromMemory(levelImageResource->GetData(), levelImageResource->GetLength()))) return;

    // Set level width
    width = levelImage->GetWidth();
    height = levelImage->GetHeight();

    // Initialize platform array
    platforms = (pf::Platform**)new void*[width * height];
    if (!platforms) return;

    // Load platform tileset
    sf::Image *tileset = new sf::Image();
    tileset->LoadFromMemory(tilesetResource->GetData(), tilesetResource->GetLength());
    if (!tileset) return;
    tileset->CreateMaskFromColor(sf::Color::Magenta);

    // Set default spawn point
    spawnX = spawnY = TILE_SIZE;

    // Create tiles
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            platforms[xy(x, y)] = NULL;
            sf::Color levelColor = levelImage->GetPixel(x, y);
            if (Tileset::Spawn == levelColor) {
                spawnX = x * TILE_SIZE;
                spawnY = y * TILE_SIZE;
            } else {
                for (int i = 0; i < Tileset::Count; i++) {
                    if (Tileset::Tiles[i].levelColor == levelColor) {
                        platforms[xy(x, y)] = new pf::Platform(
                                                this,
                                                *tileset,
                                                Tileset::Tiles[i].coords,
                                                x * TILE_SIZE,
                                                y * TILE_SIZE,
                                                Tileset::Tiles[i].alpha,
                                                Tileset::Tiles[i].liquid);
                        platforms[xy(x, y)]->SetSolid(Tileset::Tiles[i].solid);
                        break;
                    }
                }
            }
        }
    }

    // Initialize entities
    entityMap = new EntityMap();
}

void pf::World::Tick(float frametime) {
    for (EntityMap::iterator it = entityMap->begin(); it != entityMap->end(); it++)
        it->second->Tick(frametime);
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
    for (EntityMap::iterator it = entityMap->begin(); it != entityMap->end(); it++) {
        pf::IRenderable *ent = dynamic_cast<IRenderable*>(it->second);
        if (ent) ent->Render(target);
    }

    // Draw front-most platforms
    for (int i = 0; i < liquidPlatforms.size(); i++)
        target.Draw((sf::Sprite)*(liquidPlatforms.at(i)));
}

void pf::World::RenderOverlays(sf::RenderTarget& target) {
    // Draw renderable entities
    for (EntityMap::iterator it = entityMap->begin(); it != entityMap->end(); it++) {
        pf::IRenderable *ent = dynamic_cast<IRenderable*>(it->second);
        if (ent) ent->RenderOverlays(target);
    }
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
    pf::PhysicsEntity *physEnt = skip ? dynamic_cast<pf::PhysicsEntity*>(skip) : NULL;

    // Starting test tile for top-left of entity
    int baseX = x / TILE_SIZE, baseY = y / TILE_SIZE;

    // Loop through all entities and check for collisions
    for (EntityMap::iterator it = entityMap->begin(); it != entityMap->end(); it++) {
        pf::PhysicsEntity *ent = dynamic_cast<pf::PhysicsEntity*>(it->second);
        if (ent && ent != skip && ent->HitTest(x, y, width, height) && ((!physEnt) || physEnt->CanCollideWith(ent)))
            retVec.push_back((pf::Entity*)ent);
    }

    // Loop through all tile indices that entity it touching
    for (int i = 0; i < width / TILE_SIZE + 2; i++) {
        for (int j = 0; j < height / TILE_SIZE + 2; j++) {
            if (xy(baseX + i, baseY + j) < 0 || xy(baseX + i, baseY + j) >= this->width * this->height)
                continue;
            pf::Platform *platform = platforms[xy(baseX + i, baseY + j)];
            if (platform && platform != skip && platform->HitTest(x, y, width, height) && ((!physEnt) || physEnt->CanCollideWith(platform)))
                retVec.push_back((pf::Entity*)platform);
        }
    }

    return retVec;
}

std::vector<pf::Entity*> pf::World::HitsLevel(float x, float y, pf::Entity *skip) {
    std::vector<pf::Entity*> retVec;
    pf::PhysicsEntity *physEnt = skip ? dynamic_cast<pf::PhysicsEntity*>(skip) : NULL;

    // Starting test tile for top-left of entity
    int baseX = x / TILE_SIZE, baseY = y / TILE_SIZE;

    // Loop through all entities and check for collisions
    for (EntityMap::iterator it = entityMap->begin(); it != entityMap->end(); it++) {
        pf::PhysicsEntity *ent = dynamic_cast<pf::PhysicsEntity*>(it->second);
        if (ent && ent != skip && ent->HitTest(x, y) && ((!physEnt) || physEnt->CanCollideWith(ent)))
            retVec.push_back((pf::Entity*)ent);
    }

    // Loop through all tile indices that entity it touching
    for (int i = 0; i < width / TILE_SIZE + 2; i++) {
        for (int j = 0; j < height / TILE_SIZE + 2; j++) {
            if (xy(baseX + i, baseY + j) < 0 || xy(baseX + i, baseY + j) >= this->width * this->height)
                continue;
            pf::Platform *platform = platforms[xy(baseX + i, baseY + j)];
            if (platform && platform != skip && platform->HitTest(x, y) && ((!physEnt) || physEnt->CanCollideWith(platform)))
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

void pf::World::AddEntity(pf::Entity *entity) {
    entityMap->insert(std::pair<int, pf::Entity*>(entity->GetID(), entity));
}

pf::Entity *pf::World::GetEntity(int id) {
    EntityMap::iterator iter = entityMap->find(id);

    if (iter == entityMap->end())
        return NULL;

    return iter->second;
}

bool pf::World::RemoveEntity(pf::Entity& entity) {
    entityMap->erase(entity.GetID());
}

void pf::World::RemovePlatform(pf::Platform& platform) {
    platforms[xy((int)platform.GetX() / TILE_SIZE, (int)platform.GetY() / TILE_SIZE)] = 0;
}

void pf::World::SpawnCharacter(pf::Character *character) {
    AddEntity(character);
    character->SetPosition(spawnX, spawnY);
}

pf::World::~World() {
    if (entityMap) {
        entityMap->empty();
        delete entityMap;
        entityMap = NULL;
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

int pf::World::GetPixelWidth() {
    return width * TILE_SIZE;
}

int pf::World::GetPixelHeight() {
    return height * TILE_SIZE;
}

int pf::World::GetWidth() {
    return width;
}

int pf::World::GetHeight() {
    return height;
}

float pf::World::GetSpawnX() {
    return spawnX;
}

float pf::World::GetSpawnY() {
    return spawnY;
}
