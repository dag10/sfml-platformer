/*
 * PhysicsEntity.h
 * A physics-enabled renderable entity
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

#ifndef PHYSICSENTITY_H
#define PHYSICSENTITY_H

#include <Entity.h>
#include <IRenderable.h>
#include <World.h>
#include <vector>

namespace pf {
    class PhysicsEntity : public pf::Entity, public pf::IRenderable {
        public:
            const static float GRAVITY = 9.8f * pf::World::TILE_SIZE;  // 9.8 m/s * pixels per tile

            PhysicsEntity(pf::World *world);
            PhysicsEntity(pf::World *world, sf::Image *image);
            PhysicsEntity(pf::World *world, float x, float y, int width, int height);
            PhysicsEntity(pf::World *world, sf::Image *image, float x, float y);
            ~PhysicsEntity();

            void Init();

            void Tick(float frametime);
            void Render(sf::RenderTarget& target);

            bool IsMoving();
            float GetVelocityX();
            float GetVelocityY();
            void SetVelocity(float veloX, float veloY);
            void SetVelocityX(float veloX);
            void SetVelocityY(float veloY);
            bool IsSolid();
            void SetSolid(bool solid);
            bool GravityIsEnabled();
            void SetGravityEnabled(bool gravity);
            bool IsOnGround();
            bool IsPushable();
            void SetPushable(bool pushable);
            bool CanUseStairs();
            void SetCanUseStairs(bool canUseStairs);

            bool HitTest(pf::Entity& entity);
            bool HitTest(float x, float y, float width, float height);
            bool HitTest(float x, float y);

            bool AlreadyHit(pf::Entity *entity);
            void Push(float offsetX, float offsetY);

        private:
            friend class Character;

            sf::Sprite *sprite;
            bool solid, gravity, pushable, canUseStairs;
            float veloX, veloY;
            bool wasHittingVerticalSurface, wasHittingHorizontalSurface;
            bool onGround;
            std::vector<pf::Entity*> *hitEntities;
            void Move(float offsetX, float offsetY);
    };
}; // namespace pf

#endif // PHYSICSENTITY_H
