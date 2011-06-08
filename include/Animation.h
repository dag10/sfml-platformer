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

#ifndef ANIMATION_H
#define ANIMATION_H

#include <SFML/Graphics.hpp>
#include "IRenderable.h"

namespace pf {
    class Animation : public sf::Sprite, public pf::IRenderable {
        public:
            Animation(sf::Image& image, int frames, int framerate);
            ~Animation();

            void Tick(float frametime);
            void Render(sf::RenderTarget& target);
            void Play();
            void Pause();
            bool IsPlaying();
            int GetCurrentFrame();
            void SetCurrentFrame(int frame);
            int GetWidth();
            int GetHeight();
            int GetFramerate();
            void SetFramerate(int framerate);

        private:
            int framerate;
            int frames;
            float currentFrame;
            bool playing;
            int frameWidth, frameHeight;

            void UpdateRect();
    };
}; // namespace pf

#endif // ANIMATION_H
