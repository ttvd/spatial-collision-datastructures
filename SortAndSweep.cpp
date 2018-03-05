/*
 *  SortAndSweep.cpp
 *  SpatialTest Project
 *
 *  Sort and Sweep approach.
 *
 *  Created by radix on 07/04/08.
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

#include "SortAndSweep.h"

#include <algorithm>
#include <cstdlib>

namespace SpatialTest
{
    //--
    struct CompareAxisX
    {
        bool operator()(const ISpatialObject* pLeft, const ISpatialObject* pRight)
        {
            return(pLeft->VGetPosition().x - pLeft->VGetRadius() < pRight->VGetPosition().x - pRight->VGetRadius());
        }
    };
    
    
    //--
    struct CompareAxisY
    {
        bool operator()(const ISpatialObject* pLeft, const ISpatialObject* pRight)
        {
            return(pLeft->VGetPosition().y - pLeft->VGetRadius() < pRight->VGetPosition().y - pRight->VGetRadius());
        }
    };
    
    
    //--
    struct CompareAxisZ
    {
        bool operator()(const ISpatialObject* pLeft, const ISpatialObject* pRight)
        {
            return(pLeft->VGetPosition().z - pLeft->VGetRadius() < pRight->VGetPosition().z - pRight->VGetRadius());
        }
    };
    
    

    //--
    SortAndSweep::SortAndSweep() :
        ISpatialStructure(),
        m_i32SortAxis(0)
    {
    
    }
    
    
    //--
    void
    SortAndSweep::VAddObjects(const std::vector<ISpatialObject*>& refObjects)
    {
        // [rad] Copy elements into the local vector
        std::vector<ISpatialObject*>::const_iterator iter_object;
        for(iter_object = refObjects.begin(); iter_object != refObjects.end(); iter_object++)
        {
            m_vecObjects.push_back(*iter_object);
        }
    }
    
    
    //--
    void
    SortAndSweep::VUpdate()
    {
        int i32Index;
        int i32Break;
        
        float f32Sum1[3] = {0.0f, 0.0f, 0.0f};
        float f32Sum2[3] = {0.0f, 0.0f, 0.0f};
        float f32Var[3];
        
        Vector3 vec3Center1;
        Vector3 vec3Center2;
        
        float f32Radius1;
        float f32Radius2;
                
        std::vector<ISpatialObject*>::iterator iter_object1;
        std::vector<ISpatialObject*>::iterator iter_object2;
        
    
        // [rad] Sort Objects on the given axis
        switch(m_i32SortAxis)
        {
            case 0:
                {
                    // [rad] Sort on X axis
                    std::sort(m_vecObjects.begin(), m_vecObjects.end(), CompareAxisX());
                }
                break;
                
            case 1:
                {
                    // [rad] Sort on Y axis
                    std::sort(m_vecObjects.begin(), m_vecObjects.end(), CompareAxisY());
                }
                break;
                
            case 2:
                {
                    // [rad] Sort on Z axis
                    std::sort(m_vecObjects.begin(), m_vecObjects.end(), CompareAxisZ());
                }
                break;
        }
        
        
        // [rad] Now iterate through objects
        for(iter_object1 = m_vecObjects.begin(); iter_object1 != m_vecObjects.end(); iter_object1++)
        {
            // [rad] Retrieve this object's center and radius
            vec3Center1 = (*iter_object1)->VGetPosition();
            f32Radius1 = (*iter_object1)->VGetRadius();
            
            
            // [rad] Update sums: we'll need this to compute variance of sphere centers
            for(i32Index = 0; i32Index < 3; i32Index++)
            {
                f32Sum1[i32Index] += vec3Center1[i32Index];
                f32Sum2[i32Index] += vec3Center1[i32Index] * vec3Center1[i32Index];
            }
            
            
            // [rad] Set break flag
            i32Break = 0;
            
            
            // [rad] Test collisions vs all possible objects, starting from
            // the current position
            for(iter_object2 = iter_object1; iter_object2 != m_vecObjects.end(); iter_object2++)
            {
                vec3Center2 = (*iter_object2)->VGetPosition();
                f32Radius2 = (*iter_object2)->VGetRadius();
                
                // [rad] Self check
                if(*iter_object2 == *iter_object1) continue;
                
                // [rad] Can stop if we went beyond the current AABB
                switch(m_i32SortAxis)
                {
                    case 0:
                        {
                            if(vec3Center2.x - f32Radius2 > vec3Center1.x + f32Radius1)
                            {
                                i32Break = 1;
                            }
                        }
                        break;
                        
                    case 1:
                        {
                            if(vec3Center2.y - f32Radius2 > vec3Center1.y + f32Radius1)
                            {
                                i32Break = 1;
                            }
                        }
                        break;
                        
                    case 2:
                        {
                            if(vec3Center2.z - f32Radius2 > vec3Center1.z + f32Radius1)
                            {
                                i32Break = 1;
                            }
                        }
                        break;
                }
                
                
                // [rad] Stop if we are beyond the current AABB
                if(i32Break)
                {
                    break;
                }
                
                
                // [rad] Perform actual collision test
                if((*iter_object1)->VCheckCollision((*iter_object2)))
                {
                    // [rad] Mark both as in collision
                    (*iter_object1)->VCollisionOn();
                    (*iter_object2)->VCollisionOn();
                    
                }
                                
            }
            
        }
        
        // [rad] Compute variance
        for(i32Index = 0; i32Index < 3; i32Index++)
        {
            f32Var[i32Index] = f32Sum2[i32Index] - f32Sum1[i32Index] * f32Sum1[i32Index] / static_cast<float>(m_vecObjects.size());
        }
            
            
        // [rad] Update axis - select one with highest AABB variance
        m_i32SortAxis = 0;
            
        if(f32Var[1] > f32Var[0])
        {
            m_i32SortAxis = 1;
        }
            
        if(f32Var[2] > f32Var[m_i32SortAxis])
        {
            m_i32SortAxis = 2;
        }
    }
}