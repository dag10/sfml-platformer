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