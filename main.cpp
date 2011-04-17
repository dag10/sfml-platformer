/*
 * main.cpp
 * Copyright (c) 2010 Drew Gottlieb
 */

#include <SFML/Graphics.hpp>
#include <Game.h>
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
    window.Create(sf::VideoMode(width / 2, height / 2, 32),
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

    game->Render(window, width / 2, height / 2);

    // Update FPS
    char fpsText[16];
    wsprintf(fpsText, "FPS: %d", (int)(1.f / frameTime));
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
