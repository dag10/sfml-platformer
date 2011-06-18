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
#include "Resource.h"
#include "CharacterSkin.h"
#include "Logger.h"
#include <string>
#ifdef PLATFORMER_SERVER
#include "ClientInstance.h"
#include "Packet.h"
#include "Server.h"
#endif

sf::Font *pf::Character::nameFont = 0;

pf::Character::Character(pf::World *world, pf::CharacterSkin *skin, const char *name)
    : pf::PhysicsEntity(world)
{
    speed = WALK_SPEED;
    walking = false;
    direction = RIGHT;
    health = 100;

    int framerate = speed / skin->GetFramerate();

    this->skin = skin;
    spriteSheet = new sf::Image();
    pf::Resource *spriteResource = skin->GetResource();
    spriteSheet->LoadFromMemory(spriteResource->GetData(), spriteResource->GetLength());
    spriteSheet->SetSmooth(false);
    spriteSheet->CreateMaskFromColor(sf::Color::Magenta);

    image = new pf::Animation(*spriteSheet, skin->GetFrames(), framerate);
    image->Pause();

    width = image->GetWidth();
    height = image->GetHeight();

#ifdef PLATFORMER_CLIENT
    if (nameFont == NULL) {
        pf::Resource *fontResource = pf::Resource::GetOrLoadResource("resources/MIASWFTE.TTF");
        nameFont = new sf::Font();
        nameFont->LoadFromMemory(fontResource->GetData(), fontResource->GetLength(), 30);
    }
#endif

    static sf::Color nameBackgroundFill = sf::Color(0, 0, 0, 100);
    nameBackground = new sf::Shape();
    nameBackground->AddPoint(0, 0, nameBackgroundFill);
    nameBackground->AddPoint(1, 0, nameBackgroundFill);
    nameBackground->AddPoint(1, 1, nameBackgroundFill);
    nameBackground->AddPoint(0, 1, nameBackgroundFill);
    nameBackground->EnableFill(true);

    healthBackground = new sf::Shape();
    healthBackground->AddPoint(0, 0, nameBackgroundFill);
    healthBackground->AddPoint(1, 0, nameBackgroundFill);
    healthBackground->AddPoint(1, 1, nameBackgroundFill);
    healthBackground->AddPoint(0, 1, nameBackgroundFill);
    healthBackground->EnableFill(true);

    static sf::Color healthBarGreenFill = sf::Color(0, 255, 0, 100);
    healthBarGreen = new sf::Shape();
    healthBarGreen->AddPoint(0, 0, healthBarGreenFill);
    healthBarGreen->AddPoint(1, 0, healthBarGreenFill);
    healthBarGreen->AddPoint(1, 1, healthBarGreenFill);
    healthBarGreen->AddPoint(0, 1, healthBarGreenFill);
    healthBarGreen->EnableFill(true);

    static sf::Color healthBarRedFill = sf::Color(255, 0, 0, 100);
    healthBarRed = new sf::Shape();
    healthBarRed->AddPoint(0, 0, healthBarRedFill);
    healthBarRed->AddPoint(1, 0, healthBarRedFill);
    healthBarRed->AddPoint(1, 1, healthBarRedFill);
    healthBarRed->AddPoint(0, 1, healthBarRedFill);
    healthBarRed->EnableFill(true);

    this->name = new sf::String();
    this->name->SetSize(6.f);
#ifdef PLATFORMER_CLIENT
    this->name->SetFont(*nameFont);
#endif

    if (name) {
        SetName(name);
        ShowName();
    } else {
        HideName();
    }
    //ShowHealth();

    SetPushable(true);
    SetGravityEnabled(false);
    SetSolid(true);
    SetIsolateAnimation(true);
    health = 100;

#ifdef PLATFORMER_SERVER
    client = NULL;
    server = NULL;
#endif
}

pf::Character::~Character()  {
}

void pf::Character::Tick(float frametime) {
    // Set horizontal speed
    //speed = inLiquid ? SWIM_SPEED : WALK_SPEED; // Why doesn't this work?
    if (inLiquid)
        speed = SWIM_SPEED;
    else
        speed = WALK_SPEED;

    if (walking) image->Play();
    if (walking) image->SetFramerate(speed / skin->GetFramerate());
    pf::PhysicsEntity::Tick(frametime);

    // If stopped moving sideways (hit wall), stop walking
    if (GetVelocityX() == 0.f && !isolateAnimation)
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
    if (showHealth) {
        healthBackground->SetPosition(nameBackground->GetPosition());
        healthBackground->Move(0, -8);
        healthBackground->SetScale(nameBackground->GetScale().x, 6);
        target.Draw(*healthBackground);

        healthBarRed->SetPosition(healthBackground->GetPosition());
        healthBarRed->Move(1, 1);
        healthBarRed->SetScale(healthBackground->GetScale().x - 2, healthBackground->GetScale().y - 2);
        target.Draw(*healthBarRed);

        healthBarGreen->SetPosition(healthBarRed->GetPosition());\
        healthBarGreen->SetScale(healthBarRed->GetScale().x * health / 100, healthBarRed->GetScale().y);
        target.Draw(*healthBarGreen);
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
    if (!isolateAnimation) SetVelocityX(0.f);
    image->Pause();
    image->SetCurrentFrame(0);
}

void pf::Character::StartWalking() {
    walking = true;
    if (!isolateAnimation) SetVelocityX(direction * speed);
    image->Play();
}

void pf::Character::FaceRight() {
    direction = RIGHT;
    if (walking)
        SetVelocityX(speed && !isolateAnimation);
    image->FlipX(false);
}

void pf::Character::FaceLeft() {
    direction = LEFT;
    if (walking && !isolateAnimation)
        SetVelocityX(-speed);
    image->FlipX(true);
}

pf::CharacterSkin *pf::Character::GetSkin() {
    return skin;
}

pf::Animation *pf::Character::GetImage() {
    return image;
}

bool pf::Character::CanCollideWith(pf::Entity *entity) {
    pf::Character *character = dynamic_cast<pf::Character*>(entity);

    if (character)
        return false;

    return true;
}

void pf::Character::SetIsolateAnimation(bool isolateAnimation) {
    this->isolateAnimation = isolateAnimation;
}

float pf::Character::GetHealth() {
    return health;
}

void pf::Character::SetHealth(float health) {
    this->health = health;
#ifdef PLATFORMER_SERVER
    server->SendToAll(new pf::Packet::Health(this));
    pf::Logger::LogInfo("Setting health for \"%s\" to %f", GetName(), health);
#endif
}

void pf::Character::ShowHealth() {
    showHealth = true;
}

void pf::Character::HideHealth() {
    showHealth = false;
}

bool pf::Character::IsShowingHealth() {
    return showHealth;
}

#ifdef PLATFORMER_SERVER
void pf::Character::SetClient(pf::ClientInstance *client) {
    this->client = client;
}

pf::ClientInstance *pf::Character::GetClient() {
    return client;
}

void pf::Character::SetServer(pf::Server *server) {
    this->server = server;
}

pf::Server *pf::Character::GetServer() {
    return server;
}
#endif
