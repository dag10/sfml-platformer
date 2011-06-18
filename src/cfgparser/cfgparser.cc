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
 * Config parser class. This class can read and parse config files.
 *
 * AUTHOR
 * Solamyl <stepan@firma.seznam.cz>
 *
 */


#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include <string>
#include <vector>
#include <algorithm>
#include <set>

//#include <dbglog.h>
#include "cfgparser/cfgparser.h"


using std::string;
using std::vector;

/*
 * C function for detecting library via configure script.
 * Function may be absolutely dummy, however it returns rcsid string.
 * You must link this additionaly with -lmdbg.
 * Sample from configure.in:
 * ...
 * AC_LANG_CPLUSPLUS
 * AC_CHECK_HEADER(cfgparser.h,FOUND="yes",FOUND="no)
 * AC_CHECK_LIB(cfgparser,cfgparser_dummy,FOUND="yes",FOUND="no",-lmdbg)
 * ...
 */
extern "C" {
    /* return rcsid string */
    const char * cfgparser_dummy(void)
    {
        return "$Id: cfgparser.cc 4909 2008-03-28 14:58:51Z michal.bukovsky $";
    }
}

/*
 * Default constructor.
 * Construct empty config file.
 */
ConfigParser_t::ConfigParser_t(void) {}

/*
 * Create config object with content loaded from file.
 * Method reads and parses content of given file.
 * On error throws exception (string).
 * Param fname: file name of config file
 */
ConfigParser_t::ConfigParser_t(const string &fname) {

    /* initialize from config file */
    if (readFile(fname) < 0)
        throw string("Config file '") + fname + string("' not found");
}

/*
 * Trim leading/trailing spaces from a string.
 * Return: pointer to the string passed into the function
 * Param str: pointer to the string which may be modified
 */
static string * strTrim(string *str) {

    /* count leading spaces */
    /*DBG(DBG1, " -> Trim spaces '%s'", str->c_str());*/
    int i = 0;
    while (i < (int)str->length() && isspace((*str)[i]))
        ++i;
    /* delete leading spaces */
    if (i > 0)
        str->erase(0, i);

    /* last char position */
    i = str->length();
    int j = i;
    while (i > 0 && isspace((*str)[i - 1]))
        --i;
    /* delete trailing spaces */
    if (i < j)
        str->erase(i, j);

    /* return string */
    /*DBG(DBG1, " <- Trim spaces '%s'", str->c_str());*/
    return str;
}


/*
 * Remove all leading and trailing spaces and
 * also shring multi-occurences of blanks into single space.
 * Return: pointer to the string passed into function
 * Param str: pointer to the string which may be modified
 */
static string * normalizeName(string *str) {

    /* trim spaces */
    /*DBG(DBG1, " -> Normalize string '%s'", str->c_str());*/
    strTrim(str);

    /* shrink multi-spaces */
    unsigned int i, j;
    i = 0;
    while (i < str->length())
    {
        /* if space */
        if (isspace((*str)[i]))
        {
            /* detect row of spaces */
            j = i;
            while (j + 1 < str->length() && isspace((*str)[j + 1]))
                ++j;
            /* if more than one space in row */
            if (i < j)
                str->erase(i, j);
            ++i;
        }
        else /*lower case*/
            (*str)[i] = tolower((*str)[i]);
        ++i;
    }
    /* return string pointer */
    /*DBG(DBG1, " <- Normalize string '%s'", str->c_str());*/
    return str;
}

/*
 * Read and parse config file.
 * Return: 0 on success, -1 on error.
 * Param fname: file name of config file.
 */
int ConfigParser_t::readFile(const string &fname) {

    /* clear possible config content */
    entry.erase(entry.begin(), entry.end());
    /* reserve space for 64 entries withou memory realloc() */
    entry.reserve(64);

    /* stack of open files used for including config files */
    vector<FILE *> fpvec;

    /* get path leading to config file (including term slash) */
    string path;
    unsigned int i;
    i = fname.find_last_of('/');
    if (i >= 0)
        path = fname.substr(0 , i + 1); /*found*/

    /* open config file */
    //DBG(DBG2, "Reading config file '%s'", fname.c_str());
    fpvec.push_back(fopen(fname.c_str(), "rb"));
    if (fpvec.back() == NULL)
    {
        //LOG(WARN2, "Cannot open config file '%s' (%s)",
        //        fname.c_str(), strerror(errno));
        return -1; /*error*/
    }

    /* temporary variables */
    struct entry_s ent;

    /* open quotes (0 == not open) */
    /* this value is usefull for parsing lines like:
     *      option = "value&nbsp;brekeke"
     * in these cases we don't want to treat ';' as comment
     * leading char
     */
    char quoteOpen = 0;

    string buf, lastsect; /*line buffer, actual section name*/
    while (fpvec.size() > 0)
    {
        /* reset buffer and entry struct */
        ent.clear();
        buf.erase();

        /* read one line from file */
        int ch = fgetc(fpvec.back());
        while (ch != EOF && ch != '\n')
        {
            buf += ch;
            ch = fgetc(fpvec.back());
        }
        /* check for end of config file */
        if (ch == EOF)
        {
            /* close file */
            fclose(fpvec.back());
            /* remove closed file from stack */
            fpvec.pop_back();
        }

        /*
         * parse entry. forms:
         * 1)  ...empty line...
         * 2)  ; everything after semicolon (;) or hash (#) is a comment
         * 3)  [Section]   # section name
         * 4)  name = value  ; comment of normal definition
         * 5)  [Section] name = value   # comment ;-)
         * 6)  name = "value;without#'comment"
         */
        /*DBG(DBG1, "Line '%s'", buf.c_str());*/

        /* skip leading spaces */
        i = 0;
        while (i < buf.length() && isspace(buf[i]))
            ++i;
        /* read section */
        if (i < buf.length() && buf[i] == '[')
        {
            /* section name */
            ++i;
            while (i < buf.length() && buf[i] != ']')
                ent.section += buf[i++];
            /* notice actual section */
            normalizeName(&ent.section);
            lastsect = ent.section;

            /* skip ']' */
            if (i < buf.length() && buf[i] == ']')
                ++i;
            /* skip intermediate spaces */
            while (i < buf.length() && isspace(buf[i]))
                ++i;
        }
        else /*section name did not changed*/
            ent.section = lastsect;
        /*DBG(DBG1, " Section '%s'", ent.section.c_str());*/

        /* entry name */
        while (i < buf.length()
                && buf[i] != '='
                && buf[i] != '#'
                && buf[i] != ';')
            ent.name += buf[i++];
        normalizeName(&ent.name);
        /*DBG(DBG1, " Name '%s'", ent.name.c_str());*/

        /* check for included config file */
        if (ent.name.length() >= 9
                && memcmp(ent.name.c_str(), ".include ", 9) == 0)
        {
            /* remove "include" directive */
            ent.name = ent.name.substr(9);
            /* if included file name is relative, add leading path */
            if (ent.name[0] != '/')
                ent.name = path + ent.name; /*rel path*/
            /* open included file */
            //DBG(DBG2, "Including config file '%s'", ent.name.c_str() + 9);
            fpvec.push_back(fopen(ent.name.c_str(), "rb"));
            if (fpvec.back() == NULL)
            {
                //LOG(WARN2, "Cannot open included config file '%s' "
                //        "(%s)", fname.c_str(), strerror(errno));
                return -1; /*error*/
            }
        }
        else
        {
            /* regular content */
            /* equal sign */
            if (i < buf.length() && buf[i] == '=')
            {
                /* skip sign */
                ++i;
                /* read value */
                while (i < buf.length()) {
                    /* are we inside quotes? */
                    if (quoteOpen == 0) {
                        /* no, interpret comments */
                        if (buf[i] == '#' || buf[i] == ';') {
                            break;
                        }

                        if (buf[i] == '\'' || buf[i] == '"') {
                            // opening quote
                            quoteOpen = buf[i];
                        }

                        if (buf[i] == '\\' && i < buf.length()+1 && \
                                (buf[i+1] == '#' || buf[i+1] == ';') ) {
                            i++;
                        }
                    } else if (buf[i] == quoteOpen) {
                        // same quote as previous.
                        // close quote
                        quoteOpen = 0;
                    }

                    ent.value += buf[i++];

                }
                strTrim(&ent.value);
            }
            /*DBG(DBG1, " Value '%s'", ent.value.c_str());*/

            /* if read valid entry */
            if (!ent.name.empty())
            {
                /* insert entry into config object */
                //DBG(DBG1, "Config option [%s] '%s' = '%s'",
                //        ent.section.c_str(), ent.name.c_str(),
                //        ent.value.c_str());
                entry.push_back(ent);
            }
        }
    }
    /* success */
    //DBG(DBG2, "Config file read successfully");
    return 0;
}

/*
 * Get value of given entry name.
 * If entry is not found then value is left untouched
 * (good for pre-initialization with default value).
 * Return: 1=entry found, 0=not found.
 * Param section: section in which is the desired entry
 * Param name: name of the entry
 * Param value: buffer for entry value
 */
int ConfigParser_t::getValue(string section, string name, string *value) const {

    /* prepare variables (names) */
    normalizeName(&section);
    normalizeName(&name);

    unsigned int i = 0;
    while (i < entry.size())
    {
        /* compare section and entry name */
        if (section == entry[i].section && name == entry[i].name)
        {
            /* entry found */
            //DBG(DBG1, "Get config value [%s] '%s' = '%s'",
            //        section.c_str(), name.c_str(), entry[i].value.c_str());
            /* if not null, assign value */
            if (value != NULL)
                *value = entry[i].value;
            return 1; /*found*/
        }
        /* process next */
        ++i;
    }
    /* return empty value (=not found) */
    //DBG(DBG1, "Get config value [%s] '%s' FAILED",
    //        section.c_str(), name.c_str());
    return 0; /*not found*/
}


/*
 * Get value of given entry name (int).
 * If entry is not found or has invalid integer value,
 * then value param is left untouched (good for pre-initialization
 * with defaults).
 * Return: 1=entry found, 0=not found.
 * Param section: section in which is the desired entry
 * Param name: name of the entry
 * Param value: buffer for entry value
 */
int ConfigParser_t::getValue(string section, string name, int *value) const {

    /* get string value */
    string val;
    if (getValue(section, name, &val)) {
        // check for empty value
        if (val.empty()) {
            //DBG(DBG1, "Empty integer value");
            return 0;
        }
        /* try to convert string into int */
        char *endptr;
        int i = strtol(val.c_str(), &endptr, 10);
        /* if entire string is valid */
        if (*endptr == 0) {
            /* supply value */
            if (value != NULL)
                *value = i;
            /* debug */
            //DBG(DBG1, "Integer value %d", i);
            return 1;
        }
    }
    /* return not found/invalid */
    //DBG(DBG1, "Integer value INVALID");
    return 0;
}


/*
 * Get value of given entry name (double).
 * If entry is not found or has invalid double valus,
 * then value param is left untouched (good for pre-initialization
 * with defaults).
 * Return: 1=entry found, 0=not found.
 * Param section: section in which is the desired entry
 * Param name: name of the entry
 * Param value: buffer for entry value
 */
int ConfigParser_t::getValue(string section, string name, double *value) const {

    /* get string value */
    string val;
    if (getValue(section, name, &val)) {
        // check for empty value
        if (val.empty()) {
            //DBG(DBG1, "Empty double value");
            return 0;
        }

        /* try to convert string into int */
        char *endptr;
        double d = strtod(val.c_str(), &endptr);
        /* if entire string is valid */
        if (*endptr == 0) {
            /* supply value */
            if (value != NULL)
                *value = d;
            /* debug */
            //DBG(DBG1, "Double value %f", d);
            return 1;
        }
    }
    /* return not found/invalid */
    //DBG(DBG1, "Double value INVALID");
    return 0;
}


/*
 * Get values of given entry name.
 * Returns all occurences of the same section/name entries.
 * If entry is not found then value is left untouched
 * (good for pre-initialization with default value).
 * Return: 1=entry found, 0=not found.
 * Param section: section in which is the desired entry
 * Param name: name of the entry
 * Param value: vector buffer for entry values
 */
int ConfigParser_t::getValue(
        string section, string name, vector<string> *value) const {

    /* prepare variables (names) */
    normalizeName(&section);
    normalizeName(&name);

    int first = 1;
    int ret = 0;
    unsigned int i = 0;
    while (i < entry.size()) {
        /* compare section and entry name */
        if (section == entry[i].section && name == entry[i].name) {
            /* entry found */
            //DBG(DBG1, "Get config value [%s] '%s' = '%s'",
            //        section.c_str(), name.c_str(), entry[i].value.c_str());
            /* if not null, assign value */
            if (value != NULL) {
                if (first) {
                    value->erase(value->begin(), value->end());
                    first = 0;
                }
                value->push_back(entry[i].value);
                ret = 1;
            }
            ret = 1; /*found*/
        }
        /* process next */
        ++i;
    }
    /* return empty value (=not found) */
   // if (!ret)
    //    DBG(DBG1, "Get config value [%s] '%s' FAILED",
    //            section.c_str(), name.c_str());
    return ret;
}

/** Get value of given entry name (boolean).
 * If entry is not found then value is left untouched
 * (good for pre-initialization with default value).
 * \return 1=entry found, 0=not found.
 * \param section Section in which is the desired entry.
 * \param name Name of the entry.
 * \param value Buffer for entry value.
 */
int ConfigParser_t::getValue(string section, string name, bool *value) const {

    /* get string value */
    string val;
    if (getValue(section, name, &val)) {
        // strip the val from left
        std::string::iterator b = val.begin();
        for (std::string::iterator i = val.begin(); i != val.end();  ++i) {
            if (!isspace(*i)) {
                b = i;
                break;
            }
        }

        // strip the val from right
        std::string::iterator e = val.end();
        for (std::string::iterator i = val.end(); i != val.begin();  ++i) {
            if (!isspace(*(i - 1))) {
                e = i;
                break;
            }
        }
        // do the strip
        val = val.substr(b - val.begin(), e - b);
        // make val lowecase
        std::transform(val.begin(), val.end(), val.begin(), tolower);

        // determine boolean val
        if ((val == "1") || (val == "yes") || (val == "true")
                || (val == "on")) {
            //DBG(DBG1, "Boolean value <true>.");
            *value = true;
            return 1;
        } else if ((val == "0") || (val == "no") || (val == "false")
                || (val == "off")) {
            //DBG(DBG1, "Boolean value <false>.");
            *value = false;
            return 1;
        }
    }
    /* return not found/invalid */
    //DBG(DBG1, "Boolean value INVALID.");
    return 0;
}

std::multimap<std::string, std::string> ConfigParser_t::getOptions(const std::string &section) const
{
    //DBG(DBG1, "Looking for options of section '%s'", section.c_str());
    std::multimap<std::string, std::string> retval;
    for (std::vector<struct entry_s>::const_iterator anEntry = entry.begin(); anEntry != entry.end(); ++anEntry) {
        if (anEntry->section == section) {
            //DBG(DBG1, "Adding option '%s' with value '%s'", anEntry->name.c_str(), anEntry->value.c_str());
            retval.insert(std::make_pair(anEntry->name, anEntry->value));
        }
    }
    return retval;
}

std::vector<std::string> ConfigParser_t::getSections() const
{
    std::set<std::string> sections;
    for (std::vector<struct entry_s>::const_iterator anEntry = entry.begin(); anEntry != entry.end(); ++anEntry) {
        sections.insert(anEntry->section);
    }
    return std::vector<std::string>(sections.begin(), sections.end());
}

