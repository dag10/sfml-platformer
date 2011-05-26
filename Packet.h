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
    namespace Packet {
        static const char PROTOCOL_VERSION = 1;
        
        struct PacketString {
            uint16_t length;
            char *string;
            
            PacketString(char *string) {
                length = strlen(string);
                this->string = string;
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
    }; // namespace Packet
}; // namespace pf

#endif // PACKET_H
