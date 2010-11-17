//
//   Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010 Free Software
//   Foundation, Inc
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

#ifndef __GNASH_DEVICE_H__
#define __GNASH_DEVICE_H__ 1

#ifdef HAVE_CONFIG_H
#include "gnashconfig.h"
#endif

#include <boost/scoped_array.hpp>

namespace gnash
{

namespace renderer {

struct GnashDevice
{
    // Handle multiple window types. The derived classes will cast this to
    // the proper data type.
    typedef void * native_window_t;
    
    // the list of supported renders that use devices
    typedef enum {OPENVG, OPENGL, OPENGLES1, OPENGLES2, X11, VAAPI} rtype_t;

    GnashDevice() { GNASH_REPORT_FUNCTION; };
    virtual ~GnashDevice() { GNASH_REPORT_FUNCTION; };

    // Initialize GNASH Device layer
    virtual bool initDevice(int argc, char *argv[]) = 0;

    // Initialize Gnash Window on device
    virtual bool attachWindow(native_window_t window) = 0;
    
    // Utility methods not in the base class
    /// Return a string with the error code as text, instead of a numeric value
    virtual const char *getErrorString(int error) = 0;
    
    // /// Query the system for all supported configs
    // int queryGNASHConfig() { return queryGNASHConfig(_gnashDisplay); };
    // int queryGNASHConfig(GNASHDisplay display);

    virtual size_t getWidth() = 0;
    virtual size_t getHeight() = 0;
    virtual int getDepth() = 0;
    
    virtual int getRedSize() = 0;
    virtual int getGreenSize() = 0;
    virtual int getBlueSize() = 0;
    
    virtual bool isSingleBuffered() = 0;
    virtual bool isBufferDestroyed() = 0;
    
    virtual int getID() = 0;
    
    virtual bool supportsRenderer(rtype_t rtype) = 0;
    
    virtual bool isNativeRender() = 0;
};

} // namespace renderer
} // namespace gnash

#endif  // end of __GNASH_DEVICE_H__

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End: