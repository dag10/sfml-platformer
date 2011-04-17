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

#include <Character.h>
#include <Animation.h>

pf::Character::Character(pf::World *world)
    : pf::PhysicsEntity(world) {
    speed = 110.f;
    walking = false;
    direction = RIGHT;

    const int framerate = speed / 15;

    spriteSheet = new sf::Image();
    spriteSheet->LoadFromFile("resources/character_01.bmp");
    spriteSheet->SetSmooth(false);
    spriteSheet->CreateMaskFromColor(sf::Color::Magenta);

    animationWalking = new pf::Animation(*spriteSheet, 6, framerate);
    animationWalking->Pause();

    width = animationWalking->GetWidth();
    height = animationWalking->GetHeight();
}

pf::Character::~Character() {

}

void pf::Character::Tick(float frametime) {
    if (walking) animationWalking->Play();
    animationWalking->Tick(frametime);
    pf::PhysicsEntity::Tick(frametime);

    // If stopped moving sideways (hit wall), stop walking
    if (GetVelocityX() == 0.f)
        StopWalking();
}

void pf::Character::Render(sf::RenderTarget& target) {
    sprite = (sf::Sprite*)animationWalking;
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
    animationWalking->Pause();
    animationWalking->SetCurrentFrame(0);
}

void pf::Character::StartWalking() {
    walking = true;
    SetVelocityX(direction * speed);
    animationWalking->Play();
}

void pf::Character::FaceRight() {
    direction = RIGHT;
    if (walking)
        SetVelocityX(speed);
    animationWalking->FlipX(false);
}

void pf::Character::FaceLeft() {
    direction = LEFT;
    if (walking)
        SetVelocityX(-speed);
    animationWalking->FlipX(true);
}
