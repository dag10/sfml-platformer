/*
 * main_client.cpp
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

#include <SFML/Graphics.hpp>
#include "Game.h"
#include <cstdio>
#include <iostream>
using namespace std;

const int width = 800;
const int height = 600;

sf::RenderWindow window;
sf::Input *input;
pf::Game *game;
sf::String FPStext;
sf::View HUDview;

bool init();
bool loop();
void cleanup();
void handleEvent(sf::Event *event);

int main() {
    if (init())
        while(loop());
    cleanup();

    return EXIT_SUCCESS;
}

bool init() {
    game = new pf::Game();
    window.Create(sf::VideoMode(width / 1, height / 1, 32),
                  "A Platform Game",
                  sf::Style::Close);
    window.SetFramerateLimit(60);
    window.UseVerticalSync(true);
    ((sf::Window*)&window)->SetSize(width, height);
    input = (sf::Input*)&window.GetInput();

    FPStext.SetPosition(20, 20);
    FPStext.SetColor(sf::Color::Magenta);

    HUDview.SetFromRect(sf::FloatRect(0.f,
                                      0.f,
                                      width,
                                      height));

    return true;
}

bool loop() {
    float frameTime = window.GetFrameTime();
    if (frameTime > 0.15f) frameTime = 0.f;
    game->Tick(*input, frameTime);
    
    game->Render(window, width, height);

    // Update FPS
    char fpsText[16];
    sprintf(fpsText, "FPS: %d", (int)(1.f / frameTime));
    FPStext.SetText(fpsText);
    window.SetView(HUDview);
    window.Draw(FPStext);

    window.Display();

    sf::Event event;
    while(window.GetEvent(event))
        handleEvent(&event);

    return window.IsOpened();
}

void cleanup() {
    if (game) {
        delete game;
        game = NULL;
    }
}

void handleEvent(sf::Event *event) {
    switch (event->Type) {
        case sf::Event::KeyPressed:
            switch (event->Key.Code) {
                case sf::Key::Escape:
                    window.Close();
                    break;
            }
            break;
        case sf::Event::Closed:
            window.Close();
            break;
        case sf::Event::LostFocus:
            cout << "LOST FOCUS" << endl;
            break;
    }
}
