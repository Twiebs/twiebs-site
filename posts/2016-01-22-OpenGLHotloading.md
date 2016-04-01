%title(Hotloading OpenGL Procedures with C preprocessor metaprograming)
%date(2016,01,22)

One of the important features that I wanted when creating this new iteration of my game engine
Was the ablity to hotload code while the engine is still running.  I wanted this system to be as maximaly flexiable
as possible.  I really wanted to be able to hotload code that contained OpenGL functions.  This is how i accomplished
OpenGL function hotloading in the Venom game engine.

##C Preprocessor Metaprograming

The problem is that when the dll containing the game code is realoaded after compilation functions that
exist in the engine's executable will not be avaible to it.  OpenGL functions are loaded in the engine
using the same system that the game code uses to obtain them.

The opengl procedures are defined in the file opengl_procedures.h
This is a small example of what that file looks like

```

// opengl_procedures.h
...

_(PFNGLGENTEXTURESPROC, glGenTextures)
_(PFNGLTEXSTORAGE2DPROC, glTexStorage2D)
_(PFNGLTEXSTORAGE3DPROC, glTexStorage3D)
_(PFNGLTEXIMAGE2DPROC, glTexImage2D)
_(PFNGLTEXIMAGE3DPROC, glTexImage3D)
_(PFNGLTEXSUBIMAGE2DPROC, glTexSubImage2D)
_(PFNGLTEXSUBIMAGE3DPROC, glTexSubImage3D)

...


```

Using the C preprocessor allows the defintions of the OpenGL functions to be pasted in locations where
they are realevant

```

struct EngineAPI
{
#define _(sig, name) sig name;
#include "opengl_procedures.h"
#undef _
};


```

One very important facet of this system is that there is no differentation between how the
OpenGL procedures are called in debug and release builds.  This is acomplished by pasting the OpenGL
procedure list as global variables in the game module

```

//The Games Translation Unit
#define _(sig, name) static sig name;
#include "opengl_procedures.h"
#undef _

extern "C"
void on_module_load(EngineAPI *api)
{
#define _(sig,name) name = api->##name;
#include "opengl_procedures.h"
#undef _
}

```

With this small amount of code we are able to set the function pointers of our global OpenGL functions
within the game module to the procedures that have been loaded from the platform layers coresponding dynamic
OpenGL library.
