/*
 *  Core.cpp
 *  SpatialTest Project
 *
 *  GLUT application to help visualize and test the implemented spatial structures.
 *
 *  Created by radix on 05/04/08.
 *  Copyright Mykola Konyk, <mykola@konyk.org>, 2008.
 *  
 *  This code is under Microsoft Reciprocal License (Ms-RL)
 *  Please see http://www.opensource.org/licenses/ms-rl.html
 *
 *  Important points about the license (from Ms-RL):
 *
 *  [A] For any file you distribute that contains code from the software (in source code or binary format), you must provide 
 *  recipients the source code to that file along with a copy of this license, which license will govern that file. 
 *  You may license other files that are entirely your own work and do not contain code from the software under any terms 
 *  you choose.
 *
 *  [B] No Trademark License- This license does not grant you rights to use any contributors' name, logo, or trademarks.
 *
 *  [C] If you bring a patent claim against any contributor over patents that you claim are infringed by the software, your 
 *  patent license from such contributor to the software ends automatically.
 *
 *  [D] If you distribute any portion of the software, you must retain all copyright, patent, trademark, and attribution notices 
 *  that are present in the software.
 *
 *  [E] If you distribute any portion of the software in source code form, you may do so only under this license by including a
 *  complete copy of this license with your distribution. If you distribute any portion of the software in compiled or object 
 *  code form, you may only do so under a license that complies with this license.
 *
 *  [F] The software is licensed "as-is." You bear the risk of using it. The contributors give no express warranties, guarantees 
 *  or conditions. You may have additional consumer rights under your local laws which this license cannot change. To the extent 
 *  permitted under your local laws, the contributors exclude the implied warranties of merchantability, fitness for a particular 
 *  purpose and non-infringement.
 *
 */


#ifdef __APPLE__

    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
    #include <GLUT/glut.h>

#elif _MSC_VER

    #include <Windows.h>
    #include <GL/gl.h>
    #include <GL/glu.h>
    #include <GL/glut.h>

#else

    #include <GL/gl.h>
    #include <GL/glu.h>
    #include <GL/glut.h>

#endif


#include <stdlib.h>
#include <time.h>

#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>

#include "ISpatialStructure.h"
#include "ISpatialObject.h"
#include "SphereObject.h"

#include "BruteForce.h"
#include "SortAndSweep.h"
#include "UniformGrid.h"
#include "HierarchicalGrid.h"
#include "Octree.h"
#include "LooseOctree.h"
#include "Kdtree.h"

#include "Vector3.h"



//--
static SpatialTest::ISpatialStructure*                  g_pSpatialStruct = NULL;

static std::vector<SpatialTest::ISpatialObject*>        g_vecObjects;

// [rad] Min and Max radiuses for objects
static float                                            g_f32RadiusMin = 1.25f;
static float                                            g_f32RadiusMax = 7.25f;

// [rad] Number of objects to generate
static int                                              g_i32ObjectCount = 1000;

// [rad] Used to assign unique ids to objects
static int                                              g_i32LastId = 0;

static float                                            g_f32StartOffset = 50.0f;

// [rad] Bounding box for all objects
static SpatialTest::Vector3                             g_vec3ValuesMax = SpatialTest::Vector3(100.0f, 100.0f, 100.0f);
static SpatialTest::Vector3                             g_vec3ValuesMin = SpatialTest::Vector3(-100.0f, -100.0f, -100.0f);

// [rad] Number of hash buckets to use in Uniform grid and Hierarchical grid
static int                                              g_i32HashBucketCount = 2048;

static float                                            g_f32Frame = 0.015f;

// [rad] Screen width and height
static int                                              g_i32ScreenWidth = 800;
static int                                              g_i32ScreenHeight = 600;

static int                                              g_i32LastX = 0;
static int                                              g_i32LastY = 0;

static float                                            g_f32RotationX = 0.0f;
static float                                            g_f32RotationY = 0.0f;

static int                                              g_i32FirstFrame = 1;
static int                                              g_i32FrameCount = 0;

static int                                              g_i32TimeBase = 0;
static float                                            g_f32Fps = 0.0f;
static int                                              g_i32ResetFps = 0;
static float                                            g_f32MaxFps = 0.0f;

static int                                              g_i32CurrentSpatial = 2;
static int                                              g_i32DrawBox = 1;
static int                                              g_i32DrawAxis = 0;
static int                                              g_i32ShowCollisionCount = 1;
static int                                              g_i32MaxCollisions = 0;
static int                                              g_i32CurrentRebuild = 0;
static int                                              g_i32Pause = 0;
static int                                              g_i32OneFrame = 0;



//--
static void PrintText(const std::string& refString, int i32X, int i32Y);
static void RenderInfo();
static void RenderScene();
static void SetupRC();
static void ChangeSize(GLsizei w, GLsizei h);
static void MouseActiveMotion(int i32X, int i32Y);
static void KeyboardNormal(unsigned char u8Key, int i32X, int i32Y);

static void CreateObjects();
static void DeleteObjects();
static void CreateSpatialStructure();
static void DeleteSpatialStructure();

static void Tick();



//--
// [rad] Generate specified number of objects and place them randomly
static void CreateObjects()
{
    int i32Index;
    float f32Radius;

    SpatialTest::Vector3 vec3Position;
    SpatialTest::Vector3 vec3Direction;

    SpatialTest::SphereObject* pSphere;
    
    // [rad] Initialize random seed
    srand(time(0));
    
    for(i32Index = 0; i32Index < g_i32ObjectCount; i32Index++)
    {
        // [rad] Generate random radius
        f32Radius = (rand() /( static_cast<float>(RAND_MAX) + 1.0f)) * 
                                (g_f32RadiusMax - g_f32RadiusMin) + g_f32RadiusMin;
    
        // [rad] Generate random position
        vec3Position.x = (rand() /( static_cast<float>(RAND_MAX) + 1.0f)) * 
                                (g_vec3ValuesMax.x - g_vec3ValuesMin.x - 2.0f * g_f32StartOffset) + g_vec3ValuesMin.x + g_f32StartOffset;
                                
        vec3Position.y = (rand() /( static_cast<float>(RAND_MAX) + 1.0f)) * 
                                (g_vec3ValuesMax.y - g_vec3ValuesMin.y - 2.0f * g_f32StartOffset) + g_vec3ValuesMin.y + g_f32StartOffset;
                                
        vec3Position.z = (rand() /( static_cast<float>(RAND_MAX) + 1.0f)) * 
                                (g_vec3ValuesMax.z - g_vec3ValuesMin.z - 2.0f * g_f32StartOffset) + g_vec3ValuesMin.z + g_f32StartOffset;
                                
                                
        // [rad] Generate random direction
        vec3Direction.x = (rand() /( static_cast<float>(RAND_MAX) + 1.0f)) * 
                                (g_vec3ValuesMax.x - g_vec3ValuesMin.x) + g_vec3ValuesMin.x;
                                
        vec3Direction.y = (rand() /( static_cast<float>(RAND_MAX) + 1.0f)) * 
                                (g_vec3ValuesMax.y - g_vec3ValuesMin.y) + g_vec3ValuesMin.y;
                                
        vec3Direction.z = (rand() /( static_cast<float>(RAND_MAX) + 1.0f)) * 
                                (g_vec3ValuesMax.z - g_vec3ValuesMin.z) + g_vec3ValuesMin.z;
                                
                                
                                
        // [rad] Create sphere object
        pSphere = new SpatialTest::SphereObject(g_i32LastId);
        
        // [rad] Set radius, direction and position
        pSphere->VSetPosition(vec3Position);
        pSphere->VSetRadius(f32Radius);        
        pSphere->VSetDirection(vec3Direction);
        
        // [rad] Add object
        g_vecObjects.push_back(pSphere);
        
        // [rad] Increment unique id
        ++g_i32LastId;
    }
}



//--
static void DeleteObjects()
{
    std::vector<SpatialTest::ISpatialObject*>::iterator iter_objects;
    for(iter_objects = g_vecObjects.begin(); iter_objects != g_vecObjects.end(); iter_objects++)
    {
        delete((*iter_objects));
        (*iter_objects) = NULL;
    }
}



//--
static void CreateSpatialStructure()
{
    switch(g_i32CurrentSpatial)
    {
        case 1:
            delete(g_pSpatialStruct);
            g_pSpatialStruct = new SpatialTest::BruteForce();
            break;
            
        case 2:
            delete(g_pSpatialStruct);
            g_pSpatialStruct = new SpatialTest::SortAndSweep();
            break;
            
        case 3:
            delete(g_pSpatialStruct);
            g_pSpatialStruct = new SpatialTest::UniformGrid(g_i32HashBucketCount);
            break;
            
        case 4:
            delete(g_pSpatialStruct);
            g_pSpatialStruct = new SpatialTest::HierarchicalGrid(g_i32HashBucketCount);
            break;
            
        case 5:
            delete(g_pSpatialStruct);
            
            // [rad] Construct either an Octree or an Octree which is to be rebuilt every frame
            g_pSpatialStruct = new SpatialTest::Octree(SpatialTest::Vector3(0.0f, 0.0f, 0.0f), 100.0f, g_i32CurrentRebuild);
            break;
                        
        case 6:
            delete(g_pSpatialStruct);
            
            // [rad] Construct either a Loose Octree or a Loose Octree which is to be rebuilt every frame
            g_pSpatialStruct = new SpatialTest::LooseOctree(SpatialTest::Vector3(0.0f, 0.0f, 0.0f), 100.0f, g_i32CurrentRebuild);
            break;
            
            
        case 7:
            delete(g_pSpatialStruct);
            g_pSpatialStruct = new SpatialTest::KDTree(SpatialTest::Vector3(0.0f, 0.0f, 0.0f), 100.0f);
            break;
          
          
        default:
            return;
    }



    // [rad] Populate structure
    g_pSpatialStruct->VAddObjects(g_vecObjects);

    
    // [rad] Reset collision count
    g_i32MaxCollisions = 0;
    
    // [rad] Reset fps
    g_i32ResetFps = 1;
}



//--
static void DeleteSpatialStructure()
{
    delete(g_pSpatialStruct);
    g_pSpatialStruct = NULL;
}



//--
static void Tick()
{
    float f32Radius;
    int i32Collision;
    
    SpatialTest::Vector3 vec3Position;
    SpatialTest::Vector3 vec3Direction;
    SpatialTest::Vector3 vec3Normal;
    
    
    // [rad] Check if we are doing one frame processing
    if(g_i32OneFrame && g_i32Pause)
    {
        g_i32Pause = 0;
    }
    
    
    // [rad] only do collision detection / object update if not paused
    if(!g_i32Pause)
    {
        // [rad] For each object, check wall collisions
        
        std::vector<SpatialTest::ISpatialObject*>::iterator iter_objects;
        for(iter_objects = g_vecObjects.begin(); iter_objects != g_vecObjects.end(); iter_objects++)
        {
            // [rad] Reset collision info
            (*iter_objects)->VCollisionOff();
    
    
            // [rad] Check wall collisions
            vec3Position = (*iter_objects)->VGetPosition();
            vec3Direction = (*iter_objects)->VGetDirection();
            f32Radius = (*iter_objects)->VGetRadius();
        
            i32Collision = 0;
        
            // [rad] Check if we are close enough to the wall
        
            if(vec3Position.x - f32Radius <= g_vec3ValuesMin.x)
            {
                // [rad] Left wall
                vec3Normal = SpatialTest::Vector3(1.0f, 0.0f, 0.0f);
            
                vec3Direction -= 2.0f * vec3Normal * vec3Direction.Dot(vec3Normal); 
                (*iter_objects)->VSetDirection(vec3Direction);
            
                i32Collision = 1;
            }
        
            if(vec3Position.x + f32Radius >= g_vec3ValuesMax.x)
            {
                // [rad] Right wall
                vec3Normal = SpatialTest::Vector3(-1.0f, 0.0f, 0.0f);
            
                vec3Direction -= 2.0f * vec3Normal * vec3Direction.Dot(vec3Normal); 
                (*iter_objects)->VSetDirection(vec3Direction);
            
                i32Collision = 1;
            }
        
            if(vec3Position.z - f32Radius <= g_vec3ValuesMin.z)
            {
                // [rad] Far wall
                vec3Normal = SpatialTest::Vector3(0.0f, 0.0f, 1.0f);
            
                vec3Direction -= 2.0f * vec3Normal * vec3Direction.Dot(vec3Normal); 
                (*iter_objects)->VSetDirection(vec3Direction);
            
                i32Collision = 1;
            }
        
            if(vec3Position.z + f32Radius >= g_vec3ValuesMax.z)
            {
                // [rad] Near wall
                vec3Normal = SpatialTest::Vector3(0.0f, 0.0f, -1.0f);
            
                vec3Direction -= 2.0f * vec3Normal * vec3Direction.Dot(vec3Normal); 
                (*iter_objects)->VSetDirection(vec3Direction);
            
                i32Collision = 1;
            }
        
            if(vec3Position.y - f32Radius <= g_vec3ValuesMin.y)
            {
                // [rad] Bottom wall
                vec3Normal = SpatialTest::Vector3(0.0f, 1.0f, 0.0f);
            
                vec3Direction -= 2.0f * vec3Normal * vec3Direction.Dot(vec3Normal); 
                (*iter_objects)->VSetDirection(vec3Direction);
            
                i32Collision = 1;
            }
        
            if(vec3Position.y + f32Radius >= g_vec3ValuesMax.y)
            {
                // [rad] Top wall
                vec3Normal = SpatialTest::Vector3(0.0f, -1.0f, 0.0f);
            
                vec3Direction -= 2.0f * vec3Normal * vec3Direction.Dot(vec3Normal); 
                (*iter_objects)->VSetDirection(vec3Direction);
            
                i32Collision = 1;
            }
        
            if(!i32Collision)
            {
                // [rad] Nothing special
                vec3Normal = SpatialTest::Vector3(0.0f, 0.0f, 0.0f);
            
                vec3Direction -= 2.0f * vec3Normal * vec3Direction.Dot(vec3Normal); 
                (*iter_objects)->VSetDirection(vec3Direction);

            }
        
        
        
            // [rad] Update position
            vec3Position += vec3Direction * g_f32Frame;
            (*iter_objects)->VSetPosition(vec3Position);
            
            
            // [rad] Check if this was one frame processing
            if(g_i32OneFrame)
            {
                g_i32Pause = 1;
                g_i32OneFrame = 0;
            }
        }
    
    
        // [rad] Do collision detection
        g_pSpatialStruct->VUpdate();
    
    }
    
    
    glutPostRedisplay();
}


//--
static void PrintText(const std::string& refString, int i32X, int i32Y)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    
    gluOrtho2D(0, g_i32ScreenWidth, 0, g_i32ScreenHeight);
    glScalef(1, -1, 1);
	glTranslatef(0, -g_i32ScreenHeight, 0);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(1.0f, 1.0f, 0.0f);
    
    glRasterPos2f(i32X, i32Y);
    
    for(int i = 0; i < refString.size(); i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, refString[i]);
    }
    
    glColor3f(1.0f, 0.0f, 0.0f);
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

}



//--
static void RenderInfo()
{
    int i32Time;
    int i32CollisionCount = 0;

    std::stringstream ssSerial;
    std::string sBuf;
    
    
    // [rad] Upper menu
    sBuf = "";
    ssSerial.str("");
    ssSerial << "[1] Brute Force    ";
    ssSerial << "[2] Sort and Sweep    ";
    ssSerial << "[3] Uniform Grid    ";
    ssSerial << "[4] Hierarchical Grid    ";
    ssSerial << "[5] Octree / Octree (Rebuild)   ";
    
    sBuf = ssSerial.str();
    PrintText(sBuf, 20, 20);
    
    
    
    // [rad] middle menu
    sBuf = "";
    ssSerial.str("");
    ssSerial << "[6] Loose Octree / Loose Octree (Rebuild)    ";
    ssSerial << "[7] KD-Tree (SAH)    ";
        
    sBuf = ssSerial.str();
    PrintText(sBuf, 20, 40);
    
    
    
    // [rad] Lower menu
    sBuf = "";
    ssSerial.str("");
    ssSerial << "[Q] Quit    ";
    ssSerial << "[B] Enable / Disable Box    ";
    ssSerial << "[X] Enable / Disable Axis    ";
    ssSerial << "[N] Show Collision Count    ";
    sBuf = ssSerial.str();
    PrintText(sBuf, 20, 70);
    
    
    // [rad] Lower menu (2nd)
    sBuf = "";
    ssSerial.str("");
    ssSerial << "[P] Pause    ";
    ssSerial << "[Space] One Frame (Pause)    "; 
    ssSerial << "[R] Reposition    ";
    sBuf = ssSerial.str();
    PrintText(sBuf, 20, 90);
    
     
     
    
    // [rad] Print spatial structure
    sBuf = "";
    ssSerial.str("");
    ssSerial << "Spatial Structure: ";
    
    switch(g_i32CurrentSpatial)
    {
        case 1:
            {
                ssSerial << "Brute force";
            }
            break;
            
        case 2:
            {
                ssSerial << "Sort and Sweep";
            }
            break;
            
        case 3:
            {
                ssSerial << "Uniform Grid";
            }
            break;
            
        case 4:
            {
                ssSerial << "Hierarchical Grid";
            }
            break;
            
        case 5:
            {
                if(!g_i32CurrentRebuild)
                {
                    ssSerial << "Octree";
                }
                else
                {
                    ssSerial << "Octree (Rebuild)";
                }
            }
            break;
            
        case 6:
            {
                if(!g_i32CurrentRebuild)
                {
                    ssSerial << "Loose Octree";
                }
                else
                {
                    ssSerial << "Loose Octree (Rebuild)";
                }
            }
            break;
            
            
        case 7:
            {
                ssSerial << "KD-Tree (SAH)";
            }
            break;
            
            
        default:
            ssSerial << "Unknown";
    }

    sBuf = ssSerial.str();
    PrintText(sBuf, 20, 130);
    
    
    
    // [rad] Print fps
    ++g_i32FrameCount;

    i32Time = glutGet(GLUT_ELAPSED_TIME);
    if(i32Time - g_i32TimeBase > 1000)
    {
        g_f32Fps = g_i32FrameCount * 1000.0f / static_cast<float>(i32Time - g_i32TimeBase);
        g_i32TimeBase = i32Time;
        g_i32FrameCount = 0;
        
        if(g_i32ResetFps)
        {
            g_i32ResetFps = 0;
            g_f32MaxFps = 0.0f;
            g_f32Fps = 0.0f;
        }
            
    }
    
    
    // [rad] Store max fps, if matched
    if(g_f32MaxFps < g_f32Fps)
    {
        g_f32MaxFps = g_f32Fps;
    }

    
    
    sBuf = "";
    ssSerial.str("");
    ssSerial << "FPS: " << g_f32Fps;
    sBuf = ssSerial.str();
   
    PrintText(sBuf, 20, 150);
    
    
    sBuf = "";
    ssSerial.str("");
    ssSerial << "Maximum FPS: " << g_f32MaxFps;
    sBuf = ssSerial.str();
   
    PrintText(sBuf, 20, 170);
    
    
    
    // [rad] Print object count
    sBuf = "";
    ssSerial.str("");
    ssSerial << "Number of objects: " << g_i32ObjectCount;
    sBuf = ssSerial.str();
    
    PrintText(sBuf, 20, 190);
   
   
   
    // [rad] Calculate number of collisions
    std::vector<SpatialTest::ISpatialObject*>::iterator iter_objects;
    for(iter_objects = g_vecObjects.begin(); iter_objects != g_vecObjects.end(); iter_objects++)
    {
        if((*iter_objects)->VGetCollisionStatus())
        {
            ++i32CollisionCount;
        }
    }
    
    
    // [rad] Update collision count, if needed
    if(g_i32MaxCollisions < i32CollisionCount)
    {   
        g_i32MaxCollisions = i32CollisionCount;
    }
   
   
   
    // [rad] Print collision count
    if(g_i32ShowCollisionCount)
    {
        sBuf = "";
        ssSerial.str("");
    
        ssSerial << "Number of collisions: " << (i32CollisionCount / 2);
        sBuf = ssSerial.str();
   
        PrintText(sBuf, 20, 210);
        
        
        sBuf = "";
        ssSerial.str("");
    
        ssSerial << "Maximum number of collisions: " << (g_i32MaxCollisions / 2);
        sBuf = ssSerial.str();
   
        PrintText(sBuf, 20, 230);
    }
   
        
   

}


//--
static void RenderScene()
{
    // [rad] Do object update / collision detection
    Tick();

    
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    // [rad] Render text
    RenderInfo();
    
    
    // [rad] Rotate scene if necessary
    glRotatef(g_f32RotationX, 1.0f, 0.0f, 0.0f);
    g_f32RotationX = 0.0f;

    glRotatef(g_f32RotationY, 0.0f, 1.0f, 0.0f);
    g_f32RotationY = 0.0f;
    
    
    // [rad] Render all objects
    std::vector<SpatialTest::ISpatialObject*>::iterator iter_objects;
    for(iter_objects = g_vecObjects.begin(); iter_objects != g_vecObjects.end(); iter_objects++)
    {
        glPushMatrix();
        glTranslatef((*iter_objects)->VGetPosition().x, (*iter_objects)->VGetPosition().y,
                        (*iter_objects)->VGetPosition().z);
                        
        
        // [rad] Check if this object is colliding
        if((*iter_objects)->VGetCollisionStatus())
        {
            glColor3f(1.0f, 0.0f, 0.0f);
        }
        else
        {
            glColor3f(1.0f, 1.0f, 1.0f);
        }
                        
                        
        // [rad] We'll use cubes to draw our spheres, since they require the
        // least number of polygons
        glutSolidCube((*iter_objects)->VGetRadius());
        
        glPopMatrix();
    }
    
    
    // [rad] Draw box (cube) if enabled
    if(g_i32DrawBox)
    {
        glColor3f(1.0f, 1.0f, 1.0f);
        
        //glutWireCube(200.0f);
        
        
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(5, 0x55555);    
            
        
        glBegin(GL_LINE_STRIP);
        
            glVertex3f(-100.0f, 100.0f, -100.0f);
            glVertex3f(100.0f, 100.0f, -100.0f);
            glVertex3f(100.0f, 100.0f, 100.0f);
            glVertex3f(-100.0f, 100.0f, 100.0f);
            glVertex3f(-100.0f, 100.0f, -100.0f);
        
        glEnd();
        
        
        glBegin(GL_LINE_STRIP);
        
            glVertex3f(-100.0f, -100.0f, -100.0f);
            glVertex3f(100.0f, -100.0f, -100.0f);
            glVertex3f(100.0f, -100.0f, 100.0f);
            glVertex3f(-100.0f, -100.0f, 100.0f);
            glVertex3f(-100.0f, -100.0f, -100.0f);
        
        glEnd();
            
                    
        glBegin(GL_LINES);
        
            glVertex3f(-100.0f, 100.0f, 100.0f);
            glVertex3f(-100.0f, -100.0f, 100.0f);
            
            glVertex3f(-100.0f, 100.0f, -100.0f);
            glVertex3f(-100.0f, -100.0f, -100.0f);
            
            glVertex3f(100.0f, 100.0f, -100.0f);
            glVertex3f(100.0f, -100.0f, -100.0f);
            
            glVertex3f(100.0f, 100.0f, 100.0f);
            glVertex3f(100.0f, -100.0f, 100.0f);
        
        glEnd();
        
        
        glDisable(GL_LINE_STIPPLE);
    }
    
    
    // [rad] Draw axis
    if(g_i32DrawAxis)
    {
        glColor3f(1.0f, 1.0f, 1.0f);
        
        
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(5, 0x55555);

        
        glBegin(GL_LINES);

            glVertex3f(-150.0f, 0.0f, 0.0f);
            glVertex3f(150.0f, 0.0f, 0.0f);

            glVertex3f(0.0f, -150.0f, 0.0f);
            glVertex3f(0.0f, 150.0f, 0.0f);
            
            glVertex3f(0.0f, 0.0f, -150.0f);
            glVertex3f(0.0f, 0.0f, 150.0f);
    
        glEnd();
        
        
        glDisable(GL_LINE_STIPPLE);
    }
    
    
    // [rad] Swap double buffer
    glutSwapBuffers();
}



//--
static void SetupRC()
{
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
}



//--
static void ChangeSize(GLsizei w, GLsizei h)
{
    // [rad] Store new size
    g_i32ScreenWidth = w;
    g_i32ScreenHeight = h;
    
    
    GLfloat range = 200.0f;
    
    if(!h)
    {
        h = 1;
    }
    
    glViewport(0, 0, w, h);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    if(w <= h)
    {
        glOrtho(-range, range, -range * h / w, range * h / w, -range, range);
    }
    else
    {
        glOrtho(-range * w / h, range * w /h, -range, range, -range, range);
    }
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


//--
static void KeyboardNormal(unsigned char u8Key, int i32X, int i32Y)
{
    int i32Value = u8Key - 48;
    
    if(i32Value >= 1 && i32Value <= 7)
    {
        if(i32Value == g_i32CurrentSpatial)
        {
            g_i32CurrentRebuild = !g_i32CurrentRebuild;
        }
        else
        {
            g_i32CurrentSpatial = i32Value;
            g_i32CurrentRebuild = 0;
        }


        // [rad] Re-create spatial structure
        CreateSpatialStructure();
    }
    else if(u8Key == 'b' || u8Key == 'B')
    {
        g_i32DrawBox = !g_i32DrawBox;
    }
    else if(u8Key == 'x' || u8Key == 'X')
    {
        g_i32DrawAxis = !g_i32DrawAxis;
    }
    else if(u8Key == 'q' || u8Key == 'Q')
    {
        exit(0);
    }
    else if(u8Key == 'n' || u8Key == 'N')
    {
        g_i32ShowCollisionCount = !g_i32ShowCollisionCount;
    }
    else if(u8Key == 'r' || u8Key == 'R')
    {
        ChangeSize(g_i32ScreenWidth, g_i32ScreenHeight);
    }
    else if(u8Key == 'p' || u8Key == 'P')
    {
        g_i32Pause = !g_i32Pause;
    }
    else if(u8Key == ' ')
    {
        if(!g_i32Pause)
        {
            g_i32Pause = 0;
        }
        
        g_i32OneFrame = 1;
    }
}



//--
static void MouseActiveMotion(int i32X, int i32Y)
{
    if(g_i32FirstFrame)
    {
        g_i32FirstFrame = 0;
    }
    else
    {
        g_f32RotationX = static_cast<float>(g_i32LastX - i32X) * 0.5f;
        g_f32RotationY = static_cast<float>(g_i32LastY - i32Y) * 0.5f;
    }
    
    g_i32LastX = i32X;
    g_i32LastY = i32Y;
}



//--
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(g_i32ScreenWidth, g_i32ScreenHeight);
    glutCreateWindow("SpatialTest");
    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);
    glutMotionFunc(MouseActiveMotion);
    glutKeyboardFunc(KeyboardNormal);
    
    CreateObjects();
    CreateSpatialStructure();
    
    SetupRC();
    
    glutMainLoop();
    
    DeleteSpatialStructure();
    DeleteObjects();
    
    return(0);
}
