//
// two error checkers
//
// for this subroutine:
// 
// void drawshape()
// {
//   glBegin (GL_FRONT_AND_BACK);
//	 glVertex2i (0,0);
//	 glVertex2i(0,1);
//	 glEnd();
// }
//
// Use 
//    CHECK_OPENGL_ERROR ( drawshape() );
//
// or
//    drawshape();
//    CHECK_ERROR ("after drawshape");
//
#ifndef __gluit_h
#define CHECK_OPENGL_ERROR( cmd ) { \
   cmd; \
   { GLenum  error; \
      while ( error = glGetError()) \
	  printf( "[%s:%d] '%s' failed with error %s\n", __FILE__, __LINE__, #cmd, gluErrorString(error) ); } \
   }
#endif
#define CHECK_ERROR(str)                                           \
{                                                                  \
    GLenum error;                                                  \
    if(error = glGetError())                                       \
       printf("GL Error: %s (%s)\n", gluErrorString(error), str);  \
}

// textfile.h: interface for reading and writing text files
// www.lighthouse3d.com
//
// You may use these functions freely.
// they are provided as is, and no warranties, either implicit,
// or explicit are given
//////////////////////////////////////////////////////////////////////
//
// also report syntax error of both shaders
//
extern GLuint setVFGShaders (char*, char*, char*);


