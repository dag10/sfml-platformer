/*
 * Server.h
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

#ifndef SERVER_H
#define SERVER_H

#include <SFML/Network.hpp>
#include "Packet.h"
#include <vector>
#include <map>

namespace pf {
    class World;
    class ClientInstance;
    class Resource;

    typedef std::map<sf::SocketTCP, pf::ClientInstance*> ClientMap;
    typedef std::map<std::string, std::string> PropertyMap;

    class Server {
    public:
        Server();
        ~Server();

        void Kick(pf::ClientInstance *client, char *message);
        void SendToAll(pf::Packet::BasePacket *packet);
        void SendToAll(pf::Packet::BasePacket *packet, pf::ClientInstance *exclude);
        void RequireResource(pf::Resource *resource);

    private:
        sf::SelectorTCP socketSelector;
        sf::SocketTCP *listenSocket;
        sf::IPAddress serverIP;
        unsigned short serverPort;

        PropertyMap properties;
        ClientMap clientMap;
        std::vector<pf::Resource*> requiredResources;

        bool shouldQuit;
        pf::World *world;
    };
}; // namespace pf

#endif // SERVER_H
