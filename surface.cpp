#include "surface.h"
Surface::Surface(std::vector<string> _plist, GLfloat* _vertexData, GLuint _program, int _handleRad):
    plist(_plist),
    program(_program),
    handleRad(_handleRad),
    activeHandleIndex(numHandles + 1),
    frameIndex(0),
    plistIndex(0)
{
    vidCap.open(plist[0].c_str());

//** code for caching all video frames in memory
//  while(vidCap.get(CV_CAP_PROP_POS_FRAMES) < vidCap.get(CV_CAP_PROP_FRAME_COUNT) - 1)
//  {
//      Mat tempFrame;
//      vidCap >> tempFrame;
//      frameVec.push_back(tempFrame);
//  }

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoordData), texCoordData, GL_STREAM_DRAW);

    glGenBuffers( 1, &texBuffer ); 
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texBuffer);

    glGenTextures( 1, &textureObj );
    glBindTexture( GL_TEXTURE_2D,  textureObj );
    //glBindTexture( GL_TEXTURE_2D,  textureObj );
    glPixelStorei(GL_UNPACK_ROW_LENGTH, frame.step/frame.elemSize());
    glBufferData( GL_PIXEL_UNPACK_BUFFER,
              frame.rows * frame.cols * 3,
              frame.data,
              GL_STREAM_DRAW); // need to change this to GL_STREAM_DRAW

    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, frame.cols, frame.rows);

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    surfaceID = surfaceCount;

    surfaceCount++;
}

void Surface::draw()
{
    glBindBuffer(GL_ARRAY_BUFFER, vboTexcoords);
    GLint intexLocation = glGetAttribLocation(program, "in_tex_coord");
    glVertexAttribPointer(intexLocation, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(intexLocation);

    //glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texBuffer);
    glBindTexture( GL_TEXTURE_2D,  textureObj );
    //glActiveTexture(texBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, vboVert);

    GLint inposLocation = glGetAttribLocation(program, "in_pos");
    glEnableVertexAttribArray(inposLocation);
    glVertexAttribPointer(inposLocation, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
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
        glBufferData(GL_ARRAY_BUFFER, sizeof(texCoordData), texCoordData, GL_STREAM_DRAW);

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

        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, frame.cols, frame.rows);

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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    vidCap >> frame;

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texBuffer);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, frame.step/frame.elemSize());
    glBufferData( GL_PIXEL_UNPACK_BUFFER,
                  frame.rows * frame.cols * 3,
                  frame.data,
                  GL_STREAM_DRAW); // need to change this to GL_STREAM_DRAW

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
