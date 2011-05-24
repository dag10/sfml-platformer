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
#include "Platform.h"
#include "Resource.h"
#include "cpGUI.h"
#include <SFML/Window.hpp>

#include <iostream>
using namespace std;

sf::Font *pf::Game::labelFont = 0;

pf::Game::Game(sf::RenderWindow& renderWindow) {
    localCharacter = NULL;
    
    // Initial game state
    screen = Screen_Main;
    
    // Initialize GUI
    InitGUI(renderWindow);
    
    // Initialize World and view
    //world = new pf::World(levelImageResource);
    view = new sf::View(sf::FloatRect(0, 0, 0, 0));

    // Set view variables
    viewSpeed = 11.f;
    zoomSpeed = 11.f;
    viewX = viewY = 0;
    targetZoomFactor = zoomFactor = DEFAULT_ZOOM;
    followCharacter = false;
}

void pf::Game::InitGUI(sf::RenderWindow& renderWindow) {
    pf::Resource *fontResource = pf::Resource::GetOrLoadResource("resources/MIASWFTE.TTF");
    labelFont = new sf::Font();
    labelFont->LoadFromMemory(fontResource->GetData(), fontResource->GetLength(), 30);
    
    // Main Menu
    
    menuContainer = new cp::cpGuiContainer();
    
    nameBox = new cp::cpTextInputBox(&renderWindow, menuContainer, "", 0, 0, 200, 16);
    nameLabel = new sf::String("Username");
    nameLabel->SetColor(sf::Color::White);
    nameLabel->SetSize(16);
    nameLabel->SetCenter(nameLabel->GetRect().GetWidth(), 0);
    
    ipBox = new cp::cpTextInputBox(&renderWindow, menuContainer, "127.0.0.1", 0, 0, 200, 16);
    ipLabel = new sf::String("Server");
    ipLabel->SetColor(sf::Color::White);
    ipLabel->SetSize(16);
    ipLabel->SetCenter(ipLabel->GetRect().GetWidth(), 0);
    
    joinButton = new cp::cpButton(&renderWindow, menuContainer, "Join Game", 0, 0, 50, 16);
    
    // Menu background shape/sprite
    
    static sf::Color screenBackgroundFill = sf::Color(100, 100, 100);
    screenBackground = new sf::Shape();
    screenBackground->AddPoint(0, 0, screenBackgroundFill);
    screenBackground->AddPoint(1, 0, screenBackgroundFill);
    screenBackground->AddPoint(1, 1, screenBackgroundFill);
    screenBackground->AddPoint(0, 1, screenBackgroundFill);
    screenBackground->SetPosition(0, 0);
    screenBackground->EnableFill(true);
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
    
    pf::PhysicsEntity *box = new pf::PhysicsEntity(world, boxAnimation, x, y);
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
}

void pf::Game::Render(sf::RenderTarget& target, int renderWidth, int renderHeight) {
    switch (screen) {
        case Screen_Game:
            view->SetFromRect(sf::FloatRect(0, 0, (int)((float)renderWidth / zoomFactor), (int)((float)renderHeight / zoomFactor)));

            view->SetCenter(viewX, viewY);
            target.SetView(*view);

            target.Clear(sf::Color(100, 149, 237));
            world->Render(target);
            world->RenderOverlays(target);
            
            if (!(localCharacter && followCharacter)) {
                float widthScale = (float)renderWidth / (float)world->GetPixelWidth();
                float heightScale = (float)renderHeight / (float)world->GetPixelHeight();
                targetZoomFactor = ( widthScale < heightScale ? widthScale : heightScale );
            } else {
                targetZoomFactor = DEFAULT_ZOOM;
            }
            
            break;
        case Screen_Main:
            target.SetView(target.GetDefaultView());
            target.Clear(sf::Color::Black);
            
            screenBackground->SetScale(target.GetWidth(), target.GetHeight());
            target.Draw(*screenBackground);
            
            // Render name label
            nameLabel->SetPosition(100, 100);
            target.Draw(*nameLabel);
            
            // Render name box
            nameBox->SetPosition(nameLabel->GetPosition().x + UI_SPACING, nameLabel->GetPosition().y + 2);
            nameBox->Draw();
            
            // Render server label
            ipLabel->SetPosition(nameLabel->GetPosition().x, nameLabel->GetPosition().y + nameLabel->GetRect().GetHeight() + UI_SPACING);
            target.Draw(*ipLabel);
            
            // Render server box
            ipBox->SetPosition(ipLabel->GetPosition().x + UI_SPACING, ipLabel->GetPosition().y + 2);
            ipBox->Draw();
            
            // Render join button
            joinButton->SetPosition(ipBox->GetPosition().x, ipBox->GetPosition().y + ipBox->GetSize().y + UI_SPACING);
            joinButton->SetSize(80, 20);
            joinButton->SetFontSize(14);
            joinButton->Draw();
            
            break;
        case Screen_Joining:
            break;
    }
}

void pf::Game::JoinGame() {
    if (screen != Screen_Main)
        return;
                
    // Set up variables
    playerName = (char *)(nameBox->GetLabelText().c_str());
    
    // Verify inputs
    if (!playerName || !strlen(playerName)) {
        nameBox->SetFocus(true);
        return;
    }
    
    // TEMPORARY: Initialize tileset resource
    pf::Resource *tileset = pf::Resource::GetOrLoadResource("resources/tileset.bmp");
    
    // Initialize world
    world = new World(Resource::GetOrLoadResource("resources/level_01.bmp"), tileset);
    
    // Spawn local character
    localCharacter = new pf::Character(world, Resource::GetOrLoadResource("resources/character_02.bmp"), playerName);
    localCharacter->SetPosition(60, 30);
    world->AddEntity(*localCharacter);
    
    // Set zoom level
    followCharacter = true;
    
    // Change screen to game screenBackground
    SetScreen(Screen_Game);
}

void pf::Game::Tick(sf::Input& input, float frametime) {
    switch (screen) {
        case Screen_Game: {
            // Character controls
            if (localCharacter) {
                // Moving left, right, or stopping
                if (input.IsKeyDown(sf::Key::Left))
                    localCharacter->WalkLeft();
                else if (input.IsKeyDown(sf::Key::Right))
                    localCharacter->WalkRight();
                else if (localCharacter->IsWalking())
                    localCharacter->StopWalking();

                // Jumping or swimming upwards
                if (input.IsKeyDown(sf::Key::Up)
                    && localCharacter->IsOnGround())
                    localCharacter->SetVelocityY(localCharacter->IsInLiquid() ? -30 : -100);
            }
            
            // Update zoom factor
            zoomFactor += (targetZoomFactor - zoomFactor) / zoomSpeed;
            
            // Update cursor position
            sf::Vector2f halfSize = view->GetHalfSize();
            cursorPosition = sf::Vector2f(input.GetMouseX() / zoomFactor + (viewX - halfSize.x), input.GetMouseY() / zoomFactor + (viewY - halfSize.y));

            // Update view position
            float targetViewX, targetViewY;
            if (localCharacter && followCharacter) {
                targetViewX = localCharacter->GetX() + (localCharacter->GetWidth() / 2);
                targetViewY = localCharacter->GetY() + (localCharacter->GetHeight() / 2);
            } else {
                targetViewX = world->GetPixelWidth() / 2;
                targetViewY = world->GetPixelHeight() / 2;
            }
            viewX += (targetViewX - viewX) / viewSpeed;
            viewY += (targetViewY - viewY) / viewSpeed;

            // Tick world
            world->Tick(frametime);
            
            break;
            
        } case Screen_Main: {
            // Get GUI widget states
            nameBox->CheckState(&input);
            ipBox->CheckState(&input);
            int joinButtonState = joinButton->CheckState(&input);
            
            // Join button clicked
            if (joinButtonState == cp::CP_ST_MOUSE_LBUTTON_RELEASED)
                JoinGame();
            
            break;
            
        } case Screen_Joining: {
            break;
        }
    }
}

void pf::Game::HandleClick(sf::Input& input) {
    if (screen == Screen_Game) {
        //mainCharacter->SetPosition(cursorPosition.x, cursorPosition.y);
        std::vector<pf::Entity*> ents = world->HitsPlatform((float)cursorPosition.x, (float)cursorPosition.y);
        for (int i = 0; i < ents.size(); i++) {
            pf::Entity *ent = ents.at(i);
            if (((pf::Platform*)ent)->HitTest((float)cursorPosition.x, (float)cursorPosition.y, 0.f, 0.f)) {
                world->RemoveEntity(*ent);
                world->RemovePlatform((pf::Platform&)*ent);
                delete ent;
            }
        }
    }
}

sf::Vector2f pf::Game::GetCursorPosition() {
    return cursorPosition;
}

void pf::Game::HandleEvent(sf::Event *event, sf::Input *input) {
    switch (screen) {
        case Screen_Game:
            switch (event->Type) {
                case sf::Event::KeyPressed:
                    switch (event->Key.Code) {
                        case sf::Key::Space:
                            followCharacter = !followCharacter;
                            break;
                    }
                    break;
                case sf::Event::MouseButtonPressed:
                    HandleClick(*input);
                    break;
            }
            break;
        case Screen_Main:
            nameBox->ProcessTextInput(event);
            ipBox->ProcessTextInput(event);
            menuContainer->ProcessKeys(event);
            break;
        case Screen_Joining:
            break;
    }
}

void pf::Game::SetScreen(pf::Screen screen) {
    if (screen == this->screen) return;
    
    switch (screen) {
        case Screen_Game:
            break;
        case Screen_Main:
            break;
        case Screen_Joining:
            break;
    }
    
    this->screen = screen;
}

pf::Screen pf::Game::GetScreen() {
    return screen;
}
