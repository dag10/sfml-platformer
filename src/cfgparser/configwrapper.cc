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


#include <ctype.h>

#include <string>
#include <algorithm>

//#include <dbglog.h>

#include "cfgparser/configwrapper.h"

using std::string;

void ConfigWrapper_t::getString(
        const string &section, const string &option, string &value) const {

    string tmp;
    if (config.getValue(section, option, &tmp)) {
        value = tmp;
    } else {
        //LOG(WARN3, "Option [%s]::%s not given. "
         //   "Using default '%s'.", section.c_str(), option.c_str(),
        //    value.c_str());
    }
}

void ConfigWrapper_t::getInt(
        const string &section,
        const string &option, unsigned int &value) const {

    int tmp;
    if (config.getValue(section, option, &tmp)) {
        //if (tmp < 0)
            //LOG(WARN3, "Negative value of [%s]::%s. "
            //    "Using default %u.", section.c_str(),
            //    option.c_str(), value);
        //else value = tmp;
        if (tmp >= 0) value = tmp;
    } else {
        //LOG(WARN3, "Option [%s]::%s not given or not integer. "
        //    "Using default %u.", section.c_str(), option.c_str(), value);
    }
}

void ConfigWrapper_t::getInt(
        const string &section, const string &option, int &value) const {

    int tmp;
    if (config.getValue(section, option, &tmp)) {
        value = tmp;
    } else {
       // LOG(WARN3, "Option [%s]::%s not given or not integer. "
        //    "Using default %d.", section.c_str(), option.c_str(), value);
    }
}

void ConfigWrapper_t::getTime(
        const string &section, const string &option, time_t &value) const {

    int tmp;
    if (config.getValue(section, option, &tmp)) {
        //if (tmp < 0)
            //LOG(WARN3, "Negative value of [%s]::%s. "
            //    "Using default %lu.", section.c_str(),
            //    option.c_str(), value);
       // else value = tmp;
       if (tmp >= 0) value = tmp;
    } else {
        //LOG(WARN3, "Option [%s]::%s not given or not integer. "
        //    "Using default %lu.", section.c_str(), option.c_str(), value);
    }
}

void ConfigWrapper_t::getFloat(
        const string &section, const string &option, float &value) const {

    double tmp;
    if (config.getValue(section, option, &tmp)) {
        //if (tmp <= 0)
           // LOG(WARN3, "Negative value of [%s]::%s. "
           //     "Using default %f.", section.c_str(),
           //     option.c_str(), value);
        //else value = tmp;
       if (tmp > 0) value = tmp;
    } else {
       // LOG(WARN3, "Option [%s]::%s not given or not integer. "
        //    "Using default %f.", section.c_str(), option.c_str(), value);
    }
}

void ConfigWrapper_t::getBool(
        const string &section, const string &option, bool &value) const {

    string tmp;
    if (config.getValue(section, option, &tmp)) {
        std::transform(tmp.begin(), tmp.end(), tmp.begin(), toupper);
        if ((tmp == "TRUE") || (tmp == "YES")) value = true;
        else if ((tmp == "FALSE") || (tmp == "NO")) value = false;
        else {
           // LOG(WARN3, "Invalid value of boolean option [%s]::%s. "
           //     "Using default %s.", section.c_str(),
           //     option.c_str(), value ? "true" : "false");
        }
    } else {
       // LOG(WARN3, "Option [%s]::%s not given. "
       //     "Using default %s.", section.c_str(), option.c_str(),
       //     value ? "true" : "false");
    }
}

struct timeval ConfigWrapper_t::getTimeval(
        const string &section, const string &option,
        unsigned int defaultValue, TimeType_t timeType) const {

    struct timeval tv;
    getInt(section, option, defaultValue);
    switch (timeType) {
    case TT_MILISECONDS:
        tv.tv_sec = defaultValue / 1000;
        tv.tv_usec = (defaultValue % 1000) * 1000;
        break;
    case TT_SECONDS:
    default:
        tv.tv_sec = defaultValue;
        tv.tv_usec = 0;
        break;
    }
    return tv;
}
