/*
 * Tileset.h
 * List of tiles, their names, reference colors, tileset offsets, and other tile info.
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

#ifndef TILESET_H
#define TILESET_H

#include <SFML/Graphics.hpp>

namespace pf {
    namespace Tileset {
        struct Tile {
            char *name;
            bool solid;
            sf::IntRect coords;
            sf::Color levelColor;
            float alpha;
            bool liquid;

            Tile(char *name, bool solid, sf::IntRect coords, sf::Color levelColor, float alpha, bool liquid) {
                this->name = name;
                this->solid = solid;
                this->coords = coords;
                this->levelColor = levelColor;
                this->alpha = alpha;
                this->liquid = liquid;
            }
        };

        const static sf::Color Spawn = sf::Color::Red;

        const static Tile Tiles[] = {
            Tile("engraved", true, sf::IntRect(0, 0, 16, 16), sf::Color(0, 0, 0),  1.f, false),
            Tile("dirt", true, sf::IntRect(16, 0, 32, 16), sf::Color(128, 64, 0), 1.f, false),
            Tile("grass", true, sf::IntRect(32, 0, 48, 16), sf::Color(0, 255, 0), 1.f, false),
            Tile("bg_redflower", false, sf::IntRect(48, 0, 64, 16), sf::Color(255, 0, 128), 1.f, false),
            Tile("water", true, sf::IntRect(64, 0, 80, 16), sf::Color(0, 0, 255), 0.3f, true),
            Tile("bg_wall1", false, sf::IntRect(80, 0, 96, 16), sf::Color(40, 50, 40), 1.f, false)
        };

        const static int Count = sizeof(Tiles) / sizeof(Tile);
    } // namespace Tileset
}; // namespace pf

#endif // TILESET_H
