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
#include "Animation.h"
#include "Character.h"
#include "Logger.h"
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

pf::Packet::DespawnEntity::DespawnEntity(sf::SocketTCP *socket) {
    std::size_t read;
    socket->Receive((char *)&entityID, sizeof(entityID), read);
}

pf::Packet::DespawnEntity::DespawnEntity(pf::Entity *entity) {
    entityID = entity->GetID();
}

void pf::Packet::DespawnEntity::Send(sf::SocketTCP *socket) {
    char type = packetType;
    socket->Send((const char *)&type, sizeof(type));
    socket->Send((const char *)&entityID, sizeof(entityID));
}

pf::Packet::CharacterAnimation::CharacterAnimation(sf::SocketTCP *socket) {
    std::size_t read;
    socket->Receive((char *)&data, sizeof(data), read);
    socket->Receive((char *)&frame, sizeof(frame), read);
}

pf::Packet::CharacterAnimation::CharacterAnimation(pf::Character *character) {
    pf::Animation *animation = character->GetImage();
    data = MakeData(character->GetDirection() == pf::Character::RIGHT, animation->IsPlaying(), !animation->IsPlaying());
    frame = animation->GetCurrentFrame();
}

void pf::Packet::CharacterAnimation::Send(sf::SocketTCP *socket) {
    char type = packetType;
    socket->Send((const char *)&type, sizeof(type));
    socket->Send((const char *)&data, sizeof(data));
    socket->Send((const char *)&frame, sizeof(frame));
}

bool pf::Packet::CharacterAnimation::IsFacingRight() {
    return data & 0x01;
}

bool pf::Packet::CharacterAnimation::IsPlaying() {
    return data & 0x02;
}

bool pf::Packet::CharacterAnimation::ShouldGotoFrame() {
    return data & 0x04;
}

pf::Packet::OtherCharacterAnimation::OtherCharacterAnimation(sf::SocketTCP *socket) {
    std::size_t read;
    socket->Receive((char *)&entityID, sizeof(entityID), read);
    socket->Receive((char *)&data, sizeof(data), read);
    socket->Receive((char *)&frame, sizeof(frame), read);
}

pf::Packet::OtherCharacterAnimation::OtherCharacterAnimation(pf::Character *character) {
    pf::Animation *animation = character->GetImage();
    entityID = character->GetID();
    data = MakeData(character->GetDirection() == pf::Character::RIGHT, animation->IsPlaying(), !animation->IsPlaying());
    frame = animation->GetCurrentFrame();
}

void pf::Packet::OtherCharacterAnimation::Send(sf::SocketTCP *socket) {
    char type = packetType;
    socket->Send((const char *)&type, sizeof(type));
    socket->Send((const char *)&entityID, sizeof(entityID));
    socket->Send((const char *)&data, sizeof(data));
    socket->Send((const char *)&frame, sizeof(frame));
}

bool pf::Packet::OtherCharacterAnimation::IsFacingRight() {
    return data & 0x01;
}

bool pf::Packet::OtherCharacterAnimation::IsPlaying() {
    return data & 0x02;
}

bool pf::Packet::OtherCharacterAnimation::ShouldGotoFrame() {
    return data & 0x04;
}

pf::Packet::TeleportEntity::TeleportEntity(sf::SocketTCP *socket) {
    std::size_t read;
    socket->Receive((char *)&entityID, sizeof(entityID), read);
    socket->Receive((char *)&x, sizeof(x), read);
    socket->Receive((char *)&y, sizeof(y), read);
}

pf::Packet::TeleportEntity::TeleportEntity(pf::Entity *entity) {
    entityID = entity->GetID();
    x = entity->GetX();
    y = entity->GetY();
}

void pf::Packet::TeleportEntity::Send(sf::SocketTCP *socket) {
    char type = packetType;
    socket->Send((const char *)&type, sizeof(type));
    socket->Send((const char *)&entityID, sizeof(entityID));
    socket->Send((const char *)&x, sizeof(x));
    socket->Send((const char *)&y, sizeof(y));
}

pf::Packet::AbsoluteMove::AbsoluteMove(sf::SocketTCP *socket) {
    std::size_t read;
    socket->Receive((char *)&x, sizeof(x), read);
    socket->Receive((char *)&y, sizeof(y), read);
}

pf::Packet::AbsoluteMove::AbsoluteMove(pf::Entity *entity) {
    x = entity->GetX();
    y = entity->GetY();
}

void pf::Packet::AbsoluteMove::Send(sf::SocketTCP *socket) {
    char type = packetType;
    socket->Send((const char *)&type, sizeof(type));
    socket->Send((const char *)&x, sizeof(x));
    socket->Send((const char *)&y, sizeof(y));
}

pf::Packet::Health::Health(sf::SocketTCP *socket) {
    std::size_t read;
    socket->Receive((char *)&entityID, sizeof(entityID), read);
    socket->Receive((char *)&health, sizeof(health), read);
}

pf::Packet::Health::Health(pf::Character *character) {
    entityID = character->GetID();
    health = (int)character->GetHealth();
}

void pf::Packet::Health::Send(sf::SocketTCP *socket) {
    char type = packetType;
    socket->Send((const char *)&type, sizeof(type));
    socket->Send((const char *)&entityID, sizeof(entityID));
    socket->Send((const char *)&health, sizeof(health));
}