// rc.cpp:  "Run Command" configuration file, for Gnash.
// 
//   Copyright (C) 2005, 2006, 2007 Free Software Foundation, Inc.
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

// This is generated by autoconf
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "StringPredicates.h"
#include "log.h"
#include "rc.h"

#ifdef HAVE_PWD_H
# include <pwd.h>
#endif

#include <boost/cstdint.hpp>
#include <sys/types.h>
#include <unistd.h> // for getuid()
#include <sys/stat.h>
#include <cerrno>
#include <limits.h>

#include <cctype>  // for toupper
#include <string>
#include <vector>
#include <iostream>
#include <fstream>


#ifndef DEFAULT_STREAMS_TIMEOUT
// TODO: add a ./configure switch to set this
# define DEFAULT_STREAMS_TIMEOUT 10
#endif

using namespace std;
namespace gnash {

RcInitFile&
RcInitFile::getDefaultInstance()
{
	// TODO: allocate on the heap and provide a destroyDefaultInstance,
	//       for finer control of destruction order
	static RcInitFile rcfile;
	return rcfile;
}


RcInitFile::RcInitFile() : _delay(0),
			   _movieLibraryLimit(8),
                           _debug(false),
                           _debugger(false),
                           _verbosity(-1),
                           // will be reset to something else if __OS2__x is defined
                           _urlOpenerFormat("firefox -remote 'openurl(%u)'"),
                           _flashVersionString(
				DEFAULT_FLASH_PLATFORM_ID" "\
				DEFAULT_FLASH_MAJOR_VERSION","\
				DEFAULT_FLASH_MINOR_VERSION","\
				DEFAULT_FLASH_REV_NUMBER ",0"),
                           _flashSystemOS(), 
                           _flashSystemManufacturer("Gnash "DEFAULT_FLASH_SYSTEM_OS),
                           _actionDump(false),
                           _parserDump(false),
			   _verboseASCodingErrors(false),
			   _verboseMalformedSWF(false),
                           _splashScreen(true),
                           _localdomainOnly(false),
                           _localhostOnly(false),
                           _log("gnash-dbg.log"),
			   _writeLog(false),
                           _sound(true),
                           _pluginSound(true),
			   _extensionsEnabled(false),
			   _startStopped(false),
			   _insecureSSL(false),
			   _streamsTimeout(DEFAULT_STREAMS_TIMEOUT),
                           _solreadonly(false),
                           _sollocaldomain(false),
                           _lcdisabled(false),
                           _lctrace(true)

{
//    GNASH_REPORT_FUNCTION;
    loadFiles();
#ifdef __OS2__x
    _urlOpenerFormat = PrfQueryProfileString( HINI_USER, (PSZ) "WPURLDEFAULTSETTINGS",
			(PSZ) "DefaultBrowserExe", NULL,
			(PVOID) browserExe, (LONG)sizeof(browserExe) );
	_urlOPenerFormat += " -remote 'openurl(%u)'";
#endif

    // TODO: fetch GNASH_URLOPENER_FORMAT from the environment
}

RcInitFile::~RcInitFile()
{
//    GNASH_REPORT_FUNCTION;    
}

// Look for a config file in the likely places.
bool
RcInitFile::loadFiles()
{
//    GNASH_REPORT_FUNCTION;
    
    // Check the default system location
    string loadfile = "/etc/gnashrc";
    parseFile(loadfile);
    
    // Check the default config location
    loadfile = "/usr/local/etc/gnashrc";
    parseFile(loadfile);
    
    // Check the users home directory
    char *home = getenv("HOME");
    if (home) {
        loadfile = home;
        loadfile += "/.gnashrc";
        parseFile(loadfile);
    }

    // Check the GNASHRC environment variable
    char *gnashrc = getenv("GNASHRC");
    if (gnashrc) {
        loadfile = gnashrc;
        return parseFile(loadfile);
    }
    
    return false;
}

bool
RcInitFile::extractSetting(bool *var, const char *pattern,
                           std::string &variable, std::string &value)
{
//    GNASH_REPORT_FUNCTION;
    //log_msg ("%s: %s", variable, value);
    
	StringNoCaseEqual noCaseCompare;
    if ( noCaseCompare(variable, pattern) ) {
        if ( noCaseCompare(value, "on") || noCaseCompare(value, "yes") ||
             noCaseCompare(value, "true")) {
            //log_msg ("%s: Enabled", variable);
            *var = true;
        }
        if (noCaseCompare(value, "off") || noCaseCompare(value, "no") ||
            noCaseCompare(value, "false")) {
            //log_msg ("%s: Disabled", variable);
            *var = false;
        }
    }
    return *var;
}

uint32_t
RcInitFile::extractNumber(uint32_t *num, const char *pattern, string &variable,
                           string &value)
{      
//    GNASH_REPORT_FUNCTION;

    StringNoCaseEqual noCaseCompare;

//        log_msg ("%s: %s", variable.c_str(), value.c_str());
    if ( noCaseCompare(variable, pattern) ) {
        *num = strtoul(value.c_str(), NULL, 0);
        if (*num == LONG_MAX) {
            long long foo = strtoll(value.c_str(), NULL, 0);
            log_error("Conversion overflow!: %lld", foo);
            
        }
    }
    return *num;
}

/// Takes references to action ('set' or 'append'), items
/// (list of items separated by spaces), listname (name of list)
/// and the item array (list).
//
/// Returns either empty array ('set <list> off'), array with only
/// passed items ('set <list> <items>') or array passed with items
/// appended ('append <list> <items>').

void
RcInitFile::parseList(std::vector<std::string> &list, string &action,
			    std::string &listname, string &items)
{
//    GNASH_REPORT_FUNCTION;

    if (action == "set") {

	// Clear array of hosts in previously parsed
	// rc files.
 	list.clear();

	StringNoCaseEqual noCaseCompare;

        if (noCaseCompare(items, "off") || noCaseCompare(items, "no") ||
            noCaseCompare(items, "false")) {
	    // Return empty array (allows disabling of global
	    // whitelists in favour of a blacklist)
	    return;
	}
    }		

    string::size_type pos;

    // This is an ugly way to avoid breaking lists
    // Lists will work if they worked before, but
    // combining the two separators will not.
    // The colon way must be removed before protocols
    // (http://, https:// can be included in lists).
    char separator;
    if (items.find(':') != string::npos) {
	// Deprecated behaviour
	separator = ':';
	fprintf(stderr, _("The list '%s' in an rcfile contains a colon. This is deprecated and may result in "
		"unexpected behaviour. Please only use spaces as a separator."), listname.c_str());
    } else {
	// New behaviour
	separator = ' ';
    }

    while (items.size()) {
	pos = items.find(separator, 0);
    	list.push_back(items.substr(0, pos));
    	items.erase(0, pos);
    	if (items.size()) items.erase(0, items.find_first_not_of(separator)); 
    }

}

void
RcInitFile::extractDouble(double& out, const char *pattern, string &variable,
                           string &value)
{
//    GNASH_REPORT_FUNCTION;

    StringNoCaseEqual noCaseCompare;

    // printf("%s: %s\n", variable.c_str(), value.c_str());

    if ( noCaseCompare(variable, pattern) ) {
        out = strtod(value.c_str(), 0);
	//printf("strtod returned %g\n", out);
    }
}

void
RcInitFile::expandPath (std::string& path)

{

// Leaves path unchanged on systems without
// POSIX tilde expansion.

#ifdef HAVE_PWD_H
//Don't build tilde expansion on systems without pwd.h

              //Only if path starts with "~"
             if (path.substr(0,1) == "~") {
             const char *home = getenv("HOME");
                     if (path.substr(1,1) == "/") {
                          // Initial "~" followed by "/"
                          if (home) {
                               // if HOME set in env, replace ~ with HOME
                               path = path.replace(0,1,home);
                          }

# ifdef HAVE_GETPWNAM
//Don't try this on systems without getpwnam

                          //HOME not set in env: try using pwd

                          else { 
                               struct passwd *password = getpwuid(getuid());
                               const char *pwdhome = password->pw_dir;
                               if (home) {
                                   path = path.replace(0,1,pwdhome);
                               }
                               //If all that fails, leave path alone
                          }
                     }

                     //Initial "~" is not followed by "/"
                     else {
                          const char *userhome = NULL;
                          string::size_type first_slash =
                              path.find_first_of("/");
                          string user;
                          if (first_slash != string::npos) {
                              // everything between initial ~ and / 
                              user = path.substr(1, first_slash - 1 );
                          } else user = path.substr(1);

                          //find user using pwd    
                          struct passwd *password = getpwnam(user.c_str());
                          if (password) {
                              //User found
                              userhome = password->pw_dir;
                          }
                          if (userhome) {
                               string foundhome(userhome);
                               path = path.replace(0,first_slash,foundhome);
                          }
# endif
                      }
                 }
#endif

}

// Parse the config file and set the variables.
bool
RcInitFile::parseFile(const std::string& filespec)
{
//    GNASH_REPORT_FUNCTION;
    struct stat stats;
    string action;
    string variable;
    string value;
    ifstream in;

	StringNoCaseEqual noCaseCompare;
    
//  log_msg ("Seeing if %s exists", filespec);
    if (filespec.size() == 0) {
        return false;
    }
    
    if (stat(filespec.c_str(), &stats) == 0) {
        in.open(filespec.c_str());
        
        if (!in) {
            log_error(_("Couldn't open file: %s"), filespec.c_str());
            return false;
        }
        
        // Read in each line and parse it
        do {

	    // Make sure action is empty, otherwise the last loop (with no new
	    // data) keeps action, variable and value from the previous loop. This
	    // causes problems if set blacklist or set whitelist are last, because
	    // value is erased while parsing and the lists are thus deleted.
	    action.clear();

            // Get the first token
            in >> action;

            // Ignore comment lines
            if (action[0] == '#') {
                // suck up the rest of the line
                char name[128];
                in.getline(name, 128);
                continue;
            } 
            
	    // Get second token
            in >> variable;

            // cout << "Parsing " << variable << endl;

	    // Read in rest of line for parsing.
            getline(in, value);

	    // Erase leading spaces.
            string::size_type position = value.find_first_not_of(' ');
            if(position != string::npos) value.erase(0, position);

            if (noCaseCompare(action, "set") || noCaseCompare(action, "append") ) {

                if (noCaseCompare(variable, "urlOpenerFormat")) {
                    _urlOpenerFormat = value;
                    continue;
                }

                if (noCaseCompare(variable, "flashVersionString")) {
                    _flashVersionString = value;
                    continue;
                }
                
                if (noCaseCompare(variable, "flashSystemOS")) {
                    _flashSystemOS = value;
                    continue;
                }

                if (noCaseCompare(variable, "flashSystemManufacturer")) {
                    _flashSystemManufacturer = value;
                    continue;
                }

                if (noCaseCompare(variable, "debuglog")) {
                    expandPath (value);
                    _log = value;
                    continue;
                }

                if (noCaseCompare(variable, "documentroot") ) {
                    _wwwroot = value;
                    continue;
                }
                
                if (noCaseCompare(variable, "blacklist") ) {
                    parseList(_blacklist, action, variable, value);
                    continue;
                }

                if (noCaseCompare(variable, "whitelist")) {
                    parseList(_whitelist, action, variable, value);
                    continue;
                }

                if (noCaseCompare(variable, "localSandboxPath")) {
                    parseList(_localSandboxPath, action, variable, value);
                    continue;
                }

                if (noCaseCompare(variable, "SOLSafeDir")) {
                    expandPath (value);
                    _solsandbox = value;
                    continue;
                }

		if (noCaseCompare(action , "set") ) {
                     extractSetting(&_splashScreen, "splash_screen", variable,
                               value);
                     extractSetting(&_localhostOnly, "localhost", variable,
                               value);
                     extractSetting(&_localdomainOnly, "localdomain", variable,
                               value);
                     extractSetting(&_insecureSSL, "InsecureSSL", variable,
                               value);
                     extractSetting(&_debugger, "debugger", variable, value);
                     extractSetting(&_actionDump, "actionDump", variable, value);
                     extractSetting(&_parserDump, "parserDump", variable, value);
                     extractSetting(&_writeLog, "writelog", variable, value);
                     extractSetting(&_sound, "sound", variable, value);
                     extractSetting(&_pluginSound, "pluginsound", variable, value);
                     extractSetting(&_verboseASCodingErrors,
                               "ASCodingErrorsVerbosity", variable, value);
                     extractSetting(&_verboseMalformedSWF, "MalformedSWFVerbosity",
                               variable, value);
                     extractSetting(&_extensionsEnabled, "EnableExtensions",
                               variable, value);
                     extractSetting(&_startStopped, "StartStopped", variable, value);

                     extractDouble(_streamsTimeout, "StreamsTimeout", variable, value);

                     extractNumber(&_movieLibraryLimit, "movieLibraryLimit", variable, value);                
                     extractNumber(&_delay, "delay", variable, value);
                     extractNumber(&_verbosity, "verbosity", variable, value);


                     extractSetting(&_solreadonly, "SOLReadOnly", variable,
                               value);
                     extractSetting(&_lcdisabled, "LocalConnection", variable,
                               value);
                     extractSetting(&_lctrace, "LCTrace", variable,
                               value);
                     extractNumber((uint32_t *)&_lcshmkey, "LCShmkey", variable, value);
		}
            }
        } while (!in.eof());

    } else {
        if (in) {
            in.close();
        }
        return false;
    }  
    
    if (in) {
        in.close();
    }

    return true;
}

// Write the changed settings to the config file
bool
RcInitFile::updateFile(const std::string& /* filespec */)
{
    cerr << __PRETTY_FUNCTION__ << "ERROR: unimplemented!" << endl;
    return false;
}

void
RcInitFile::useSplashScreen(bool value)
{
    _splashScreen = value;
}

void
RcInitFile::useLocalDomain(bool value)
{
    _localdomainOnly = value;
}

void
RcInitFile::useLocalHost(bool value)
{
    _localhostOnly = value;
}

void
RcInitFile::useActionDump(bool value)
{
//    GNASH_REPORT_FUNCTION;
    _actionDump = value;
    if (value) {
        _verbosity++;
    }
}

void
RcInitFile::showASCodingErrors(bool value)
{
//    GNASH_REPORT_FUNCTION;
    
    _verboseASCodingErrors = value;
    if (value) {
        _verbosity++;
    }
}

void
RcInitFile::useParserDump(bool value)
{
//    GNASH_REPORT_FUNCTION;
    
    _parserDump = value;
    if (value) {
        _verbosity++;
    }
}

void
RcInitFile::useWriteLog(bool value)
{
//    GNASH_REPORT_FUNCTION;
    
    _writeLog = value;
}

void
RcInitFile::dump()
{
    cerr << endl << "Dump RcInitFile:" << endl;
    cerr << "\tTimer interupt delay value: " << _delay << endl;
    cerr << "\tFlash debugger: "
         << ((_debugger)?"enabled":"disabled") << endl;
    cerr << "\tVerbosity Level: " << _verbosity << endl;
    cerr << "\tDump ActionScript processing: "
         << ((_actionDump)?"enabled":"disabled") << endl;
    cerr << "\tDump parser info: "
         << ((_parserDump)?"enabled":"disabled") << endl;
    cerr << "\tActionScript coding errors verbosity: "
         << ((_verboseASCodingErrors)?"enabled":"disabled") << endl;
    cerr << "\tMalformed SWF verbosity: "
         << ((_verboseASCodingErrors)?"enabled":"disabled") << endl;
    cerr << "\tUse Splash Screen: "
         << ((_splashScreen)?"enabled":"disabled") << endl;
    cerr << "\tUse Local Domain Only: "
         << ((_localdomainOnly)?"enabled":"disabled") << endl;
    cerr << "\tUse Localhost Only: "
         << ((_localhostOnly)?"enabled":"disabled") << endl;
    cerr << "\tWrite Debug Log To Disk: "
         << ((_writeLog)?"enabled":"disabled") << endl;
    cerr << "\tAllow insecure SSL connections: "
         << ((_insecureSSL)?"yes":"no") << endl;
    cerr << "\tEnable sound: "
         << ((_sound)?"enabled":"disabled") << endl;
    cerr << "\tEnable Plugin sound: "
         << ((_pluginSound)?"enabled":"disabled") << endl;
    cerr << "\tEnable Extensions: "
         << ((_extensionsEnabled)?"enabled":"disabled") << endl;
    if (_log.size()) {
        cerr << "\tDebug Log name is: " << _log << endl;
    }
    if (_flashVersionString.size()) {
        cerr << "\tFlash Version String is: " << _flashVersionString << endl;
    }
    
//     std::vector<std::string> _whitelist;
//     std::vector<std::string> _blacklist;
}


} // end of namespace gnash
