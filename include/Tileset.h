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
    struct Tile {
        char *name;
        bool solid;
        sf::IntRect coords;
        sf::Color levelColor;

        Tile(char *name, bool solid, sf::IntRect coords, sf::Color levelColor) {
            this->name = name;
            this->solid = solid;
            this->coords = coords;
            this->levelColor = levelColor;
        }
    };

    const static Tile Tileset[] = {
        Tile("engraved", true, sf::IntRect(0, 0, 16, 16), sf::Color(0, 0, 0)),
        Tile("dirt", true, sf::IntRect(16, 0, 32, 16), sf::Color(128, 64, 0)),
        Tile("grass", true, sf::IntRect(32, 0, 48, 16), sf::Color(0, 255, 0)),
        Tile("redflower", false, sf::IntRect(48, 0, 64, 16), sf::Color(255, 0, 128))
    };

    const static int TilesetCount = sizeof(Tileset) / sizeof(Tile);

}; // namespace pf

#endif // TILESET_H
