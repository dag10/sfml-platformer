/*
 * ClientInstance.h
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

#ifndef CLIENTINSTANCE_H
#define CLIENTINSTANCE_H

#include <SFML/Network.hpp>
#include <vector>
#include <queue>

namespace pf {
    namespace Packet {
        class BasePacket;
    }

    class Resource;
    class Character;
    class Server;

    class ClientInstance {
    public:
        ClientInstance(pf::Server *server, sf::SocketTCP *socket, sf::IPAddress *clientIP);
        ~ClientInstance();

        sf::SocketTCP *GetSocket();
        sf::IPAddress *GetAddress();

        void SetUsername(char *username);
        char *GetUsername();

        void Kick(char *message);
        bool WasKicked();

        void EnqueuePacket(pf::Packet::BasePacket *packet);
        void EnqueueResource(pf::Resource *resource);
        pf::Packet::BasePacket *DequeuePacket();
        int QueuedResources();
        int QueuedPackets();

        bool IsLoading();
        void BeginLoading();
        void EndLoading();

        void SetCharacter(pf::Character *character);
        pf::Character *GetCharacter();

    private:
        sf::SocketTCP *socket;
        sf::IPAddress clientIP;

        pf::Server *server;
        std::queue<pf::Packet::BasePacket*> packetQueue;
        int resourceCount;
        bool loading;

        char *username;
        bool wasKicked;

        pf::Character *character;
    };
}; // namespace pf

#endif // CLIENTINSTANCE_H
