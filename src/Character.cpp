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
#include <string>

sf::Font *pf::Character::nameFont = 0;

pf::Character::Character(pf::World *world, pf::Resource *spriteResource, const char *name)
    : pf::PhysicsEntity(world)
{
    speed = WALK_SPEED;
    walking = false;
    direction = RIGHT;

    const int framerate = speed / 15;

    spriteSheet = new sf::Image();
    spriteSheet->LoadFromMemory(spriteResource->GetData(), spriteResource->GetLength());
    spriteSheet->SetSmooth(false);
    spriteSheet->CreateMaskFromColor(sf::Color::Magenta);

    image = new pf::Animation(*spriteSheet, 6, framerate);
    image->Pause();

    width = image->GetWidth();
    height = image->GetHeight();
        
    if (nameFont == NULL) {
        pf::Resource *fontResource = pf::Resource::GetOrLoadResource("resources/MIASWFTE.TTF");
        nameFont = new sf::Font();
        nameFont->LoadFromMemory(fontResource->GetData(), fontResource->GetLength(), 30);
    }
        
    static sf::Color nameBackgroundFill = sf::Color(0, 0, 0, 100);
    nameBackground = new sf::Shape();
    nameBackground->AddPoint(0, 0, nameBackgroundFill);
    nameBackground->AddPoint(1, 0, nameBackgroundFill);
    nameBackground->AddPoint(1, 1, nameBackgroundFill);
    nameBackground->AddPoint(0, 1, nameBackgroundFill);
    nameBackground->EnableFill(true);
    
    this->name = new sf::String();
    this->name->SetSize(6.f);
    this->name->SetFont(*nameFont);
    
    if (name) {
        SetName(name);
        ShowName();
    } else {
        HideName();
    }
    
    SetPushable(true);
}

pf::Character::~Character() {

}

void pf::Character::Tick(float frametime) {
    // Set horizontal speed
    //speed = inLiquid ? SWIM_SPEED : WALK_SPEED; // Why doesn't this work?
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
    pf::PhysicsEntity::Render(target);
}

void pf::Character::RenderOverlays(sf::RenderTarget& target) {
    if (showName) {
        name->SetPosition(x + (width / 2) - (name->GetRect().GetWidth() / 2), y - name->GetRect().GetHeight() - 2);
        
        nameBackground->SetPosition(name->GetPosition());
        nameBackground->Move(0.f, 1.5f);
        target.Draw(*nameBackground);
        
        name->SetColor(sf::Color(50, 50, 50));
        target.Draw(*name);
        
        name->SetColor(sf::Color::White);
        name->Move(0.5f, 0.5f);
        target.Draw(*name);
    }
}

void pf::Character::SetName(const char *name) {
    this->name->SetText(name);
    nameBackground->SetScale(this->name->GetRect().GetWidth(), this->name->GetRect().GetHeight());
}

char *pf::Character::GetName() {
    return (char *)((std::string)name->GetText()).c_str();
}

void pf::Character::ShowName() {
    showName = true;
}

void pf::Character::HideName() {
    showName = false;
}

bool pf::Character::IsShowingName() {
    return showName;
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
