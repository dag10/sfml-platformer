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