/*
 * ClientInstance.cpp
 * Manages a server's client instance and its networking
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

#include "ClientInstance.h"
#include "Packet.h"

pf::ClientInstance::ClientInstance(pf::Server *server, sf::SocketTCP *socket, sf::IPAddress *clientIP) {
    this->server = server;
    this->socket = socket;
    this->clientIP = *clientIP;
    this->username = NULL;
}

pf::ClientInstance::~ClientInstance() {
    
}

sf::SocketTCP *pf::ClientInstance::GetSocket() {
    return socket;
}

sf::IPAddress *pf::ClientInstance::GetAddress() {
    return &clientIP;
}

void pf::ClientInstance::SetUsername(char *username) {
    this->username = new char[strlen(username) + 1];
    strcpy(this->username, username);
}

char *pf::ClientInstance::GetUsername() {
    return username;
}

void pf::ClientInstance::EnqueueResource(pf::Resource *resource) {
    resourceQueue.push_back(resource);
}

pf::Resource *pf::ClientInstance::DequeueResource() {
    if (!resourceQueue.size())
        return NULL;
    
    pf::Resource *resource = resourceQueue.back();
    resourceQueue.pop_back();
    return resource;
}

int pf::ClientInstance::QueuedResources() {
    return resourceQueue.size();
}

void pf::ClientInstance::Kick(char *message) {
    pf::Packet::Kick(message).Send(socket);
}