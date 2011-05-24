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
        Screen_Joining
    };

    class Game {
        public:
            static const int UI_SPACING = 10;
        
            Game(sf::RenderWindow& renderWindow);
            ~Game();

            void Render(sf::RenderTarget& target, int renderWidth, int renderHeight);
            void Tick(sf::Input& input, float frametime);
            
            void HandleClick(sf::Input& input);
            void HandleEvent(sf::Event *event);
            
            sf::Vector2f GetCursorPosition();
            
            void SetScreen(Screen screen);
            Screen GetScreen();

        private:
            pf::World *world;
            sf::View *view;
            float viewX, viewY;
            float viewSpeed;
            void addBox(int x, int y);
            sf::RenderTarget *lastTarget;
            pf::Character *mainCharacter, *secondCharacter;
            pf::PhysicsEntity *box;
            pf::Particle *particle;
            float zoomFactor;
            sf::Vector2f cursorPosition;
            
            static sf::Font *labelFont;
            void InitGUI(sf::RenderWindow& renderWindow);
            cp::cpGuiContainer *menuContainer;
            cp::cpTextInputBox *nameBox;
            sf::String *nameLabel;
            cp::cpTextInputBox *ipBox;
            sf::String *ipLabel;
            cp::cpButton *joinButton;
            
            pf::Screen screen;
            sf::Shape *screenBackground;
    };
}; // namespace pf

#endif // GAME_H
