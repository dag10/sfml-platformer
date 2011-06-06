/*
 * Server.cpp
 * Manages a server and networking
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

#include "Server.h"
#include "Logger.h"
#include "World.h"
#include "Resource.h"
#include "ClientInstance.h"
#include "CharacterSkin.h"
#include "Character.h"
#include "Packet.h"

pf::Server::Server(unsigned short port) {
    shouldQuit = false;
    this->serverPort = port;
    
    // Initialize properties
    
    pf::Logger::LogInfo("Loading default settings");
    properties["level"] = "resources/level_01.bmp";
    properties["tileset"] = "resources/tileset.bmp";
    properties["hostname"] = "Drew's Test Server";
    
    // Initialize resources
    
    pf::Logger::LogInfo("Loading resources");
    pf::Resource::SetServer(this);
    pf::Resource *levelResource = pf::Resource::GetOrLoadResource((char *)properties["level"].c_str());
    pf::Resource *tilesetResource = pf::Resource::GetOrLoadResource((char *)properties["tileset"].c_str());
    
    // Initialize character skins
    
    new pf::CharacterSkin("character_01", pf::Resource::GetOrLoadResource("resources/character_01.bmp"), 15, 6);
    new pf::CharacterSkin("character_02", pf::Resource::GetOrLoadResource("resources/character_02.bmp"), 15, 6);
    
    // Initialize world
    
    pf::Logger::LogInfo("Initializing world");
    world = new pf::World(levelResource, tilesetResource);
    
    // Initialize network
    
    pf::Logger::LogInfo("Initializing network");
    listenSocket = new sf::SocketTCP();
    if (!listenSocket->Listen(port)) {
        pf::Logger::LogFatal("Failed to listen on port %d", port);
        return;
    }
    socketSelector.Add(*listenSocket);
    
    // Main loop
    
    pf::Logger::LogInfo("Listening on port %d", port);
    while (!shouldQuit) {
        int readySockets = socketSelector.Wait(0.01f);
        
        // Handle incoming connections/data
        for (int i = 0; i < readySockets; i++) {
            sf::SocketTCP socket = socketSelector.GetSocketReady(i);
            
            if (socket == *listenSocket) {
                sf::IPAddress clientAddress;
                sf::SocketTCP *clientSocket = new sf::SocketTCP();
                
                if (listenSocket->Accept(*clientSocket, &clientAddress) != sf::Socket::Done) {
                    pf::Logger::LogError("Failed to accept connection [%s]", clientAddress.ToString().c_str());
                }
        
                pf::ClientInstance *client = new pf::ClientInstance(this, clientSocket, &clientAddress);
                socketSelector.Add(*clientSocket);
                clientMap[*clientSocket] = client;
                
            } else {
                pf::ClientInstance *client = clientMap[socket];
                size_t read;
                char packetType;
                int status = socket.Receive(&packetType, sizeof(packetType), read);
                
                if (status != sf::Socket::Done) {
                    if (status == sf::Socket::Disconnected) {
                        if (!client->WasKicked())
                            pf::Logger::LogInfo("A client disconnected: \"%s\" [%s]",
                                                (client->GetUsername() ? client->GetUsername() : ""),
                                                client->GetAddress()->ToString().c_str());
                    } else {
                        pf::Logger::LogError("Error while receiving from socket. Disconnecting client: \"%s\" [%s]",
                                            (client->GetUsername() ? client->GetUsername() : ""),
                                            client->GetAddress()->ToString().c_str());
                    }
                    
                    clientMap.erase(socket);
                    socketSelector.Remove(socket);
                    socket.Close();
                    delete client;
                    continue;
                }
                
                switch (packetType) {
                    case pf::Packet::LoginRequest::packetType:
                        // Break if already logged in
                        if (client->GetUsername()) break;
                        
                        // Read and parse login packet
                        pf::Packet::LoginRequest packet(&socket);
                        client->SetUsername(packet.username->string);
                        pf::Logger::LogInfo("Player \"%s\" connected from [%s]", client->GetUsername(), client->GetAddress()->ToString().c_str());
                        
                        // Kick client if using outdated protocol version
                        if (packet.clientProtocolVersion < pf::Packet::PROTOCOL_VERSION) {
                            Kick(client, "You are using an incompatible (outdated) client!");
                            break;
                        }
                        
                        // Create character
                        static bool alternateSkin = false;
                        alternateSkin = !alternateSkin;
                        pf::CharacterSkin *skin;
                        if (alternateSkin)
                            skin = pf::CharacterSkin::GetCharacterSkin("character_01");
                        else
                            skin = pf::CharacterSkin::GetCharacterSkin("character_02");
                        
                        pf::Character *character = new pf::Character(world, skin, client->GetUsername());
                        client->SetCharacter(character);
                        world->SpawnCharacter(client->GetCharacter());
                        
                        // Send properties
                        for (PropertyMap::iterator it = properties.begin(); it != properties.end(); it++)
                            client->EnqueuePacket(new pf::Packet::Property((char *)it->first.c_str(), (char *)it->second.c_str()));
                        
                        // Send resources
                        for (std::vector<pf::Resource*>::iterator it = requiredResources.begin(); it != requiredResources.end(); it++) {
                            client->EnqueueResource(*it);
                        }
                        
                        // Send character skins
                        for (CharacterSkinMap::iterator it = pf::CharacterSkin::GetCharacterSkinMap()->begin();
                             it != pf::CharacterSkin::GetCharacterSkinMap()->end();
                             it++)
                            client->EnqueuePacket(new pf::Packet::CharacterSkin(it->second));
                        
                        // Send indicator to finalize world
                        client->EnqueuePacket(new pf::Packet::StartWorld());
                        
                        // Send character to others
                        pf::Packet::SpawnCharacter *spawnPacket = new pf::Packet::SpawnCharacter(client->GetCharacter());
                        SendToAll(spawnPacket, client);
                        
                        // Spawn other characters
                        for (ClientMap::iterator it = clientMap.begin(); it != clientMap.end(); it++)
                            client->EnqueuePacket(new pf::Packet::SpawnCharacter(it->second->GetCharacter()));
                        
                        // Set client's character
                        client->EnqueuePacket(new pf::Packet::SetCharacter(client->GetCharacter()));
                        
                        // Begin loading
                        client->BeginLoading();
                        
                        break;
                }
            }
        }
        
        // Loop through each client
        for (ClientMap::iterator it = clientMap.begin(); it != clientMap.end(); it++) {
            pf::ClientInstance *client = it->second;
            
            // Send resources as needed
            pf::Packet::BasePacket *packet = client->DequeuePacket();
            if (packet) {
                if (dynamic_cast<pf::Packet::Resource*>(packet))
                    pf::Logger::LogInfo("Sending resource \"%s\" to client \"%s\"", dynamic_cast<pf::Packet::Resource*>(packet)->GetResource()->GetFilename(), client->GetUsername());
                else if (dynamic_cast<pf::Packet::SpawnCharacter*>(packet))
                    pf::Logger::LogInfo("Sending character \"%s\" to client \"%s\"", dynamic_cast<pf::Packet::SpawnCharacter*>(packet)->username->string, client->GetUsername());
                else if (dynamic_cast<pf::Packet::CharacterSkin*>(packet))
                    pf::Logger::LogInfo("Sending character skin \"%s\" to client \"%s\"", dynamic_cast<pf::Packet::CharacterSkin*>(packet)->name->string, client->GetUsername());
                packet->Send(client->GetSocket());
                if (!client->QueuedPackets() && client->IsLoading()) {
                    client->EndLoading();
                }
            }
        }
    }
}

pf::Server::~Server() {
    
}

void pf::Server::Kick(pf::ClientInstance *client, char *message) {
    client->Kick(message);
    socketSelector.Remove(*client->GetSocket());
    clientMap.erase(*client->GetSocket());
    client->GetSocket()->Close();
    delete client;
}

void pf::Server::SendToAll(pf::Packet::BasePacket *packet) {
    SendToAll(packet, NULL);
}

void pf::Server::SendToAll(pf::Packet::BasePacket *packet, pf::ClientInstance *exclude) {
    for (ClientMap::iterator it = clientMap.begin(); it != clientMap.end(); it++) {
        pf::ClientInstance *client = it->second;
        if (client && client != exclude && !client->IsLoading())
            client->EnqueuePacket(packet);
    }
}

void pf::Server::RequireResource(pf::Resource *resource) {
    for (int i = 0; i < requiredResources.size(); i++)
        if (requiredResources.at(i) == resource)
            return;
    
    requiredResources.push_back(resource);
    
    //for (ClientMap::iterator it = clientMap.begin(); it != clientMap.end(); it++)
    //    it->second->EnqueueResource(resource);
}