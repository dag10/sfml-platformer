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
 * Config parser wrapper for easier access.
 *
 * AUTHOR
 * Vasek Blazek <blazek@firma.seznam.cz>
 *
 */


#ifndef __CONFIGWRAPPER_H__
#define __CONFIGWRAPPER_H__

#include <sys/time.h>

#include <string>

#include "cfgparser/cfgparser.h"

struct ConfigWrapper_t {
    ConfigWrapper_t(const ConfigParser_t &config)
        : config(config)
    {}

    /** @short Reads string value from config.
     *
     * If option is not found, value is left untouched.
     *
     * @param section section if config
     * @param option option in the given section
     * @param value read string value
     */
    void getString(
            const std::string &section,
            const std::string &option, std::string &value) const;

    /** @short Reads unsigned int value from config.
     *
     * If option is not found, value is left untouched.
     *
     * @param section section if config
     * @param option option in the given section
     * @param value read unsigned int value
     */
    void getInt(
            const std::string &section,
            const std::string &option, unsigned int &value) const;

    /** @short Reads int value from config.
     *
     * If option is not found, value is left untouched.
     *
     * @param section section if config
     * @param option option in the given section
     * @param value read int value
     */
    void getInt(
            const std::string &section,
            const std::string &option, int &value) const;

    /** @short Reads time_t value from config.
     *
     * If option is not found, value is left untouched.
     *
     * @param section section if config
     * @param option option in the given section
     * @param value read time_t value
     */
    void getTime(
            const std::string &section,
            const std::string &option, time_t &value) const;

    /** @short Reads float value from config.
     *
     * If option is not found, value is left untouched.
     *
     * @param section section if config
     * @param option option in the given section
     * @param value read float value
     */
    void getFloat(
            const std::string &section,
            const std::string &option, float &value) const;

    /** @short Reads bool value from config.
     *
     * If value is 'true' or 'yes' (in any case) value is set to true.
     * If value is 'false' or 'no' (in any case) value is set to
     * false. Otherwise value is left untouched and error message is
     * given.
     *
     * @param section section if config
     * @param option option in the given section
     * @param value read bool value
     */
    void getBool(
            const std::string &section,
            const std::string &option, bool &value) const;

    /** @short Defines time record type in config (stored in sec or msec).
      */
    enum TimeType_t {
        TT_SECONDS,
        TT_MILISECONDS,
    };

     /** @short Reads timeval value from config.
     *
     * If option is not found, value is left untouched.
     *
     * @param section section if config
     * @param option option in the given section
     * @param defaultValue dafault time value
     * @param timeType time record type (TT_SECONDS or TT_MILISECONDS)
     *
     * @return struct timeval - if not found creates timeval from defaultValue
     */
    struct timeval getTimeval(
            const std::string &section, const std::string &option,
            unsigned int defaultValue, TimeType_t timeType) const;

    const ConfigParser_t &config;
};

#endif // __CONFIGWRAPPER_H__
