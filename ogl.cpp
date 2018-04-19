#include "ogl.h"
/*
 * Pass in a shader or program object and print its log
 * to stderr
 */
void print_log(GLuint object)
{
   GLint log_length = 0;

   // check to see if the arg object is a program or shader
   if(glIsShader(object))
   {
      glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
   }
   else if(glIsProgram(object))
   {
      glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
   }
   else
   {
      cerr << "print_log: Not a shader or a program" << endl;
   }

   char* log = new char[log_length];
   
   if(glIsShader(object))
   {
      glGetShaderInfoLog(object, log_length, NULL, log);
   }
   else if(glIsProgram(object))
   {
      cerr << "print_log: object is a program" << endl;
      glGetProgramInfoLog(object, log_length, NULL, log);
   }

   cerr << log << endl;
   delete[] log;
}

/*
 * Make a shader from a file and return a GLint
 * with a valid compiled shader object
 */
GLuint make_shader(const char* fileName, GLenum type)
{
   
   string text;
   int length;
   GLuint shaderObj; 

   if(file_reader( fileName, text, length))
   {
      const GLchar* source[] = {text.c_str()}; // have to wrap the values in arrays
      GLint lengths[] = {(GLint)text.length()};         // for glShaderSource

      shaderObj= glCreateShader(type);
      glShaderSource(shaderObj, 1, source, lengths);
      
      glCompileShader(shaderObj);
      GLint successFlag;
      glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &successFlag);
      cerr << "make_shader: " << fileName << " GL_COMPILE_STATUS " << successFlag << endl;
      if(!successFlag)
      {

         cerr << "make_shader: shader did not compile " << fileName << endl;
         print_log(shaderObj);
      }
      return shaderObj;
   }
   else
   {
      cerr << "make_shader: didn't make a shader file read was bad" << endl;
   }
}

/* make a program by linking together shaders
 * takes an array of strings with GLenum types
 * and makes a shader for each
 */
GLuint make_program( int count, string fileNames[], GLenum type[])
{
   GLint successFlag = GL_FALSE;

   GLuint program = glCreateProgram();

   for(int i = 0; i < count; i++)
   {
      const GLchar* tempFileName = {fileNames[i].c_str()};
      GLuint shader = make_shader(tempFileName, type[i]);
      glAttachShader(program, shader); 
   }
   glLinkProgram(program);
   print_log(program);
   get_error();
   cerr << "------------end printlog-------------" << endl;

   GLint linkOk;
   glGetProgramiv(program,GL_ATTACHED_SHADERS, &linkOk);
   cerr << "make_program: GL_ATTACHED_SHADERS " << linkOk << endl;
   glGetProgramiv(program,GL_LINK_STATUS, &linkOk);
   cerr << "make_program: GL_LINK_STATUS " << linkOk << endl;
   return program;
}

GLint get_attribute(GLuint program, const char *name)
{
   GLint attribute = glGetAttribLocation(program, name);
   if(attribute == -1)
   {
      cerr << "get_attribute: could not find attribute " << name;
   }
   return attribute;
}

void get_error(void)
{
   GLenum error = glGetError();

   if( error == GL_NO_ERROR )
      cerr <<  "getError: GL_NO_ERROR" << endl;
   if( error == GL_INVALID_ENUM )
      cerr <<  "getError: GL_INVALID_ENUM" << endl;
   if( error == GL_INVALID_OPERATION )
      cerr <<  "getError: GL_INVALID_OPERATION" << endl;
   if( error == GL_INVALID_VALUE )
      cerr <<  "getError: GL_INVALID_VALUE" << endl;
   cerr << "getError value = " << error << endl;
}

/************************Texture Stuff **************************************/
bool cvLoad_image( string fileName, Mat dataContainer)
{
   dataContainer =imread(fileName, 1);
}

bool make_texture(GLuint& textureObj, ImageData dataContainer)
{
   bool success = false;

   glGenTextures( 1, &textureObj );
   glBindTexture( GL_TEXTURE_2D,  textureObj );
   
   glTexStorage2D(GL_TEXTURE_2D, 4, GL_RGB32F, 512, 512);

   glTexSubImage2D(GL_TEXTURE_2D,
                   0,
                   0, 0,
                   512, 
                   512,
                   GL_RGBA, GL_UNSIGNED_BYTE,
                   dataContainer.data);

   //GLint swizzles[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
   //glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzles);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   //glTexImage2D( GL_TEXTURE_2D,        /* 2D texture target */
   //              0,                    /* mipmap level CHECK THIS */
   //              GL_RGBA,              /* internal format */
   //              dataContainer.width,  /* width */
   //              dataContainer.height,  /* height */
   //              0,                     /* border */
   //              GL_RGBA,               /* external format */
   //              GL_UNSIGNED_BYTE,      /* type */
   //              dataContainer.data);   /* pointer to the pixels */

   return success;
}

bool cvMake_texture(GLuint& textureObj, Mat dataContainer, GLuint& texBuffer)
{
   bool success = false;

   glGenBuffers( 1, &texBuffer ); 
   glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texBuffer);

   glGenTextures( 1, &textureObj );
   glBindTexture( GL_TEXTURE_2D,  textureObj );
   
//   //use fast 4-byte alignemtnt (default anyway) if possible
//   glPixelStorei(GL_UNPACK_ALIGNMENT, (dataContainer.step & 3) ? 1 : 4);
//   cout << "____________________________________________________________" << endl;
//   cout << "(dataContainer.step & 3) ? 1 : 4" << ((dataContainer.step & 3) ? 1 : 4) << endl;
//
//   //set length of one complete row in data (doesnt need to equal image.cols)
//   glPixelStorei(GL_UNPACK_ROW_LENGTH, dataContainer.step/dataContainer.elemSize());
//   cout << "dataContainer.step/dataContainer.elemSize()" << dataContainer.step/dataContainer.elemSize() << endl;
//   cout << "____________________________________________________________" << endl;

   


//   glTexImage2D( GL_TEXTURE_2D,        /* 2D texture target */
//                 0,                    /* mipmap level CHECK THIS */
//                 GL_RGBA,              /* internal format */
//                 dataContainer.rows,  /* width */
//                 dataContainer.cols,  /* height */
//                 0,                     /* border */
//                 GL_BGR,               /* external format */
//                 GL_UNSIGNED_BYTE,      /* type */
////                 dataContainer.data);   /* pointer to the pixels */
//                 NULL);
   glBufferData( GL_PIXEL_UNPACK_BUFFER,
//                 dataContainer.step[0] * dataContainer.step[1],
                 dataContainer.rows * dataContainer.cols * 3,
     //            sizeof(dataContainer.data),
                 dataContainer.data,
                 GL_STREAM_DRAW); // need to change this to GL_STREAM_DRAW

   glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, dataContainer.cols, dataContainer.rows);

   glTexSubImage2D(GL_TEXTURE_2D,
                   0,      //mip level
                   0, 0,   // offset
                   dataContainer.cols, 
                   dataContainer.rows,
                   GL_BGR, 
                   GL_UNSIGNED_BYTE,
                   NULL);
//                   dataContainer.data);

   get_error();
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   return success;
}

void cv_update_texture( Mat dataContainer, GLuint texBuffer)
{

//   glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texBuffer);
   glBufferData( GL_PIXEL_UNPACK_BUFFER,
                 dataContainer.rows * dataContainer.cols * 3,
                 dataContainer.data,
                 GL_STREAM_DRAW); // need to change this to GL_STREAM_DRAW
   glTexSubImage2D(GL_TEXTURE_2D,
                   0,      //mip level
                   0, 0,   // offset
                   dataContainer.cols,
                   dataContainer.rows, 
                   GL_BGR, 
                   GL_UNSIGNED_BYTE,
                   NULL);
//   get_error();
}
