/*
 * Game.cpp
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

#include "Game.h"
#include "PhysicsEntity.h"
#include "Character.h"
#include "World.h"
#include "Elevator.h"
#include "Animation.h"
#include "BouncyParticle.h"
#include "Resource.h"

#include <iostream>
using namespace std;

pf::Game::Game() {
    // Initialize resources
    Resource *stepImageResource = Resource::GetOrLoadResource("resources/step.bmp");
    Resource *levelImageResource = Resource::GetOrLoadResource("resources/level_01.bmp");
    
    // Initialize World and view
    world = new pf::World(levelImageResource);
    view = new sf::View(sf::FloatRect(0, 0, 0, 0));

    // Initialize view variables
    viewSpeed = 11.f;
    viewX = viewY = 0;
    zoomFactor = 1.f;
    zoomFactor = 2.5f;
    //zoomFactor = 0.1f;
    
    // Initialize main character
    mainCharacter = new pf::Character(world, pf::Resource::GetOrLoadResource("resources/character_01.bmp"), "Drew");
    mainCharacter->SetPosition(30, 30);
    mainCharacter->SetGravityEnabled(true);
    mainCharacter->SetSolid(true);
    mainCharacter->SetCanUseStairs(true);
    world->AddEntity(*mainCharacter);
    
    // Initialize secondary character
    pf::Character *secondCharacter = new pf::Character(world, pf::Resource::GetOrLoadResource("resources/character_01.bmp"), "Some Hacker");
    secondCharacter->SetPosition(160, 30);
    secondCharacter->SetGravityEnabled(true);
    secondCharacter->SetSolid(true);
    secondCharacter->SetCanUseStairs(true);
    world->AddEntity(*secondCharacter);

    // Add boxes
    addBox(85, 30);
    //addBox(100, 42);
    //addBox(100, 27);
    //addBox(115, 40);
    addBox(130, 30);

    // Initialize step image
    sf::Image *stepImage = new sf::Image();
    stepImage->LoadFromMemory(stepImageResource->GetData(), stepImageResource->GetLength());
    stepImage->SetSmooth(false);
    pf::Animation *stepAnimation = new pf::Animation(*stepImage, 1, 0);

    // Initialize three steps
    pf::PhysicsEntity *step1 = new pf::PhysicsEntity(world, stepAnimation, 50, 30);
    step1->SetGravityEnabled(true);
    step1->SetSolid(true);
    step1->SetPushable(false);
    world->AddEntity(*step1);
    pf::PhysicsEntity *step2 = new pf::PhysicsEntity(world, stepAnimation, 60, 50);
    step2->SetGravityEnabled(true);
    step2->SetSolid(true);
    step2->SetPushable(false);
    world->AddEntity(*step2);
    pf::PhysicsEntity *step3 = new pf::PhysicsEntity(world, stepAnimation, 60, 30);
    step3->SetGravityEnabled(true);
    step3->SetSolid(true);
    step3->SetPushable(false);
    world->AddEntity(*step3);

    // PhysicsEntity another step
    pf::Elevator *elevator = new pf::Elevator(world, stepAnimation, 130, 55);
    elevator->SetSolid(true);
    //world->AddEntity(*elevator);
    
    // Add particle
    sf::Image *particleImage = new sf::Image();
    particleImage->LoadFromFile("resources/particle_01.bmp");
    particleImage->SetSmooth(false);
    pf::Animation *particleAnimation = new pf::Animation(*particleImage, 1, 0);
    pf::Particle *particle = new pf::BouncyParticle(world, particleAnimation, 80, 40);
    world->AddEntity(*particle);
    particle->SetVelocity(60.f, 40.f);
}

void pf::Game::addBox(int x, int y) {
    static pf::Animation *boxAnimation;
    
    if (!boxAnimation) {
        sf::Image *boxImage = new sf::Image();
        Resource *boxImageResource = Resource::GetOrLoadResource("resources/box.bmp");
        boxImage->LoadFromMemory(boxImageResource->GetData(), boxImageResource->GetLength());
        boxImage->SetSmooth(false);
        //boxImage->CreateMaskFromColor(sf::Color(255, 0, 255));
        boxAnimation = new pf::Animation(*boxImage, 1, 10);
    }
    
    box = new pf::PhysicsEntity(world, boxAnimation, x, y);
    box->SetGravityEnabled(true);
    box->SetSolid(true);
    box->SetPushable(true);
    world->AddEntity(*box);
}

pf::Game::~Game() {
    if (world) {
        delete world;
        world = NULL;
    }
    if (view) {
        delete view;
        view = NULL;
    }
    // TODO: Is this really neccesary?
    //       Wouldn't all entities be deleted when the entities list in the World is emptied?
    if (mainCharacter) {
        delete mainCharacter;
        mainCharacter = NULL;
    }
}

void pf::Game::Render(sf::RenderTarget& target, int renderWidth, int renderHeight) {
    if (lastTarget != &target) {
        view->SetFromRect(sf::FloatRect(0, 0, (int)((float)renderWidth / zoomFactor), (int)((float)renderHeight / zoomFactor)));
        lastTarget = &target;
    }

    //view->SetCenter((int)viewX, (int)viewY);
    view->SetCenter(viewX, viewY);
    target.SetView(*view);

    target.Clear(sf::Color(100, 149, 237));
    world->Render(target);
    world->RenderOverlays(target);
}

void pf::Game::Tick(sf::Input& input, float frametime) {
    if (input.IsKeyDown(sf::Key::Left))
        mainCharacter->WalkLeft();
    else if (input.IsKeyDown(sf::Key::Right))
        mainCharacter->WalkRight();
    else if (mainCharacter->IsWalking())
        mainCharacter->StopWalking();

    if (input.IsKeyDown(sf::Key::Up)
        && mainCharacter->IsOnGround())
        mainCharacter->SetVelocityY(mainCharacter->IsInLiquid() ? -30 : -100);

    // Update view position
    float targetViewX = mainCharacter->GetX() + (mainCharacter->GetWidth() / 2);
    float targetViewY = mainCharacter->GetY() + (mainCharacter->GetHeight() / 2);
    viewX += (targetViewX - viewX) / viewSpeed;
    viewY += (targetViewY - viewY) / viewSpeed;

    // Tick world
    world->Tick(frametime);
}
