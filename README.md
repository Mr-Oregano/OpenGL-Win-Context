# OpenGL-Win-Context
OpenGL context creation is platform dependent. On Windows you use `wglCreateContext`/`wglMakeCurrent` to create a 'dummy' context. In order to request CORE context and specific version, the dummy context is used to load `wglChoosePixelFormatARB`/`wglCreateContextAttribsARB` functions which are then used to create the *real* context.

Window events and management is handled with WinAPI.

### References

 - https://www.khronos.org/opengl/wiki/Creating_an_OpenGL_Context_(WGL)
