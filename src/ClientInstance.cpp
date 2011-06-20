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
#include "Logger.h"
#include "Server.h"
#include "Resource.h"
#include "Character.h"

pf::ClientInstance::ClientInstance(pf::Server *server, sf::SocketTCP *socket, sf::IPAddress *clientIP) {
    this->server = server;
    this->socket = socket;
    this->clientIP = *clientIP;
    this->username = NULL;

    loading = true;
    wasKicked = false;
    character = NULL;
    resourceCount = 0;
}

pf::ClientInstance::~ClientInstance() {
    if (socket) {
        socket->Close();
        delete socket;
    }
    delete [] username;
    delete character;
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

void pf::ClientInstance::EnqueuePacket(pf::Packet::BasePacket *packet) {
    if (!dynamic_cast<pf::Packet::BasePacket*>(packet))
        pf::Logger::LogWarning("ERROR ENQUEUEING PACKET!");
    packetQueue.push(packet);
}

void pf::ClientInstance::EnqueueResource(pf::Resource *resource) {
    EnqueuePacket(new pf::Packet::Resource(resource));
    pf::Logger::LogInfo("Enqueueing resource \"%s\" for client \"%s\"", resource->GetFilename(), username);
    resourceCount++;
}

pf::Packet::BasePacket *pf::ClientInstance::DequeuePacket() {
    if (packetQueue.empty())
        return NULL;

    pf::Packet::BasePacket *packet = packetQueue.front();
    packetQueue.pop();

    if(dynamic_cast<pf::Packet::Resource*>(packet))
        resourceCount--;

    pf::Packet::Kick *kick = dynamic_cast<pf::Packet::Kick*>(packet);
    if (kick)
        pf::Logger::LogInfo("[%s] KICK: \"%s\"", GetUsername(), kick->reason->string);

    return packet;
}

int pf::ClientInstance::QueuedPackets() {
    return packetQueue.size();
}

int pf::ClientInstance::QueuedResources() {
    return resourceCount;
}

void pf::ClientInstance::Kick(char *message) {
    pf::Logger::LogInfo("Player \"%s\" [%s] kicked: %s", GetUsername(), clientIP.ToString().c_str(), message);
    pf::Packet::Kick(message).Send(socket);
    wasKicked = true;
}

bool pf::ClientInstance::WasKicked() {
    return wasKicked;
}

void pf::ClientInstance::SetCharacter(pf::Character *character) {
    if (!this->character)
        this->character = character;
}

pf::Character *pf::ClientInstance::GetCharacter() {
    return character;
}

bool pf::ClientInstance::IsLoading() {
    return loading;
}

void pf::ClientInstance::BeginLoading() {
    pf::Packet::BeginLoad(resourceCount).Send(socket);
    loading = true;
}

void pf::ClientInstance::EndLoading() {
    pf::Packet::EndLoad().Send(socket);
    loading = false;
    resourceCount = 0;
}
