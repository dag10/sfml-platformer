/* 
 * Cfgparser - configuration reader C++ library
 * Copyright (c) 2008, Seznam.cz, a.s.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Seznam.cz, a.s.
 * Radlicka 2, Praha 5, 15000, Czech Republic
 * http://www.seznam.cz, mailto:cfgparser@firma.seznam.cz
 * 
 * $Id: $
 *
 * PROJECT
 * Configuration file parser
 *
 * DESCRIPTION
 * Config parser class definition. This class can read and parse config files.
 *
 * AUTHOR
 * Solamyl <stepan@firma.seznam.cz>
 *
 */


#ifndef _CFGPARSER_H
#define _CFGPARSER_H

#include <string>
#include <vector>
#include <map>


/** This is a config file class. It can read and parse
 * config files in style of M$ windows 3.1 INI-files. */
class ConfigParser_t {
private:

    /** Structure used for single line of config file. */
    struct entry_s {
        /** Entry is in this section.
         * Section name is the std::string between '[' and ']'.
         * Entries on top of config file have empty section. */
        std::string section;
        /** Entry name.
         * Name is the std::string from start of the line
         * until (but not including) '=', '#', ';' or EOL. */
        std::string name;
        /** Value of entry.
         * Value is std::string after '=' and before '#', ';' or EOL
         * (also not including these special characters). */
        std::string value;

        /** Clear entry data.
         * \return Reference to 'this' object with cleared content. */
        struct entry_s & clear(void)
        {
            /* clear entry properties */
            section.erase();
            name.erase();
            value.erase();
            /* return itself */
            return *this;
        }
    };

    /** Config file entries.
     * An array of entry structures. */
    std::vector<struct entry_s> entry;


public:

    /** Default constructor.
     * Construct empty config file. */
    ConfigParser_t(void);

    /** Create config object with content loaded from file.
     * Method reads and parses content of given file.
     * On error throws exception (std::string).
     * \param fname File name of config file. */
    explicit ConfigParser_t(const std::string &fname);

    /** Read and parse config file.
     * \return 0 on success, -1 on error.
     * \param fname File name of config file. */
    int readFile(const std::string &fname);

    /** Get value of given entry name (std::string).
     * If entry is not found then value is left untouched
     * (good for pre-initialization with default value).
     * \return 1=entry found, 0=not found.
     * \param section Section in which is the desired entry.
     * \param name Name of the entry.
     * \param value Buffer for entry value. */
    int getValue(
            std::string section, std::string name, std::string *value) const;

    /** Get value of given entry name (int).
     * If entry is not found or has invalid integer value,
     * then value param is left untouched (good for pre-initialization
     * with defaults).
     * \return 1=entry found, 0=not found.
     * \param section Dection in which is the desired entry.
     * \param name Name of the entry.
     * \param value Buffer for entry value. */
    int getValue(std::string section, std::string name, int *value) const;

    /** Get value of given entry name (double).
     * If entry is not found or has invalid double valus,
     * then value param is left untouched (good for pre-initialization
     * with defaults).
     * \return 1=entry found, 0=not found.
     * \param section Section in which is the desired entry.
     * \param name Name of the entry.
     * \param value Buffer for entry value. */
    int getValue(std::string section, std::string name, double *value) const;

    /** Get values of given entry name.
     * Returns all occurences of the same section/name entries.
     * If entry is not found then value is left untouched
     * (good for pre-initialization with default value).
     * \return 1=entry found, 0=not found.
     * \param section Section in which is the desired entry.
     * \param name Name of the entry.
     * \param value Vector buffer for entry values. */
    int getValue(
            std::string section,
            std::string name, std::vector<std::string> *value) const;

    /** Get value of given entry name (boolean).
     * If entry is not found then value is left untouched
     * (good for pre-initialization with default value).
     * true: "1", "true", "on", "yes"
     * false: "0", "false", "off", "no"
     * other std::strings are invalid
     * (all comparisions are case insensitive)
     * \return 1=entry found, 0=not found.
     * \param section Section in which is the desired entry.
     * \param name Name of the entry.
     * \param value Buffer for entry value. */
    int getValue(std::string section, std::string name, bool *value) const;

    /**
     * Returns a map of name-value pairs from a section specified by the argument. A multimap
     * is returned because there may be more than one row with the same name in a section (such
     * rows are subsequently considered parts of an array of values).
     * @param section the name of the desired section
     * @return name-value pairs from the section
     */
    std::multimap<std::string, std::string> getOptions(const std::string &section) const;

    /**
     * Returns a vector of names of sections found in the config file (possible duplicates are eliminated)
     * @return list of section names
     */
    std::vector<std::string> getSections() const;
};

#endif /*_CFGPARSER_H*/
