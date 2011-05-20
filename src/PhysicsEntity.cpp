/*
 * PhysicsEntity.cpp
 * A physics-enabled renderable entity
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

#include "PhysicsEntity.h"
#include "Platform.h"
#include "Animation.h"
#include <vector>

pf::PhysicsEntity::PhysicsEntity(pf::World *world)
    : pf::Entity(world) {
    image = NULL;

    Init();
}

pf::PhysicsEntity::PhysicsEntity(pf::World *world, float x, float y, int width, int height)
    : pf::Entity(world, x, y, width, height) {
    image = NULL;

    Init();
}

pf::PhysicsEntity::PhysicsEntity(pf::World *world, pf::Animation *image)
    : pf::Entity(world, 0, 0, image->GetWidth(), image->GetHeight()) {
        this->image = image;

    Init();
}

pf::PhysicsEntity::PhysicsEntity(pf::World *world, pf::Animation *image, float x, float y)
: pf::Entity(world, x, y, image->GetWidth(), image->GetHeight()) {
    this->image = image;

    Init();
}

pf::PhysicsEntity::~PhysicsEntity() {
    if (image) {
        delete image;
        image = NULL;
    }
}

void pf::PhysicsEntity::Init() {
    veloX = veloY = 0.f;
    wasHittingHorizontalSurface = false;
    wasHittingVerticalSurface = false;
    onGround = false;
    hitEntities = new std::vector<pf::Entity*>();
    canUseStairs = false;
    if (image) image->Play();
}

void pf::PhysicsEntity::Tick(float frametime) {
    static float terminalLiquidX = 5.0f;
    static float terminalLiquidY = 5.0f;
    
    hitEntities->clear();
    
    // Gravity
    if (gravity && !onGround)
        veloY += GRAVITY * frametime;

    // Bottom surface friction
    if (solid && onGround && veloX != 0)
        veloX *= 0.95;
    
    // Liquid resistance
    if (inLiquid) {
        if (veloX > terminalLiquidX || veloX < -terminalLiquidX) veloX *= 0.8f;
        if (veloY > terminalLiquidY || veloY < -terminalLiquidY) veloY *= 0.8f;
    }
    
    Move(veloX * frametime, veloY * frametime);

    // Clip velocity to zero if it's very near
    if (veloX > -0.001f && veloX < 0.001f) veloX = 0.f;
    if (veloY > -0.001f && veloY < 0.001f) veloY = 0.f;
    
    if (image) image->Tick(frametime);
}

bool pf::PhysicsEntity::AlreadyHit(pf::Entity *entity) {
    for (int i = 0; i < hitEntities->size(); i++)
        if (hitEntities->at(i) == entity)
            return true;

    return false;
}

void pf::PhysicsEntity::Push(float offsetX, float offsetY) {
    if (!pushable) return;
    
    Move(offsetX, offsetY);

    veloX += offsetX;
    veloY += offsetY;
}

void pf::PhysicsEntity::Move(float offsetX, float offsetY) {
    inLiquid = false;
    
    if (offsetX != 0.f || offsetY != 0.f) {
        if (solid) {
            if (offsetY < 0.f) onGround = false;

            std::vector<pf::Entity*> ents;
            std::vector<pf::PhysicsEntity*> willAdd;

            // Move up/down and check for collision
            if (offsetY != 0) {
                y += offsetY;
                ents = world->HitsLevel(x, y, width, height, (pf::Entity*)this);
                if (ents.size() > 0) {
                    for (int i = 0; i < ents.size(); i++) {
                        pf::Entity *ent = ents.at(i);
                        pf::PhysicsEntity *pEnt = dynamic_cast<pf::PhysicsEntity*>(ent);
                        pf::Platform *platform = dynamic_cast<pf::Platform*>(ent);
                        if (platform && platform->IsLiquid()) {
                            inLiquid = true;
                        } else {
                            if (pEnt) {
                                if (!pEnt->AlreadyHit(this) && !AlreadyHit(pEnt)) {
                                    if (offsetY > 0.f) {
                                        //if (pEnt->IsPushable()) {
                                            pEnt->Push(0.f, (y + height) - pEnt->GetY());
                                            pEnt->veloY = 0;
                                        //}
                                        y = pEnt->GetY() - height;
                                        onGround = true;
                                    } else if (offsetY < 0.f) {
                                        //if (pEnt->IsPushable()) {
                                            pEnt->Push(0.f, y - (pEnt->GetY() + pEnt->GetHeight()));
                                            pEnt->veloY = 0;
                                        //}
                                        y = pEnt->GetY() + pEnt->GetHeight();
                                    }
                                    willAdd.push_back(pEnt);
                                }
                            } else if (offsetY > 0.f) {
                                y = ent->GetY() - height - 0.0f;
                                onGround = true;
                            } else
                                y = ent->GetY() + ent->GetHeight() + 0.0f;
                            veloY = 0.f;
                        }
                    }
                } else if (offsetY > 0.f)
                        onGround = false;
            }

            // Move left/right and check for collision
            if (offsetX != 0) {
                x += offsetX;
                ents = world->HitsLevel(x, y, width, height, (pf::Entity*)this);
                if (ents.size() > 0) {
                    for (int i = 0; i < ents.size(); i++) {
                        pf::Entity *ent = ents.at(i);
                        pf::PhysicsEntity *pEnt = dynamic_cast<pf::PhysicsEntity*>(ent);
                        pf::Platform *platform = dynamic_cast<pf::Platform*>(ent);
                        if (platform && platform->IsLiquid()) {
                            inLiquid = true;
                        } else if (canUseStairs && IsOnGround() && !pEnt && veloX != 0.f &&
                            (y + height) - ent->GetY() <= pf::World::STEP_HEIGHT && (y + height) > ent->GetY() &&
                            world->HitsLevel(x, ent->GetY() - height, width, height, this).size() == 0) {
                            y = ent->GetY() - height;
                        } else if (pEnt) {
                            if (!pEnt->AlreadyHit(this) && !AlreadyHit(pEnt)) {
                                float origX = pEnt->GetX();
                                float preMoveX = x;
                                if (offsetX > 0.f) {
                                    pEnt->Push((x + width) - pEnt->GetX(), 0.f);
                                    x = pEnt->GetX() - width;
                                } else if (offsetX < 0.f) {
                                    pEnt->Push(x - (pEnt->GetX() + pEnt->GetWidth()), 0.f);
                                    x = pEnt->GetX() + pEnt->GetWidth();
                                }
                                /*if (pEnt->GetX() == origX && canUseStairs && IsOnGround() && veloX != 0.f &&
                                    (y + height) - ent->GetY() <= pf::World::STEP_HEIGHT && (y + height) > ent->GetY() &&
                                    world->HitsLevel(x + offsetX, ent->GetY() - height, width, height, this).size() == 0) {
                                    y = ent->GetY() - height;
                                    x += offsetX;
                                }*/
                                if (x != preMoveX && canUseStairs && IsOnGround() && veloX != 0.f &&
                                    (y + height) - ent->GetY() <= pf::World::STEP_HEIGHT && (y + height) > ent->GetY() &&
                                    world->HitsLevel(x, ent->GetY() - height, width, height, this).size() == 0) {
                                    y = ent->GetY() - height;
                                    x += offsetX;
                                }
                                hitEntities->push_back(pEnt);
                            }
                        } else if (offsetX > 0.f)
                            x = ent->GetX() - width - 0.0f;
                        else
                            x = ent->GetX() + ent->GetWidth() + 0.0f;
                        veloX = 0.f;
                    }
                }
            }
            /*
            if (ent = world->HitsLevel(x, y, width, height, (pf::Entity*)this)) {
                pf::PhysicsEntity *pEnt = dynamic_cast<pf::PhysicsEntity*>(ent);
                if (canUseStairs && IsOnGround() && veloX != 0.f &&
                    (y + height) - ent->GetY() <= pf::World::STEP_HEIGHT && (y + height) > ent->GetY() &&
                    !world->HitsLevel(x, ent->GetY() - height, width, height, this)) {
                    y = ent->GetY() - height;
                } else if (pEnt) {
                    if (!pEnt->AlreadyHit(this) && !AlreadyHit(pEnt)) {
                        if (offsetX > 0.f) {
                            pEnt->Push((x + width) - pEnt->GetX(), 0.f);
                            x = pEnt->GetX() - width;
                        } else if (offsetX < 0.f) {
                            pEnt->Push(x - (pEnt->GetX() + pEnt->GetWidth()), 0.f);
                            x = pEnt->GetX() + pEnt->GetWidth();
                        }
                        hitEntities->push_back((pf::Entity*)pEnt);
                    }
                } else if (offsetX > 0.f)
                    x = ent->GetX()  - width - 0.0f;
                else
                    x = ent->GetX() + ent->GetWidth() + 0.0f;
                veloX = 0.f;
            }
            */

            // Re-add original collision entities
            for (int i = 0; i < willAdd.size(); i++)
                hitEntities->push_back(willAdd.at(i));

        } else {
            x += offsetX;
            y += offsetY;
        }
    }

    // Re-check onGround if moved horizontally but not vertically
    if (solid && offsetY == 0.f && veloY == 0.f) {
        std::vector<pf::Entity*> hits = world->HitsLevel(x, y + 1, width, height, (pf::Entity*)this);
        onGround = false;
        for (int i = 0; i < hits.size(); i++) {
            pf::Platform *platform = dynamic_cast<pf::Platform*>(hits.at(i));
            if (platform && platform->IsLiquid())
                continue;
            onGround = true;
        }
    }
}

bool pf::PhysicsEntity::HitTest(pf::Entity& entity) {
    return HitTest(entity.GetX(),
                   entity.GetY(),
                   entity.GetWidth(),
                   entity.GetHeight());
}

bool pf::PhysicsEntity::HitTest(float x, float y, float width, float height) {
    if (!solid) return false;

    if (x >= this->x + this->width
        || y >= this->y + this->height)
        return false;

    if (x + width <= this->x || y + height <= this->y)
        return false;

    return true;
}

bool pf::PhysicsEntity::HitTest(float x, float y) {
    if (!solid) return false;

    return (!(x >= this->x + this->width
        || y >= this->y + this->height));
}

void pf::PhysicsEntity::Render(sf::RenderTarget& target) {
    if (!image) return;

    /*
    if (IsOnGround)
        image->SetColor(sf::Color::Red);
    else
        image->SetColor(sf::Color::White);
     */
    
    image->SetPosition((int)x, (int)y);
    image->Render(target);
}

bool pf::PhysicsEntity::IsMoving() {
    return (veloX == 0.f && veloY == 0.f);
}

float pf::PhysicsEntity::GetVelocityX() {
    return veloX;
}

float pf::PhysicsEntity::GetVelocityY() {
    return veloY;
}

void pf::PhysicsEntity::SetVelocity(float veloX, float veloY) {
    this->veloX = veloX;
    this->veloY = veloY;
}

void pf::PhysicsEntity::SetVelocityX(float veloX) {
    this->veloX = veloX;
}

void pf::PhysicsEntity::SetVelocityY(float veloY) {
    this->veloY = veloY;
}

bool pf::PhysicsEntity::IsSolid() {
    return solid;
}

void pf::PhysicsEntity::SetSolid(bool solid) {
    this->solid = solid;
}

bool pf::PhysicsEntity::GravityIsEnabled() {
    return gravity;
}

void pf::PhysicsEntity::SetGravityEnabled(bool gravity) {
    this->gravity = gravity;
}

bool pf::PhysicsEntity::IsOnGround() {
    return onGround || inLiquid;
}

bool pf::PhysicsEntity::IsInLiquid() {
    return inLiquid;
}

bool pf::PhysicsEntity::IsPushable() {
    return pushable;
}

void pf::PhysicsEntity::SetPushable(bool pushable) {
    this->pushable = pushable;
}

bool pf::PhysicsEntity::CanUseStairs() {
    return canUseStairs;
}

void pf::PhysicsEntity::SetCanUseStairs(bool canUseStairs) {
    this->canUseStairs = canUseStairs;
}
