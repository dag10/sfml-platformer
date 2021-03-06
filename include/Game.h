/*
 * Game.h
 * Manages a game session. Contains level, player(s), and entities
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

#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <map>
#include <list>

namespace cp {
    class cpGuiContainer;
    class cpTextInputBox;
    class cpButton;
}

class sf::Event;

namespace pf {
    class Character;
    class PhysicsEntity;
    class Particle;
    class World;

    enum Screen {
        Screen_Game,
        Screen_Main,
        Screen_Joining,
        Screen_Disconnect,
        Screen_Chat
    };

    struct ChatMessage {
        sf::String *string;
        float countdown;

        ChatMessage(const char *string) {
            this->string = new sf::String(string);
            this->string->SetSize(16.f);
            countdown = 6;
        }

        ~ChatMessage() {
            delete string;
        }
    };

    typedef std::map<std::string, std::string> PropertyMap;
    typedef std::list<ChatMessage*> ChatMessageQueue;

    class Game {
        public:
            static const int UI_SPACING = 10;
            static const int CHAT_UI_SPACING = 10;
            static const int CHAT_MESSAGE_SPACING = 5;
            static const int MAX_CHAT_MESSAGES = 10;
            static const float DEFAULT_ZOOM = 2.5f;

            Game(sf::RenderWindow& renderWindow);
            ~Game();

            void Render(sf::RenderTarget& target, int renderWidth, int renderHeight);
            bool Tick(sf::Input& input, float frametime);

            void HandleEvent(sf::Event *event, sf::Input *input);

            sf::Vector2f GetCursorPosition();

            void Disconnect(char *message);

            void SetScreen(Screen screen);
            Screen GetScreen();

        private:
            pf::World *world;
            sf::View *view;
            float viewX, viewY;
            float viewSpeed, zoomSpeed;
            bool followCharacter;
            void addBox(int x, int y);  // FOR TESTING STUFF ONLY. NOT TO BE USED IN FINAL CODE.
            sf::RenderTarget *lastTarget;
            pf::Character *localCharacter;
            float zoomFactor, targetZoomFactor;
            sf::Vector2f cursorPosition;

            void HandleClick(sf::Input& input);
            void JoinGame();

            char *playerName;
            sf::IPAddress serverIP;
            unsigned short serverPort;
            sf::SocketTCP *socket;
            sf::SelectorTCP *socketSelector;

            int resourcesToLoad, resourcesLoaded;
            PropertyMap properties;

            void StopGame();
            void InitWorld();

            void SendChat(const char *message);
            bool switchingToChat;
            ChatMessageQueue chatMessages;

            static sf::Font *labelFont;
            void InitGUI(sf::RenderWindow& renderWindow);
            cp::cpGuiContainer *menuContainer;
            cp::cpTextInputBox *nameBox;
            sf::String *nameLabel;
            cp::cpTextInputBox *ipBox;
            sf::String *ipLabel;
            cp::cpTextInputBox *portBox;
            sf::String *portLabel;
            cp::cpButton *joinButton;
            cp::cpGuiContainer *chatContainer;
            cp::cpTextInputBox *chatBox;
            cp::cpGuiContainer *joiningContainer;
            sf::String *joiningLabel1, *joiningLabel2;
            void SetJoiningLabelText(char *line1, char *line2);
            cp::cpButton *joiningReturnButton;

            pf::Screen screen;
            sf::Shape *screenBackground;

            bool shouldQuit;
    };
}; // namespace pf

#endif // GAME_H
