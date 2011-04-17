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

#include <Game.h>
#include <PhysicsEntity.h>
#include <Character.h>
#include <World.h>

#include <iostream>
using namespace std;

pf::Game::Game() {
    // Initialize World and view
    world = new pf::World("resources/level_01.bmp");
    view = new sf::View(sf::FloatRect(0, 0, 0, 0));

    // Initialize view variables
    viewSpeed = 11.f;
    viewX = viewY = 0;

    // Initialize main character
    mainCharacter = new pf::Character(world);
    mainCharacter->SetPosition(30, 30);
    mainCharacter->SetGravityEnabled(true);
    mainCharacter->SetSolid(true);
    mainCharacter->SetCanUseStairs(true);
    world->AddEntity(*mainCharacter);

    // Initialize box image
    sf::Image *boxImage = new sf::Image();
    boxImage->LoadFromFile("resources/box.bmp");
    boxImage->SetSmooth(false);

    // Initialize box
    box = new pf::PhysicsEntity(world, boxImage, 90, 30);
    box->SetGravityEnabled(true);
    box->SetSolid(true);
    box->SetPushable(true);
    world->AddEntity(*box);

    // Initialize another block
    pf::PhysicsEntity *c = new pf::PhysicsEntity(world, boxImage, 100, 30);
    c->SetGravityEnabled(true);
    c->SetSolid(true);
    c->SetPushable(true);
    world->AddEntity(*c);

    // Initialize step image
    sf::Image *stepImage = new sf::Image();
    stepImage->LoadFromFile("resources/step.bmp");
    stepImage->SetSmooth(false);

    // Initialize three steps
    pf::PhysicsEntity *step1 = new pf::PhysicsEntity(world, stepImage, 50, 30);
    step1->SetGravityEnabled(true);
    step1->SetSolid(true);
    step1->SetPushable(false);
    world->AddEntity(*step1);
    pf::PhysicsEntity *step2 = new pf::PhysicsEntity(world, stepImage, 60, 50);
    step2->SetGravityEnabled(true);
    step2->SetSolid(true);
    step2->SetPushable(false);
    world->AddEntity(*step2);
    pf::PhysicsEntity *step3 = new pf::PhysicsEntity(world, stepImage, 60, 30);
    step3->SetGravityEnabled(true);
    step3->SetSolid(true);
    step3->SetPushable(false);
    world->AddEntity(*step3);

    // Initialize another step
    pf::PhysicsEntity *step4 = new pf::PhysicsEntity(world, stepImage, 130, 50);
    step4->SetGravityEnabled(true);
    step4->SetSolid(true);
    step4->SetPushable(true);
    world->AddEntity(*step4);
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
    if (mainCharacter) {
        delete mainCharacter;
        mainCharacter = NULL;
    }
    if (box) {
        delete box;
        box = NULL;
    }
}

void pf::Game::Render(sf::RenderTarget& target, int renderWidth, int renderHeight) {
    if (lastTarget != &target) {
        view->SetFromRect(sf::FloatRect(0, 0, renderWidth, renderHeight));
        lastTarget = &target;
    }

    //view->SetCenter((int)viewX, (int)viewY);
    view->SetCenter(viewX, viewY);
    target.SetView(*view);

    target.Clear(sf::Color(100, 149, 237));
    world->Render(target);
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
        mainCharacter->SetVelocityY(-100);

    // Update view position
    float targetViewX = mainCharacter->GetX() + (mainCharacter->GetWidth() / 2);
    float targetViewY = mainCharacter->GetY() + (mainCharacter->GetHeight() / 2);
    viewX += (targetViewX - viewX) / viewSpeed;
    viewY += (targetViewY - viewY) / viewSpeed;

    // Tick world
    world->Tick(frametime);
}
