/*
  * Packet.h
  * Sends, receives, and represents packets for the protocol
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

#ifndef PACKET_H
#define PACKET_H

#include <string>

namespace sf {
    class SocketTCP;
}

namespace pf {
    class Resource;
    
    namespace Packet {
        static const char PROTOCOL_VERSION = 1;
        
        struct PacketString {
            uint16_t length;
            char *string;
            
            PacketString(char *string) {
                length = strlen(string);
                this->string = new char[length + 1];
                memcpy(this->string, string, length + 1);
            }
            
            PacketString(sf::SocketTCP *socket);
            void Send(sf::SocketTCP *socket);
            
            ~PacketString() {
                delete [] string;
            }
        };
        
        struct LoginRequest {
            static const char packetType = 0x01;
            char clientProtocolVersion;
            PacketString *username;
            
            LoginRequest(char *username) {
                clientProtocolVersion = PROTOCOL_VERSION;
                this->username = new PacketString(username);
            }
            
            LoginRequest(sf::SocketTCP *socket);
            void Send(sf::SocketTCP *socket);
            
            ~LoginRequest() {
                delete username;
            }
        };
        
        struct Kick {
            static const char packetType = 0x07;
            PacketString *reason;
            
            Kick(char *reason) {
                this->reason = new PacketString(reason);
            }
            
            Kick(sf::SocketTCP *socket);
            void Send(sf::SocketTCP *socket);
            
            ~Kick() {
                delete reason;
            }
        };
        
        struct BeginLoad {
            static const char packetType = 0x02;
            uint16_t numResources;
            
            BeginLoad(uint16_t numResources) {
                this->numResources = numResources;
            }
            
            BeginLoad(sf::SocketTCP *socket);
            void Send(sf::SocketTCP *socket);
            
            ~BeginLoad() {}
        };
        
        struct EndLoad {
            static const char packetType = 0x03;
            
            EndLoad() {}
            
            EndLoad(sf::SocketTCP *socket);
            void Send(sf::SocketTCP *socket);
            
            ~EndLoad() {}
        };
        
        struct Resource {
            static const char packetType = 0x04;
            PacketString *filename;
            uint32_t length;
            char *data;
            
            Resource(pf::Resource *resource);

            Resource(sf::SocketTCP *socket);
            void Send(sf::SocketTCP *socket);
            
            pf::Resource *GetResource();
            
            ~Resource() {
                delete filename;
            }
        };
        
        struct Property {
            static const char packetType = 0x05;
            PacketString *name;
            PacketString *value;
            
            Property(char *name, char *value) {
                this->name = new PacketString(name);
                this->value = new PacketString(value);
            }
            
            Property(sf::SocketTCP *socket);
            void Send(sf::SocketTCP *socket);
            
            ~Property() {
                delete name;
                delete value;
            }
        };
    }; // namespace Packet
}; // namespace pf

#endif // PACKET_H
