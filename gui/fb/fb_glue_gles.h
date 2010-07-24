// 
//   Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010
//              Free Software Foundation, Inc.
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

#ifndef FB_OGL_GLUE_H
#define FB_OGL_GLUE_H

#ifdef HAVE_CONFIG_H
#include "gnashconfig.h"
#endif

// gles-1.0c for Linux
#ifdef HAVE_GLES_GL_H
# include <GLES/gl.h>
# endif
#ifdef HAVE_GLES_EGL_H
#include <GLES/egl.h>
#endif
// Mali Developer Tools for ARM 1.x
#ifdef HAVE_EGL_EGL_H
# include <EGL/egl.h>
# include <EGL/eglext.h>
#endif
// Mali Developer Tools for ARM 2.x and Android 2.1
#ifdef HAVE_GLES2_GL2_H
# include <GLES2/gl2.h>
# include <GLES2/gl2ext.h>
#endif

#include "ogl_glue.h"
#include "fbsup.h"
#include "render_handler_gles.h"

namespace gnash
{

class FBglesGlue: public FBGlue, public OglGlue
{
public:
    FBglesGlue() :
        _surface (EGL_NO_SURFACE),
        _pbuffer (EGL_NO_SURFACE) {}
    virtual ~FBglesGlue();
    
    virtual bool init(int /*argc*/, char *** /*argv*/);
    
    virtual render_handler* createRenderHandler() {
        //_render_handler = create_render_handler_ogl (true, this);
        return _render_handler;
    }
    
    virtual void setInvalidatedRegions(const InvalidatedRanges& /* ranges */) {}
    
    virtual int width ();
    virtual int height ();
    virtual void render ();
    
    virtual void render_to_pbuffer ();
    virtual void prepare_copy_from_pbuffer ();
    virtual void render_to_display ();
    
private:
    render_handler* _render_handler;
    EGLDisplay      _display;
    EGLConfig       _config;
    EGLContext      _context;
    EGLSurface      _surface;
    EGLConfig       _pbuffer_config;
    EGLSurface      _pbuffer;
};

} // namespace gnash

#endif // FB_OGL_GLUE_H

// Local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
