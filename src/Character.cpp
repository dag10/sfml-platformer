/*
 * Character.cpp
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

#include "Character.h"
#include "Animation.h"

pf::Character::Character(pf::World *world)
    : pf::PhysicsEntity(world) {
    speed = WALK_SPEED;
    walking = false;
    direction = RIGHT;

    const int framerate = speed / 15;

    spriteSheet = new sf::Image();
    spriteSheet->LoadFromFile("resources/character_01.bmp");
    spriteSheet->SetSmooth(false);
    spriteSheet->CreateMaskFromColor(sf::Color::Magenta);

    image = new pf::Animation(*spriteSheet, 6, framerate);
    image->Pause();

    width = image->GetWidth();
    height = image->GetHeight();
        
    SetPushable(true);
}

pf::Character::~Character() {

}

void pf::Character::Tick(float frametime) {
    // Set horizontal speed
    //speed = inLiquid ? SWIM_SPEED : WALK_SPEED;
    if (inLiquid)
        speed = SWIM_SPEED;
    else
        speed = WALK_SPEED;
    
    if (walking) image->Play();
    pf::PhysicsEntity::Tick(frametime);

    // If stopped moving sideways (hit wall), stop walking
    if (GetVelocityX() == 0.f)
        StopWalking();
}

void pf::Character::Render(sf::RenderTarget& target) {
    //image->SetColor(IsInLiquid() ? sf::Color::Red : sf::Color::Green);
    //image = animationWalking;
    pf::PhysicsEntity::Render(target);
    //animationWalking->SetPosition((int)x, (int)y);
    //animationWalking->Render(target);
}

int pf::Character::GetDirection() {
    return direction;
}

bool pf::Character::IsWalking() {
    return walking;
}

void pf::Character::WalkRight() {
    FaceRight();
    StartWalking();
}

void pf::Character::WalkLeft() {
    FaceLeft();
    StartWalking();
}

void pf::Character::StopWalking() {
    walking = false;
    SetVelocityX(0.f);
    image->Pause();
    image->SetCurrentFrame(0);
}

void pf::Character::StartWalking() {
    walking = true;
    SetVelocityX(direction * speed);
    image->Play();
}

void pf::Character::FaceRight() {
    direction = RIGHT;
    if (walking)
        SetVelocityX(speed);
    image->FlipX(false);
}

void pf::Character::FaceLeft() {
    direction = LEFT;
    if (walking)
        SetVelocityX(-speed);
    image->FlipX(true);
}
