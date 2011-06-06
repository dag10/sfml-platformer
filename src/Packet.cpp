/*
 * Packet.cpp
 * Creates, parses, and represents packets for the protocol
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

#include "Packet.h"
#include "Resource.h"
#include "CharacterSkin.h"
#include "Character.h"
#include <SFML/Network.hpp>

pf::Packet::PacketString::PacketString(sf::SocketTCP *socket) {
    std::size_t read;
    socket->Receive((char *)&length, sizeof(length), read);
    string = new char[length + 1];
    string[length] = 0;
    socket->Receive(string, length, read);
}

void pf::Packet::PacketString::Send(sf::SocketTCP *socket) {
    socket->Send((const char *)&length, sizeof(length));
    socket->Send(string, length);
}

pf::Packet::LoginRequest::LoginRequest(sf::SocketTCP *socket) {
    std::size_t read;
    socket->Receive((char *)&clientProtocolVersion, sizeof(clientProtocolVersion), read);
    username = new PacketString(socket);
}

void pf::Packet::LoginRequest::Send(sf::SocketTCP *socket) {
    char type = packetType;
    socket->Send((const char *)&type, sizeof(type));
    socket->Send((const char *)&clientProtocolVersion, sizeof(clientProtocolVersion));
    username->Send(socket);
}

pf::Packet::Kick::Kick(sf::SocketTCP *socket) {
    reason = new PacketString(socket);
}

void pf::Packet::Kick::Send(sf::SocketTCP *socket) {
    char type = packetType;
    socket->Send((const char *)&type, sizeof(type));
    reason->Send(socket);
}

pf::Packet::BeginLoad::BeginLoad(sf::SocketTCP *socket) {
    std::size_t read;
    socket->Receive((char *)&numResources, sizeof(numResources), read);
}

void pf::Packet::BeginLoad::Send(sf::SocketTCP *socket) {
    char type = packetType;
    socket->Send((const char *)&type, sizeof(type));
    socket->Send((const char *)&numResources, sizeof(numResources));
}

pf::Packet::EndLoad::EndLoad(sf::SocketTCP *socket) {}

void pf::Packet::EndLoad::Send(sf::SocketTCP *socket) {
    char type = packetType;
    socket->Send((const char *)&type, sizeof(type));
}

pf::Packet::Resource::Resource(sf::SocketTCP *socket) {
    filename = new PacketString(socket);
    std::size_t read;
    socket->Receive((char *)&length, sizeof(length), read);
    data = new char[length];
    socket->Receive(data, length, read);
}

pf::Packet::Resource::Resource(pf::Resource *resource) {
    this->filename = new PacketString(resource->GetFilename());
    this->length = resource->GetLength();
    this->data = resource->GetData();
}

void pf::Packet::Resource::Send(sf::SocketTCP *socket) {
    char type = packetType;
    socket->Send((const char *)&type, sizeof(type));
    filename->Send(socket);
    socket->Send((const char *)&length, sizeof(length));
    socket->Send(data, length);
}

pf::Resource *pf::Packet::Resource::GetResource() {
    char *newFilename = new char[filename->length + 1];
    strcpy(newFilename, filename->string);
    char *newData = new char[length];
    memcpy(newData, data, length);
    return new pf::Resource(newFilename, newData, length);
}

pf::Packet::Property::Property(sf::SocketTCP *socket) {
    name = new PacketString(socket);
    value = new PacketString(socket);
}

void pf::Packet::Property::Send(sf::SocketTCP *socket) {
    char type = packetType;
    socket->Send((const char *)&type, sizeof(type));
    name->Send(socket);
    value->Send(socket);
}

pf::Packet::SpawnCharacter::SpawnCharacter(sf::SocketTCP *socket) {
    std::size_t read;
    socket->Receive((char *)&entityID, sizeof(entityID), read);
    username = new PacketString(socket);
    skin = new PacketString(socket);
    socket->Receive((char *)&x, sizeof(x), read);
    socket->Receive((char *)&y, sizeof(y), read);
}

pf::Packet::SpawnCharacter::SpawnCharacter(pf::Character *character) {
    entityID = character->GetID();
    username = new PacketString(character->GetName());
    skin = new PacketString(character->GetSkin()->GetName());
    x = character->GetX();
    y = character->GetY();
}

void pf::Packet::SpawnCharacter::Send(sf::SocketTCP *socket) {
    char type = packetType;
    socket->Send((const char *)&type, sizeof(type));
    socket->Send((const char *)&entityID, sizeof(entityID));
    username->Send(socket);
    skin->Send(socket);
    socket->Send((const char *)&x, sizeof(x));
    socket->Send((const char *)&y, sizeof(y));
    pf::Logger::LogInfo("SENDING CHARACTER WITH ID: %d", entityID);
}

pf::Packet::CharacterSkin::CharacterSkin(sf::SocketTCP *socket) {
    std::size_t read;
    name = new PacketString(socket);
    resource = new PacketString(socket);
    socket->Receive((char *)&width, sizeof(width), read);
    socket->Receive((char *)&height, sizeof(height), read);
    socket->Receive((char *)&framerate, sizeof(framerate), read);
    socket->Receive((char *)&frames, sizeof(frames), read);
}

pf::Packet::CharacterSkin::CharacterSkin(pf::CharacterSkin *skin) {
    name = new PacketString(skin->GetName());
    resource = new PacketString(skin->GetResource()->GetFilename());
    width = skin->GetWidth();
    height = skin->GetHeight();
    framerate = skin->GetFramerate();
    frames = skin->GetFrames();
}

pf::CharacterSkin *pf::Packet::CharacterSkin::GetCharacterSkin() {
    return new pf::CharacterSkin(name->string, pf::Resource::GetOrLoadResource(resource->string), framerate, frames);
}

void pf::Packet::CharacterSkin::Send(sf::SocketTCP *socket) {
    char type = packetType;
    socket->Send((const char *)&type, sizeof(type));
    name->Send(socket);
    resource->Send(socket);
    socket->Send((const char *)&width, sizeof(width));
    socket->Send((const char *)&height, sizeof(height));
    socket->Send((const char *)&framerate, sizeof(framerate));
    socket->Send((const char *)&frames, sizeof(frames));
}

pf::Packet::StartWorld::StartWorld(sf::SocketTCP *socket) {}

void pf::Packet::StartWorld::Send(sf::SocketTCP *socket) {
    char type = packetType;
    socket->Send((const char *)&type, sizeof(type));
}

pf::Packet::SetCharacter::SetCharacter(sf::SocketTCP *socket) {
    std::size_t read;
    socket->Receive((char *)&entityID, sizeof(entityID), read);
}

pf::Packet::SetCharacter::SetCharacter(pf::Character *character) {
    entityID = character->GetID();
}

void pf::Packet::SetCharacter::Send(sf::SocketTCP *socket) {
    char type = packetType;
    socket->Send((const char *)&type, sizeof(type));
    socket->Send((const char *)&entityID, sizeof(entityID));
}