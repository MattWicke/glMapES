#include "surface.h"

void check(std::string functionName)
{
	GLenum err = glGetError();
	if(err != GL_NO_ERROR)
		std::cout << "************* GL ERROR: " << err 
			<< functionName << std::endl;
}
Surface::Surface(std::vector<string> _plist, GLfloat* _vertexData, GLuint _program, int _handleRad):
    plist(_plist),
    program(_program),
    handleRad(_handleRad),
    activeHandleIndex(numHandles + 1),
    frameIndex(0),
    plistIndex(0)
{
    vidCap.open(plist[0].c_str());
    if(!vidCap.isOpened())
        std::cerr << "ERROR: failed to open " << plist[0] << std::endl;

//** code for caching all video frames in memory
//  while(vidCap.get(CV_CAP_PROP_POS_FRAMES) < vidCap.get(CV_CAP_PROP_FRAME_COUNT) - 1)
//  {
//      Mat tempFrame;
//      vidCap >> tempFrame;
//      frameVec.push_back(tempFrame);
//  }

    //for(int ii; ii <12; ii++)
        vidCap >> frame;


    glGenVertexArrays(1, &vertexArrayObj);
    glGenBuffers(1, &vboVert);

    for(int i = 0; i < 8; i++)
    {
        vertexData[i] = _vertexData[i];
        cout << vertexData[i] << " ";
    }

    glBindVertexArray(vertexArrayObj);
    glBindBuffer(GL_ARRAY_BUFFER, vboVert);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8,vertexData, GL_STATIC_DRAW);

    GLfloat texCoordData[] =
    {
        1.0f,   1.0f,
        0.0f,   1.0f,
        0.0f,   0.0f,
        1.0f,   0.0f,
    };

    glGenVertexArrays( 1, &vboTexcoords ) ;
    glGenBuffers(1, &vboTexcoords);
    glBindBuffer(GL_ARRAY_BUFFER, vboTexcoords);
    glBufferData(GL_ARRAY_BUFFER
		    , sizeof(texCoordData)
		    , texCoordData
		    , GL_STREAM_DRAW);

    glGenBuffers( 1, &texBuffer ); 
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texBuffer);

    glGenTextures( 1, &textureObj );
    glBindTexture( GL_TEXTURE_2D,  textureObj );
    glPixelStorei(GL_UNPACK_ROW_LENGTH, frame.step/frame.elemSize());
    glBufferData( GL_PIXEL_UNPACK_BUFFER,
              frame.rows * frame.cols * 3,
              frame.data,
              GL_STREAM_DRAW); // need to change this to GL_STREAM_DRAW

    //glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, frame.cols, frame.rows);

    glTexImage2D(
		    GL_TEXTURE_2D
		    , 0 //LEVEL
		    , 4 //base,
		    , frame.cols
		    , frame.rows
		    , 0
		    , GL_BGR
		    , GL_UNSIGNED_BYTE
		    ,NULL);


    glTexSubImage2D(GL_TEXTURE_2D,
                0,      //mip level
                0, 0,   // offset
                frame.cols, 
                frame.rows,
                GL_BGR, 
                GL_UNSIGNED_BYTE,
                NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    surfaceID = surfaceCount;

    surfaceCount++;
}

void Surface::draw()
{
	glUseProgram(program);
    GLint intexLocation = glGetAttribLocation(program, "in_tex_coord");
    if(intexLocation == -1)
	    std::cout << "could not bind intex" << std::endl;
    GLint tex_uniform = glGetUniformLocation(program, "tex");
    if(tex_uniform == -1)
	    std::cout << "could not bind tex" << std::endl;
    GLint inposLocation = glGetAttribLocation(program, "in_pos");
    if(inposLocation == -1)
	    std::cout << "could not bind inposeLocation" << std::endl;

    glUniform1i(tex_uniform, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vboTexcoords);
    glBindTexture( GL_TEXTURE_2D,  textureObj );
    glVertexAttribPointer(intexLocation, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(intexLocation);

    glActiveTexture(GL_TEXTURE0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texBuffer);


    //glBindBuffer(GL_ARRAY_BUFFER, vboVert);
    glEnableVertexAttribArray(inposLocation);
    glBindBuffer(GL_ARRAY_BUFFER, vboVert);
    glVertexAttribPointer(inposLocation, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  //int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
  //glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
}

void Surface::update()
{
    GLfloat texCoordData[] =
    {
        1.0f,   1.0f,
        0.0f,   1.0f,
        0.0f,   0.0f,
        1.0f,   0.0f,
    };
    if(vidCap.get(CV_CAP_PROP_POS_FRAMES) > vidCap.get(CV_CAP_PROP_FRAME_COUNT) - 1)
    {
        repeatCount = 0;
        vidCap.release();
        if(plistIndex < plist.size() -1)
            plistIndex++;
        else
            plistIndex = 0;
        cout << "Opening: " << plist[plistIndex].c_str() << endl;
        vidCap.open(plist[plistIndex].c_str());
        if(!vidCap.isOpened())
            cout << "*************Furck******************************" << endl;
        vidCap >> frame;

        glDeleteBuffers(1, &vboTexcoords);
        glGenBuffers(1, &vboTexcoords);
        glBindBuffer(GL_ARRAY_BUFFER, vboTexcoords);
        glBufferData(GL_ARRAY_BUFFER
			, sizeof(texCoordData)
			, texCoordData
			, GL_STREAM_DRAW);

        glDeleteBuffers(1, &texBuffer);
        glGenBuffers( 1, &texBuffer ); 
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texBuffer);

        glDeleteTextures( 1, &textureObj );
        glGenTextures( 1, &textureObj );
        glBindTexture( GL_TEXTURE_2D,  textureObj );
        glPixelStorei(GL_UNPACK_ROW_LENGTH, frame.step/frame.elemSize());
        glBufferData( GL_PIXEL_UNPACK_BUFFER,
                      frame.rows * frame.cols * 3,
                      frame.data,
                      GL_STREAM_DRAW); // need to change this to GL_STREAM_DRAW

        //glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, frame.cols, frame.rows);

	    glTexImage2D(
		    GL_TEXTURE_2D
		    , 0 //LEVEL
		    , 4 //base,
		    , frame.cols
		    , frame.rows
		    , 0
		    , GL_BGR
		    , GL_UNSIGNED_BYTE
		    ,NULL);
        glTexSubImage2D(GL_TEXTURE_2D,
                        0,      //mip level
                        0, 0,   // offset
                        frame.cols, 
                        frame.rows,
                        GL_BGR, 
                            GL_UNSIGNED_BYTE,
                        NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    vidCap >> frame;

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texBuffer);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, frame.step/frame.elemSize());
    glBufferData( GL_PIXEL_UNPACK_BUFFER,
                  frame.rows * frame.cols * 3,
                  frame.data,
                  GL_STREAM_DRAW); // need to change this to GL_STREAM_DRAW

    std::chrono::time_point<std::chrono::system_clock> start 
	    = std::chrono::system_clock::now();

    glBindTexture( GL_TEXTURE_2D,  textureObj );
    glTexSubImage2D(GL_TEXTURE_2D,
                    0,      //mip level
                    0, 0,   // offset
                    frame.cols,
                    frame.rows, 
                    GL_BGR, 
                    GL_UNSIGNED_BYTE,
                    NULL);
                    //NULL);

    std::chrono::time_point<std::chrono::system_clock> now 
	    = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_time = now - start;
   std::cout <<  "update" << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time).count() << std::endl;
 //** code for caching all video frames in memory rather than on disk
//   glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texBuffer);
//   glBufferData( GL_PIXEL_UNPACK_BUFFER,
//                 frameVec[frameIndex].rows * frameVec[frameIndex].cols * 3,
//                 frameVec[frameIndex].data,
//                 GL_STREAM_DRAW); // need to change this to GL_STREAM_DRAW

//   glBindTexture( GL_TEXTURE_2D,  textureObj );
//   glTexSubImage2D(GL_TEXTURE_2D,
//                   0,      //mip level
//                   0, 0,   // offset
//                   frameVec[frameIndex].cols,
//                   frameVec[frameIndex].rows, 
//                   GL_BGR, 
//                   GL_UNSIGNED_BYTE,
//                   NULL);
//   if(frameIndex < frameVec.size())
//       frameIndex ++;
//   else
//       frameIndex = 0;

}
void Surface::dragHandle(double _x, double _y)
{
    /* so activeHandleIndex will be initialized to a value
     * greater than the number of handles so that no handles
     * are selcted by default. this will also be used
     * as the no selected state
     */
    if(activeHandleIndex < numHandles)
    {
        //cout << "drag x " << _x << " y " << _y << endl;
        //cout << "drag activeHandle index " << activeHandleIndex << endl;
        vertexData[activeHandleIndex] = _x;
        vertexData[activeHandleIndex+1] = _y;
        //cout <<"lvertexData[activeHandleIndex] " <<vertexData[activeHandleIndex] << endl;
        //cout <<"lvertexData[activeHandleIndex+1] " <<vertexData[activeHandleIndex+1] << endl;
        glBindBuffer(GL_ARRAY_BUFFER, vboVert);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, vertexData, GL_STATIC_DRAW);
    }
}

void Surface::releaseHandle()
{
    activeHandleIndex = numHandles + 1;
}

void Surface::reset()
{
    vidCap.set(CV_CAP_PROP_POS_FRAMES, 1.0);
}

float* Surface::getVertData()
{
    return vertexData;
}
