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
#include "Logger.h"
#include "Packet.h"
#include "CharacterSkin.h"
#include <sstream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

sf::Font *pf::Game::labelFont = NULL;

pf::Game::Game(sf::RenderWindow& renderWindow) {
    localCharacter = NULL;
    world = NULL;
    socket = NULL;

    // Initial game state
    screen = Screen_Main;

    // Initialize GUI
    InitGUI(renderWindow);

    // Initialize World and view
    view = new sf::View(sf::FloatRect(0, 0, 0, 0));

    // Set view variables
    viewSpeed = 11.f;
    zoomSpeed = 11.f;
    viewX = viewY = 0;
    targetZoomFactor = zoomFactor = DEFAULT_ZOOM;
    followCharacter = true;

    shouldQuit = false;
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

    portBox = new cp::cpTextInputBox(&renderWindow, menuContainer, "32123", 0, 0, 100, 16);
    portLabel = new sf::String("Port");
    portLabel->SetColor(sf::Color::White);
    portLabel->SetSize(16);
    portLabel->SetCenter(portLabel->GetRect().GetWidth(), 0);

    joinButton = new cp::cpButton(&renderWindow, menuContainer, "Join Game", 0, 0, 50, 16);

    // Chat

    chatContainer = new cp::cpGuiContainer();

    chatBox = new cp::cpTextInputBox(&renderWindow, chatContainer, "", 0, 0, renderWindow.GetWidth() - (CHAT_UI_SPACING * 2), 16);

    // Joining screen

    joiningContainer = new cp::cpGuiContainer();

    joiningLabel1 = new sf::String();
    joiningLabel1->SetColor(sf::Color::White);
    joiningLabel1->SetSize(24);
    joiningLabel2 = new sf::String();
    joiningLabel2->SetColor(sf::Color::White);
    joiningLabel2->SetSize(18);
    SetJoiningLabelText("Connecting...", NULL);

    joiningReturnButton = new cp::cpButton(&renderWindow, joiningContainer, "Return to Main Screen");

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
        Resource *boxImageResource = Resource::GetOrLoadResource("resources/step.bmp");
        boxImage->LoadFromMemory(boxImageResource->GetData(), boxImageResource->GetLength());
        boxImage->SetSmooth(false);
        //boxImage->CreateMaskFromColor(sf::Color(255, 0, 255));
        boxAnimation = new pf::Animation(*boxImage, 1, 10);
    }

    pf::PhysicsEntity *box = new pf::PhysicsEntity(world, boxAnimation, x, y);
    box->SetGravityEnabled(true);
    box->SetSolid(true);
    box->SetPushable(true);
    world->AddEntity(box);
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

void pf::Game::SetJoiningLabelText(char *line1, char *line2) {
    sf::FloatRect rect;

    if (line1) {
        joiningLabel1->SetText(line1);
        rect = joiningLabel1->GetRect();
        joiningLabel1->SetCenter(rect.GetWidth() / 2, rect.GetHeight() / 2);
    }

    if (line2) {
        joiningLabel2->SetText(line2);
        rect = joiningLabel2->GetRect();
        joiningLabel2->SetCenter(rect.GetWidth() / 2, rect.GetHeight() / 2);
    }
}

void pf::Game::Render(sf::RenderTarget& target, int renderWidth, int renderHeight) {
    switch (screen) {
        case Screen_Game: {
        case Screen_Chat:
            // Set the view
            view->SetFromRect(sf::FloatRect(0, 0, (int)((float)renderWidth / zoomFactor), (int)((float)renderHeight / zoomFactor)));
            view->SetCenter(viewX, viewY);
            target.SetView(*view);

            // Clear rendertarget and render world
            target.Clear(sf::Color(100, 149, 237));
            world->Render(target);
            world->RenderOverlays(target);

            // Reset view for UI rendering
            view->SetFromRect(sf::FloatRect(0, 0, renderWidth, renderHeight));

            // Render chat input box
            chatBox->SetPosition(CHAT_UI_SPACING, renderHeight - chatBox->GetSize().y - CHAT_UI_SPACING);
            chatBox->SetSize(renderWidth - (CHAT_UI_SPACING * 2), chatBox->GetSize().y);
            if (screen == Screen_Chat) chatBox->Draw();

            // Render chat messages
            sf::String *last = NULL;
            for (ChatMessageQueue::iterator it = chatMessages.begin(); it != chatMessages.end(); it++) {
                ChatMessage *message = *it;
                if (message->countdown > 0.f || screen == Screen_Chat) {
                    if (last)
                        message->string->SetPosition(chatBox->GetPosition().x, last->GetPosition().y - message->string->GetRect().GetHeight() - CHAT_MESSAGE_SPACING);
                    else
                        message->string->SetPosition(chatBox->GetPosition().x, chatBox->GetPosition().y - message->string->GetRect().GetHeight() - CHAT_UI_SPACING);

                    message->string->SetColor(sf::Color(50, 50, 50,
                                                        (int)((float)255 * ((message->countdown >= 1.f  || screen == Screen_Chat) ?
                                                                            1.f : message->countdown))));
                    target.Draw(*message->string);

                    message->string->SetColor(sf::Color(255, 255, 255,
                                                        (int)((float)255 * ((message->countdown >= 1.f  || screen == Screen_Chat) ?
                                                                            1.f : message->countdown))));
                    message->string->Move(1.f, 1.f);
                    target.Draw(*message->string);
                    last = message->string;
                }
            }

            // Adjust the view to follow something or zoom
            if (!(localCharacter && followCharacter)) {
                float widthScale = (float)renderWidth / (float)world->GetPixelWidth();
                float heightScale = (float)renderHeight / (float)world->GetPixelHeight();
                targetZoomFactor = ( widthScale < heightScale ? widthScale : heightScale );
            } else {
                targetZoomFactor = DEFAULT_ZOOM;
            }

            break;
        }

        case Screen_Main: {
            view->SetFromRect(sf::FloatRect(0, 0, renderWidth, renderHeight));
            target.SetView(*view);
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

            // Render port label
            portLabel->SetPosition(ipLabel->GetPosition().x, ipLabel->GetPosition().y + ipLabel->GetRect().GetHeight() + UI_SPACING);
            target.Draw(*portLabel);

            // Render port box
            portBox->SetPosition(portLabel->GetPosition().x + UI_SPACING, portLabel->GetPosition().y + 2);
            portBox->Draw();

            // Render join button
            joinButton->SetPosition(portBox->GetPosition().x, portBox->GetPosition().y + portBox->GetSize().y + UI_SPACING);
            joinButton->SetSize(80, 20);
            joinButton->SetFontSize(14);
            joinButton->Draw();

            break;
        }

        case Screen_Disconnect: {
        case Screen_Joining:
            view->SetFromRect(sf::FloatRect(0, 0, renderWidth, renderHeight));
            target.SetView(*view);
            target.Clear(sf::Color::Black);

            screenBackground->SetScale(target.GetWidth(), target.GetHeight());
            target.Draw(*screenBackground);

            // Render joining label2
            joiningLabel1->SetPosition(renderWidth / 2, renderHeight / 2 - joiningLabel1->GetRect().GetHeight() * 2);
            joiningLabel2->SetPosition(renderWidth / 2, renderHeight / 2 + joiningLabel2->GetRect().GetHeight());
            target.Draw(*joiningLabel1);
            target.Draw(*joiningLabel2);

            // Render return button
            joiningReturnButton->SetPosition((renderWidth / 2) - (joiningReturnButton->GetSize().x / 2),
                                             (renderHeight * 2 / 3) - (joiningReturnButton->GetSize().y / 2));
            joiningReturnButton->Draw();

            break;
        }
    }
}

void pf::Game::JoinGame() {
    if (screen != Screen_Main)
        return;

    // Get username
    const char *tempName = nameBox->GetLabelText().c_str();
    playerName = strlen(tempName) ? new char[strlen(tempName) + 1] : NULL;
    if (playerName) strcpy(playerName, tempName);
    delete tempName;

    // Get server IP and port
    serverIP = sf::IPAddress(ipBox->GetLabelText());
    const char *serverPortString = portBox->GetLabelText().c_str();

    // Verify inputs
    if (!playerName || !strlen(playerName)) {
        nameBox->SetFocus(true);
        return;
    } else if (!serverIP.IsValid()) {
        ipBox->SetFocus(true);
        return;
    } else if (!serverPortString || !strlen(serverPortString)) {
        portBox->SetFocus(true);
        return;
    }

    // Set port
    serverPort = atoi(serverPortString);
    delete serverPortString;

    // Set joining label text
    SetJoiningLabelText("", (char *)("Connecting to " + serverIP.ToString() + "...").c_str());

    // Change screen to loading screen
    SetScreen(Screen_Joining);

    // Connect to server
    socket = new sf::SocketTCP();
    if (socket->Connect(serverPort, serverIP) != sf::Socket::Done) {
        std::stringstream portStr;
        portStr << serverPort;
        pf::Logger::LogError("Failed to connect to %s:%d", serverIP.ToString().c_str(), serverPort);
        Disconnect((char *)("Failed to connect to " + serverIP.ToString() + ":" + portStr.str()).c_str());
        return;
    }
    socketSelector = new sf::SelectorTCP();
    socketSelector->Add(*socket);
    pf::Logger::LogInfo("Connected to %s:%d", serverIP.ToString().c_str(), serverPort);

    // Log in
    SetJoiningLabelText(NULL, "Joining game...");
    pf::Logger::LogInfo("Logging in as \"%s\"", playerName);

    // Send login packet
    pf::Packet::LoginRequest(playerName).Send(socket);
}

bool pf::Game::Tick(sf::Input& input, float frametime) {
    if (screen == Screen_Game || screen == Screen_Joining || screen == Screen_Chat) {
        if (socketSelector->Wait(0.01f)) {
            size_t read;
            char packetType;
            int status = socket->Receive(&packetType, sizeof(packetType), read);

            if (status != sf::Socket::Done) {
                Disconnect("Connection broken or terminated.");
                return shouldQuit;
            }

            switch (packetType) {
                case pf::Packet::Kick::packetType: {
                    pf::Packet::Kick packet(socket);
                    Disconnect(packet.reason->string);
                    break;
                }
                case pf::Packet::BeginLoad::packetType: {
                    pf::Packet::BeginLoad packet(socket);
                    resourcesToLoad = packet.numResources;
                    resourcesLoaded = 1;
                    SetJoiningLabelText(screen == Screen_Joining ? NULL : (char *)"Loading...", "Getting server info");
                    pf::Logger::LogInfo("Beginning to load %d resources.", resourcesToLoad);
                    SetScreen(Screen_Joining);
                    break;
                }
                case pf::Packet::EndLoad::packetType: {
                    pf::Packet::EndLoad packet(socket);
                    SetScreen(Screen_Game);
                    break;
                }
                case pf::Packet::Resource::packetType: {
                    pf::Packet::Resource packet(socket);
                    pf::Resource *resource = packet.GetResource();
                    pf::Logger::LogInfo("Received resource \"%s\" ( %d bytes )", packet.filename->string, resource->GetLength());
                    std::stringstream resourceStatus;
                    resourceStatus << "Downloaded resource " << resourcesLoaded << " of " << resourcesToLoad;
                    SetJoiningLabelText(screen == Screen_Joining ? NULL : (char *)"Loading...", (char *)resourceStatus.str().c_str());
                    resourcesLoaded++;
                    break;
                }
                case pf::Packet::Property::packetType: {
                    pf::Packet::Property packet(socket);
                    properties[packet.name->string] = packet.value->string;

                    if (!strcmp("hostname", packet.name->string)) {
                        SetJoiningLabelText(packet.value->string, NULL);
                    }

                    break;
                }
                case pf::Packet::CharacterSkin::packetType: {
                    pf::Packet::CharacterSkin packet(socket);
                    pf::Logger::LogInfo("Received character skin: %s", packet.GetCharacterSkin()->GetName());
                    break;
                }
                case pf::Packet::SpawnCharacter::packetType: {
                    pf::Packet::SpawnCharacter packet(socket);
                    pf::Logger::LogInfo("Spawning character \"%s\" (%d) at (%f, %f)", packet.username->string, packet.entityID, packet.x, packet.y);
                    pf::Character *character = new pf::Character(world, pf::CharacterSkin::GetCharacterSkin(packet.skin->string), packet.username->string);
                    character->SetID(packet.entityID);
                    character->SetPosition(packet.x, packet.y);
                    world->AddEntity(character);
                    break;
                }
                case pf::Packet::StartWorld::packetType: {
                    pf::Packet::StartWorld packet(socket);
                    InitWorld();
                    break;
                }
                case pf::Packet::SetCharacter::packetType: {
                    pf::Packet::SetCharacter packet(socket);

                    if (localCharacter) localCharacter->SetGravityEnabled(false);
                    pf::Character *newLocalCharacter = dynamic_cast<pf::Character*>(world->GetEntity(packet.entityID));
                    if (localCharacter == newLocalCharacter)
                        break;

                    if (localCharacter) localCharacter->SetIsolateAnimation(true);
                    newLocalCharacter->SetIsolateAnimation(false);
                    localCharacter = newLocalCharacter;
                    localCharacter->SetGravityEnabled(true);

                    break;
                }
                case pf::Packet::DespawnEntity::packetType: {
                    pf::Packet::DespawnEntity packet(socket);
                    pf::Entity *entity = world->GetEntity(packet.entityID);
                    if (entity) delete entity;
                    break;
                }
                case pf::Packet::OtherCharacterAnimation::packetType: {
                    pf::Packet::OtherCharacterAnimation packet(socket);
                    pf::Entity *entity = world->GetEntity(packet.entityID);
                    if (!entity) break;
                    pf::Character *character = dynamic_cast<pf::Character*>(entity);
                    if (!character) break;
                    pf::Animation *animation = character->GetImage();
                    if (!animation) break;

                    if (packet.IsFacingRight())
                        character->FaceRight();
                    else
                        character->FaceLeft();

                    if (packet.IsPlaying())
                        character->StartWalking();
                    else
                        character->StopWalking();

                    if (packet.ShouldGotoFrame())
                        animation->SetCurrentFrame(packet.frame);

                    break;
                }
                case pf::Packet::TeleportEntity::packetType: {
                    pf::Packet::TeleportEntity packet(socket);
                    pf::Entity *entity = world->GetEntity(packet.entityID);
                    if (!entity) break;

                    entity->SetPosition(packet.x, packet.y);

                    break;
                }
                case pf::Packet::Health::packetType: {
                    pf::Packet::Health packet(socket);
                    pf::Entity *entity = world->GetEntity(packet.entityID);
                    if (!entity) break;
                    pf::Character *character = dynamic_cast<pf::Character*>(entity);
                    if (!character) break;

                    character->SetHealth(packet.health);

                    break;
                }
                case pf::Packet::Chat::packetType: {
                    pf::Packet::Chat packet(socket);
                    pf::Logger::LogInfo("[CHAT] %s", packet.message->string);

                    chatMessages.push_front(new ChatMessage(packet.message->string));
                    while (chatMessages.size() > MAX_CHAT_MESSAGES)
                        chatMessages.pop_back();

                    break;
                }
            }

        }
    }

    switch (screen) {
        case Screen_Game: {
        case Screen_Chat:
            if (screen == Screen_Chat)
                chatBox->CheckState(&input);

            int oldX, oldY;

            // Character controls
            if (localCharacter && screen == Screen_Game) {
                char oldDirection = localCharacter->GetDirection();
                bool wasWalking = localCharacter->IsWalking();
                oldX = localCharacter->GetX();
                oldY = localCharacter->GetY();

                // Moving left, right, or stopping
                if (input.IsKeyDown(sf::Key::Left))
                    localCharacter->WalkLeft();
                else if (input.IsKeyDown(sf::Key::Right))
                    localCharacter->WalkRight();
                else if (localCharacter->IsWalking())
                    localCharacter->StopWalking();

                // Send animation packet
                if (localCharacter->GetDirection() != oldDirection ||
                    localCharacter->IsWalking() != wasWalking) {
                    pf::Packet::CharacterAnimation(localCharacter).Send(socket);
                }

                // Jumping or swimming upwards
                if (input.IsKeyDown(sf::Key::Up)
                    && localCharacter->IsOnGround())
                    localCharacter->SetVelocityY(localCharacter->IsInLiquid() ? -30 : -100);
            }

            // Fade chat messages
            for (ChatMessageQueue::iterator it = chatMessages.begin(); it != chatMessages.end(); it++) {
                ChatMessage *message = *it;
                if (message->countdown > 0.f) message->countdown -= frametime;
                if (message->countdown < 0.f) message->countdown = 0.f;
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

            if (localCharacter) {
                // Send movement packet
                if ((int)localCharacter->GetX() != oldX ||
                    (int)localCharacter->GetY() != oldY) {
                    pf::Packet::AbsoluteMove(localCharacter).Send(socket);
                }
            }

            break;

        }
        case Screen_Main: {
            // Get GUI widget states
            nameBox->CheckState(&input);
            ipBox->CheckState(&input);
            portBox->CheckState(&input);
            int joinButtonState = joinButton->CheckState(&input);

            // Join button clicked
            if (joinButtonState == cp::CP_ST_MOUSE_LBUTTON_RELEASED)
                JoinGame();

            break;
        }
        case Screen_Joining: {
        case Screen_Disconnect:
            // Get GUI widget states
            int joiningReturnButtonState = joiningReturnButton->CheckState(&input);

            // Return button clicked
            if (joiningReturnButtonState == cp::CP_ST_MOUSE_LBUTTON_RELEASED)
                SetScreen(Screen_Main);

            break;
        }
    }

    return shouldQuit;
}

void pf::Game::HandleClick(sf::Input& input) {
    switch (screen) {
        case Screen_Game: {
            /*if (input.IsMouseButtonDown(sf::Mouse::Right)) {
                std::vector<pf::Entity*> ents = world->HitsPlatform((float)cursorPosition.x, (float)cursorPosition.y);
                for (int i = 0; i < ents.size(); i++) {
                    pf::Entity *ent = ents.at(i);
                    if (((pf::Platform*)ent)->HitTest((float)cursorPosition.x, (float)cursorPosition.y, 0.f, 0.f)) {
                        world->RemoveEntity(*ent);
                        world->RemovePlatform((pf::Platform&)*ent);
                        delete ent;
                    }
                }
            } else if (input.IsMouseButtonDown(sf::Mouse::Left)) {
                addBox(cursorPosition.x, cursorPosition.y);
            } else if (input.IsMouseButtonDown(sf::Mouse::Middle)) {
                localCharacter->SetPosition(cursorPosition.x, cursorPosition.y);
            }*/

            break;
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
            portBox->ProcessTextInput(event);
            menuContainer->ProcessKeys(event);
            break;
        case Screen_Joining:
        case Screen_Disconnect:
            break;
        case Screen_Chat:
            if (switchingToChat)
                switchingToChat = false;
            else
                chatBox->ProcessTextInput(event);
            break;
    }

    switch (event->Type) {
        case sf::Event::KeyPressed:
            switch (event->Key.Code) {
                case sf::Key::Escape:
                    switch (screen) {
                        case Screen_Game:
                        case Screen_Joining:
                            Disconnect("You left the game.");
                            SetScreen(Screen_Main);
                            break;
                        case Screen_Main:
                            shouldQuit = true;
                            break;
                        case Screen_Disconnect:
                            SetScreen(Screen_Main);
                            break;
                        case Screen_Chat:
                            SetScreen(Screen_Game);
                            break;
                    }
                    break;
                case sf::Key::Y:
                    if (screen == Screen_Game)
                        SetScreen(Screen_Chat);
                    break;
                case sf::Key::Return:
                    if (screen == Screen_Chat) {
                        SendChat(chatBox->GetLabelText().c_str());
                        SetScreen(Screen_Game);
                    }
                    break;
            }
            break;
        case sf::Event::Closed:
            shouldQuit = true;
            break;
    }
}

void pf::Game::StopGame() {
    if (socket && socket->IsValid()) {
        socket->Close();
        delete socket;
        socket = NULL;
    }
    if (world) {
        delete world;
        world = NULL;
    }
}

void pf::Game::Disconnect(char *message) {
    SetJoiningLabelText("Disconnected from Server", message);
    SetScreen(Screen_Disconnect);
}

void pf::Game::SetScreen(pf::Screen screen) {
    if (screen == this->screen) return;

    // Switching FROM screen
    switch (this->screen) {
        case Screen_Game:
            break;
        case Screen_Main:
            break;
        case Screen_Joining:
            break;
        case Screen_Disconnect:
            break;
        case Screen_Chat:
            break;
    }

    // Switching TO screen
    switch (screen) {
        case Screen_Game:
            break;
        case Screen_Main:
            StopGame();
            break;
        case Screen_Joining:
            joiningReturnButton->SetFocus(false);
            joiningReturnButton->Show(false);
            break;
        case Screen_Disconnect:
            joiningReturnButton->SetFocus(true);
            joiningReturnButton->Show(true);
            break;
        case Screen_Chat:
            chatBox->SetFocus(true);
            chatBox->SetLabelText("");
            switchingToChat = true;
            break;
    }

    this->screen = screen;
}

pf::Screen pf::Game::GetScreen() {
    return screen;
}

void pf::Game::InitWorld() {
    pf::Logger::LogInfo("Starting World!");
    pf::Logger::LogInfo("LEVEL: %s", (char *)properties["level"].c_str());
    pf::Logger::LogInfo("TILESET: %s", (char *)properties["tileset"].c_str());
    if (world) delete world;
    world = new pf::World(pf::Resource::GetResource((char *)properties["level"].c_str()),
                          pf::Resource::GetResource((char *)properties["tileset"].c_str()));
}

void pf::Game::SendChat(const char *message) {
    if (strlen(message))
        pf::Packet::Chat(message).Send(socket);
}
