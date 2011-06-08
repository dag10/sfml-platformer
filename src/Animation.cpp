/*
 * Animation.cpp
 * Manages an animated sprite
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

#include "Animation.h"

pf::Animation::Animation(sf::Image& image, int frames, int framerate) {
    this->SetImage(image);
    this->frames = frames;
    this->framerate = framerate;

    playing = false;
    frameWidth = image.GetWidth() / frames;
    frameHeight = image.GetHeight();
    currentFrame = 0.f;

    UpdateRect();
}

pf::Animation::~Animation() {

}

void pf::Animation::Render(sf::RenderTarget& target) {
    target.Draw((sf::Sprite)*this);
}

void pf::Animation::Tick(float frametime) {
    if (playing) {
        UpdateRect();
        currentFrame += (float)framerate * frametime;
        while (currentFrame > frames) currentFrame -= frames;
    }
}

void pf::Animation::UpdateRect() {
    this->SetSubRect(sf::IntRect(frameWidth * (int)currentFrame,
                                 0,
                                 frameWidth * (int)currentFrame + frameWidth,
                                 frameHeight));
}

int pf::Animation::GetWidth() {
    return frameWidth;
}

int pf::Animation::GetHeight() {
    return frameHeight;
}

void pf::Animation::Play() {
    playing = true;
}

void pf::Animation::Pause() {
    playing = false;
}

bool pf::Animation::IsPlaying() {
    return playing;
}

int pf::Animation::GetCurrentFrame() {
    return currentFrame;
}

void pf::Animation::SetCurrentFrame(int frame) {
    currentFrame = frame;
}

int pf::Animation::GetFramerate() {
    return framerate;
}

void pf::Animation::SetFramerate(int framerate) {
    if (framerate >= 0)
        this->framerate = framerate;
}
