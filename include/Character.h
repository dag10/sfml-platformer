/*
 * Character.h
 * Manages a character, its animation, and state
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

#ifndef CHARACTER_H
#define CHARACTER_H

#include <SFML/Graphics.hpp>
#include "PhysicsEntity.h"

namespace pf {
    class Animation;
    class Resource;
    class CharacterSkin;
#ifdef PLATFORMER_SERVER
    class ClientInstance;
    class Server;
#endif

    class Character : public pf::PhysicsEntity {
        public:
            // Speed
            const static float WALK_SPEED = 110.f;
            const static float SWIM_SPEED = 50.f;

            // Direction
            const static int LEFT = -1;
            const static int RIGHT = 1;

            Character(pf::World *world, pf::CharacterSkin *skin, const char *name);
            ~Character();

            void Tick(float frametime);
            void Render(sf::RenderTarget& target);
            void RenderOverlays(sf::RenderTarget& target);

            void SetName(const char *name);
            char *GetName();
            void ShowName();
            void HideName();
            bool IsShowingName();

            int GetDirection();
            bool IsWalking();

            float GetHealth();
            void SetHealth(float health);
            void ShowHealth();
            void HideHealth();
            bool IsShowingHealth();

            void WalkRight();
            void WalkLeft();
            void StopWalking();
            void StartWalking();
            void FaceRight();
            void FaceLeft();

#ifdef PLATFORMER_SERVER
            void SetClient(pf::ClientInstance *client);
            pf::ClientInstance *GetClient();
            void SetServer(pf::Server *server);
            pf::Server *GetServer();
#endif

            pf::CharacterSkin *GetSkin();
            pf::Animation *GetImage();

            void SetIsolateAnimation(bool isolateAnimation);

            bool CanCollideWith(pf::Entity *entity);

        private:
            static sf::Font *nameFont;

            sf::Image *spriteSheet;
            pf::CharacterSkin *skin;

            float speed;
            bool walking;
            int direction;
            float health;

            sf::String *name;
            sf::Shape *nameBackground;
            sf::Shape *healthBackground;
            sf::Shape *healthBarGreen;
            sf::Shape *healthBarRed;
            bool showName;
            bool showHealth;
            bool isolateAnimation;

#ifdef PLATFORMER_SERVER
            pf::ClientInstance *client;
            pf::Server *server;
#endif
    };
}; // namespace pf

#endif // CHARACTER_H
