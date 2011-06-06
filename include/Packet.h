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

#include "Logger.h"
#include <string>

namespace sf {
    class SocketTCP;
}

namespace pf {
    class Resource;
    class CharacterSkin;
    class Character;
    
    namespace Packet {
        static const char PROTOCOL_VERSION = 1;
        
        struct BasePacket {
            virtual void Send(sf::SocketTCP *socket) {};
        };
        
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
        
        struct LoginRequest : BasePacket {
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
        
        struct Kick : BasePacket {
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
        
        struct BeginLoad : BasePacket {
            static const char packetType = 0x02;
            uint16_t numResources;
            
            BeginLoad(uint16_t numResources) {
                this->numResources = numResources;
            }
            
            BeginLoad(sf::SocketTCP *socket);
            void Send(sf::SocketTCP *socket);
            
            ~BeginLoad() {}
        };
        
        struct EndLoad : BasePacket {
            static const char packetType = 0x03;
            
            EndLoad() {}
            
            EndLoad(sf::SocketTCP *socket);
            void Send(sf::SocketTCP *socket);
            
            ~EndLoad() {}
        };
        
        struct Resource : BasePacket {
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
        
        struct Property : BasePacket {
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
        
        struct SpawnCharacter : BasePacket {
            static const char packetType = 0x08;
            uint32_t entityID;
            PacketString *username;
            PacketString *skin;
            float x, y;
            
            SpawnCharacter(int entityID, char *username, char *skin, float x, float y) {
                this->entityID = entityID;
                this->username = new PacketString(username);
                this->skin = new PacketString(skin);
                this->x = x;
                this->y = y;
            }
            
            SpawnCharacter(pf::Character *character);
            
            SpawnCharacter(sf::SocketTCP *socket);
            void Send(sf::SocketTCP *socket);
            
            ~SpawnCharacter() {
                delete username;
                delete skin;
            }
        };
        
        struct CharacterSkin : BasePacket {
            static const char packetType = 0x09;
            PacketString *name;
            PacketString *resource;
            uint16_t width, height;
            char framerate;
            uint16_t frames;
            
            CharacterSkin(char *name, char *resource, unsigned short width, unsigned short height, char framerate, unsigned short frames) {
                this->name = new PacketString(name);
                this->resource = new PacketString(resource);
                this->width = width;
                this->height = height;
                this->framerate = framerate;
                this->frames = frames;
            }
            
            CharacterSkin(pf::CharacterSkin *skin);
            
            CharacterSkin(sf::SocketTCP *socket);
            void Send(sf::SocketTCP *socket);
            
            pf::CharacterSkin *GetCharacterSkin();
            
            ~CharacterSkin() {
                delete name;
                delete resource;
            }
        };
        
        struct StartWorld : BasePacket {
            static const char packetType = 0x0B;
            
            StartWorld() {}
            
            StartWorld(sf::SocketTCP *socket);
            void Send(sf::SocketTCP *socket);
            
            ~StartWorld() {}
        };
        
        struct SetCharacter : BasePacket {
            static const char packetType = 0x06;
            uint32_t entityID;
            
            SetCharacter(int entityID) {
                this->entityID = entityID;
            }
            
            SetCharacter(pf::Character *character);
            
            SetCharacter(sf::SocketTCP *socket);
            void Send(sf::SocketTCP *socket);
            
            ~SetCharacter() {}
        };
    }; // namespace Packet
}; // namespace pf

#endif // PACKET_H
