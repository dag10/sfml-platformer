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
#include "Packet.h"

pf::Server::Server(unsigned short port) {
    shouldQuit = false;
    this->serverPort = port;
    
    // Initialize properties
    
    pf::Logger::LogInfo("Loading default settings");
    properties["level"] = "resources/level_01.bmp";
    properties["tileset"] = "resources/tileset.bmp";
    
    // Initialize resources
    
    pf::Logger::LogInfo("Loading resources");
    pf::Resource *levelResource = pf::Resource::GetOrLoadResource((char *)properties["level"].c_str());
    pf::Resource *tilesetResource = pf::Resource::GetOrLoadResource((char *)properties["tileset"].c_str());
    
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
        int readySockets = socketSelector.Wait(1000.f);
        
        for (int i = 0; i < readySockets; i++) {
            sf::SocketTCP socket = socketSelector.GetSocketReady(i);
            
            if (socket == *listenSocket) {
                sf::IPAddress clientAddress;
                sf::SocketTCP clientSocket;
                
                if (listenSocket->Accept(clientSocket, &clientAddress) != sf::Socket::Done) {
                    pf::Logger::LogError("Failed to accept connection [%s]", clientAddress.ToString().c_str());
                }
        
                pf::ClientInstance *client = new pf::ClientInstance(this, &clientSocket, &clientAddress);
                socketSelector.Add(clientSocket);
                clientMap[clientSocket] = client;
                
            } else {
                pf::ClientInstance *client = clientMap[socket];
                size_t read;
                char packetType;
                int status = socket.Receive(&packetType, sizeof(packetType), read);
                
                if (status != sf::Socket::Done) {
                    if (status == sf::Socket::Disconnected)
                        pf::Logger::LogInfo("A client disconnected: \"%s\" [%s]",
                                            (client->GetUsername() ? client->GetUsername() : ""),
                                            client->GetAddress()->ToString().c_str());
                    else
                        pf::Logger::LogError("Error while receiving from socket. Disconnecting client: \"%s\" [%s]",
                                            (client->GetUsername() ? client->GetUsername() : ""),
                                            client->GetAddress()->ToString().c_str());
                    
                    socketSelector.Remove(socket);
                    socket.Close();
                    continue;
                }
                
                switch (packetType) {
                    case pf::Packet::LoginRequest::packetType:
                        pf::Packet::LoginRequest packet(&socket);
                        client->SetUsername(packet.username->string);
                        pf::Logger::LogInfo("Player \"%s\" connected from [%s]", packet.username->string, client->GetAddress()->ToString().c_str());
                        break;
                }
            }
        }
    }
}

pf::Server::~Server() {
    
}