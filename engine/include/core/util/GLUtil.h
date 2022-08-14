#ifndef ENGINE2_0_GLUTIL_H
#define ENGINE2_0_GLUTIL_H

#ifdef NDEBUG
#define CHECK_GL_ERROR(extraInfo)
#else
#define CHECK_GL_ERROR(extraInfo)                           \
        {                                                   \
            GLenum err;                                     \
            while((err = glGetError()) != GL_NO_ERROR){     \
                LOG_E("GL_ERROR ", extraInfo," -> ", err)   \
            }                                               \
        }
#endif
#endif