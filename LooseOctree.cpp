/*
 *  LooseOctree.cpp
 *  SpatialTest Project
 *
 *  Loose Octree implementation.
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

#include "Base.h"
#include "LooseOctree.h"
#include <cstdlib>

namespace SpatialTest
{
    //--
    int LooseOctree::s_i32MaxDepth                      = 5;
    float LooseOctree::s_f32MaxObjectNodeRatio          = 8.0f;
    
    
    //--
    LooseOctree::LooseOctree(const Vector3& refCenter, float f32HalfWidth, int i32Rebuild) :
        m_i32Rebuild(i32Rebuild),
        m_f32HalfWidth(f32HalfWidth)
    {
        // [rad] Create root node (0 depth)
        m_pRootNode = new LooseOctreeNode(NULL, refCenter, f32HalfWidth, 0);
    }
    
    
    
    //--
    LooseOctree::~LooseOctree()
    {
        if(m_pRootNode)
        {
            delete(m_pRootNode);
            m_pRootNode = NULL;
        }
    }
    
    
    
    //--
    inline
    void
    LooseOctree::Preallocate(int i32Depth)
    {
        m_pRootNode->Preallocate(i32Depth);
    }
    

    
    
    //--
    void
    LooseOctree::VUpdate()
    {   
        // [rad] We are completely rebuilding this loose octree
        if(m_i32Rebuild)
        {
        
            // [rad] We are going to rebuild octree (without dealocating space)
            m_pRootNode->Rebuild();
        
        
            // [rad] Populate the octree
            std::vector<ISpatialObject*>::iterator iter_object;
            for(iter_object = m_vecObjects.begin(); iter_object != m_vecObjects.end(); iter_object++)
            {
                m_pRootNode->AddObject((*iter_object));
            }
        }
        else
        {      
            LooseOctreeNode* pNode;
            ISpatialObject* pObject;
            ISpatialObject* pPrev;
        
            std::vector<ISpatialObject*>::iterator iter_object;
            for(iter_object = m_vecObjects.begin(); iter_object != m_vecObjects.end(); iter_object++)
            {
                pNode = static_cast<LooseOctreeNode*>((*iter_object)->VGetCell());
                
                // [rad] Check if this node still contains the element
                if(pNode->CheckContains(*iter_object))
                {
                    continue;
                }
                else
                {
                    // [rad] Remove element
                    if(pNode->m_pObjects == (*iter_object))
                    {
                        pNode->m_pObjects = pNode->m_pObjects->VGetNext();
                    }
                    else
                    {
                        // [rad] traverse list and remove
                        pObject = pNode->m_pObjects;
                    
                        while(pObject)
                        {
                            pPrev = pObject;
                            pObject = pObject->VGetNext();
                        
                            if(pObject == (*iter_object))
                            {
                                pPrev->VSetNext(pObject->VGetNext());
                                break;
                            }
                        }
                    }
                    
                    
                    
                    // [rad] Go up one node
                    pNode = pNode->m_pParent;
                
                    // [rad] Check if this node still contains this object
                    while(pNode)
                    {
                        // [rad] Check if contains, here we use non-loose 
                        // check, because it's possible that object will be
                        // stuck in the parent's node (since it might happen
                        // that object will be in it's loose dimensions)
                        if(pNode->CheckContainsNonLoose(*iter_object))
                        {
                            pNode->AddObject((*iter_object));
                            break;
                        }
                        else
                        {
                            pNode = pNode->m_pParent;
                        }
                    }   
                    
                    
                    //m_pRootNode->AddObject(*iter_object);     
                }
            }
            
        }
        

                
        // [rad] Do top-down collision testing
        std::vector<ISpatialObject*>::iterator iter_object;
        for(iter_object = m_vecObjects.begin(); iter_object != m_vecObjects.end(); iter_object++)
        {
            m_pRootNode->CheckCollisions((*iter_object));
        }
        
    }
    
    
    //--
    void
    LooseOctree::VAddObjects(const std::vector<ISpatialObject*>& refObjects)
    {        
        float f32MinDiameter = 1.0e38f;
        float f32CellSize = 2.0f * m_f32HalfWidth;
        float f32Diameter;

        int i32Depth = 0;
        int i32Divisions = 1;

        std::vector<ISpatialObject*>::const_iterator iter_object;
        for(iter_object = refObjects.begin(); iter_object != refObjects.end(); iter_object++)
        {
            f32Diameter = (*iter_object)->VGetRadius();
            
            if(f32Diameter < f32MinDiameter)
            {
                // [rad] Found a smaller object, update
                f32MinDiameter = f32Diameter;
            }
        }
        
        // [rad] Make cell size 'x' times as big as the smallest diameter 
        f32MinDiameter *= s_f32MaxObjectNodeRatio;
        
        
        // [rad] Calculate depth
        f32CellSize = 2.0f * m_f32HalfWidth;
        
        while(f32MinDiameter <= f32CellSize)
        {
            // [rad] Don't make more than max depth 
            if(s_i32MaxDepth == i32Depth)
            {
                break;
            }
            
            i32Divisions *= 2;                        
            f32CellSize = 2.0f * m_f32HalfWidth / static_cast<float>(i32Divisions);        
            
            i32Depth++;
        }
        
        
        // [rad] Delete everything but the root
        m_pRootNode->Free();
        
        // [rad] Pre-allocate the tree
        Preallocate(i32Depth);


        
        // [rad] Insert elements into the tree
        for(iter_object = refObjects.begin(); iter_object != refObjects.end(); iter_object++)
        {
            m_pRootNode->AddObject((*iter_object));
            
            // [rad] Store locally in a list, for fast iteration
            m_vecObjects.push_back((*iter_object));
        }
        
        
    }
    
    
    
    

    //--
    LooseOctreeNode::LooseOctreeNode(LooseOctreeNode* pParent, const Vector3& refCenter, float f32HalfWidth, int i32Depth) :
        m_vec3Center(refCenter),
        m_f32HalfWidth(f32HalfWidth),
        m_i32Depth(i32Depth),
        m_pObjects(NULL),
        m_pParent(pParent)
    {
        for(int i = 0; i < 8; i++)
        {
            m_pChildren[i] = NULL;
        }
    }
    
    
    //--
    LooseOctreeNode::~LooseOctreeNode()
    {
        for(int i = 0; i < 8; i++)
        {
            if(m_pChildren[i])
            {
                delete(m_pChildren[i]);
                m_pChildren[i] = NULL;
            }
        }            
    }
    
    
    
    //--
    // [rad] Delete all children
    void
    LooseOctreeNode::Free()
    {
        for(int i = 0; i < 8; i++)
        {
            if(m_pChildren[i])
            {
                delete(m_pChildren[i]);
                m_pChildren[i] = NULL;
            }
        }            
    }

    
    
    //--
    void
    LooseOctreeNode::CheckCollisions(ISpatialObject* pObject)
    {        
        
        // [rad] Check if the object is completely outside the boundary
        // for this node
        //if(!CheckContains(pObject))
        if(!CheckBoundaries(pObject))
        {
            // [rad] Object is outside of this node
            return;
        }
        
        // [rad] Check children recursively
        for(int i = 0; i < 8; i++)
        {
            if(m_pChildren[i])
            {
                m_pChildren[i]->CheckCollisions(pObject);
            }
        }   
        
        
        // [rad] Check all objects in this node
        ISpatialObject* pIter = m_pObjects;
        while(pIter)
        {
            // [rad] Skip self check
            if(pIter != pObject)
            {
                if(pIter->VCheckCollision(pObject))
                {
                    // [rad] Mark both as in collision
                    pObject->VCollisionOn();
                    pIter->VCollisionOn();
                }
            }
            
            pIter = pIter->VGetNext();
        }
    }
    


    //--
    // [rad] Checks whether the given object is inside the boundaries
    // partially or non-partially
    inline
    int
    LooseOctreeNode::CheckBoundaries(ISpatialObject* pObject)
    {
        /*
        int i32Index = 0;
        
        float f32Radius = pObject->VGetRadius();
        Vector3 vec3Center = pObject->VGetPosition(); 
    
        for(int i32Index = 0; i32Index < 3; i32Index++)
        {        
            // [rad] Adapted to take looseness factor into account (side * looseness)
            if((m_vec3Center[i32Index] - 2.0f * m_f32HalfWidth > vec3Center[i32Index] + f32Radius) ||
                (m_vec3Center[i32Index] + 2.0f * m_f32HalfWidth < vec3Center[i32Index] - f32Radius))
            {
                return(0);
            }
        }
        
        return(1);        
        */
        
        
        float f32SqDistance = 0.0f;
        int i32Index = 0;
        
        float f32Min;
        float f32Max;
        
        Vector3 vec3Center = pObject->VGetPosition(); 
        float f32Radius = pObject->VGetRadius();
        
        for(int i32Index = 0; i32Index < 3; i32Index++)
        {
            float f32V = vec3Center[i32Index];
            
            f32Min = m_vec3Center[i32Index] - 2.0f * m_f32HalfWidth;
            f32Max = m_vec3Center[i32Index] + 2.0f * m_f32HalfWidth;
            
            if(f32V < f32Min)
            {
                f32SqDistance += (f32Min - f32V) * (f32Min - f32V);
            }
            
            if(f32V > f32Max)
            {
                f32SqDistance += (f32V - f32Max) * (f32V - f32Max);
            }        
        }
        
        return(f32SqDistance <= f32Radius * f32Radius);
        
    }



    //--
    inline
    int
    LooseOctreeNode::CheckContains(ISpatialObject* pObject)
    {
        int i32Index = 0;
        
        float f32Radius = pObject->VGetRadius();
        Vector3 vec3Center = pObject->VGetPosition(); 
        
        
        for(int i32Index = 0; i32Index < 3; i32Index++)
        {
            // [rad] Adapted to take looseness factor into account (side * looseness)
            if((m_vec3Center[i32Index] - 2.0f * m_f32HalfWidth > vec3Center[i32Index] - f32Radius) ||
                (m_vec3Center[i32Index] + 2.0f * m_f32HalfWidth < vec3Center[i32Index] + f32Radius))
            {
                return(0);
            }
        }

        return(1);
    }
    
    
    
    //--
    int
    LooseOctreeNode::CheckContainsNonLoose(ISpatialObject* pObject)
    {
        int i32Index = 0;
        
        float f32Radius = pObject->VGetRadius();
        Vector3 vec3Center = pObject->VGetPosition(); 
        
        
        for(int i32Index = 0; i32Index < 3; i32Index++)
        {
            if((m_vec3Center[i32Index] - m_f32HalfWidth > vec3Center[i32Index] - f32Radius) ||
                (m_vec3Center[i32Index] + m_f32HalfWidth < vec3Center[i32Index] + f32Radius))
            {
                return(0);
            }
        }

        return(1);
    }



    
    
    //--
    void
    LooseOctreeNode::AddObject(ISpatialObject* pObject)
    {
        int i32Index = 0;
        int i32Position = 0;
        int i32Straddle = 0;
        
        float f32Radius = pObject->VGetRadius();
        Vector3 vec3Center = pObject->VGetPosition();      
        
        
        for(int i32Index = 0; i32Index < 3; i32Index++)
        {
            if(m_vec3Center[i32Index] < vec3Center[i32Index])
            {
                if(m_vec3Center[i32Index] - (m_f32HalfWidth / 2.0f) > vec3Center[i32Index] - f32Radius)
                {
                    i32Straddle = 1;
                    break;
                }
                else
                {
                    i32Position |= (1 << i32Index);
                }
            }
            else
            {
                if(m_vec3Center[i32Index] + (m_f32HalfWidth / 2.0f) < vec3Center[i32Index] + f32Radius)
                {
                    i32Straddle = 1;
                    break;
                }
            }        
        }
        
        
        if(!i32Straddle && m_pChildren[i32Position])
        {
            // [rad] Contained in existing child node
            m_pChildren[i32Position]->AddObject(pObject);
        }
        else
        {
            // [rad] Store this node for fast back-link
            pObject->VSetCell(this);
        
            pObject->VSetNext(m_pObjects);
            m_pObjects = pObject;
        }

    }
    
        
        
    //--
    inline
    bool
    LooseOctreeNode::IsLeaf() const
    {
        return(!m_pChildren[0] && !m_pChildren[1] && !m_pChildren[2] &&
                !m_pChildren[3] && !m_pChildren[4] && !m_pChildren[5] &&
                !m_pChildren[6] && !m_pChildren[7]);
    }
    
    
    
    //--
    void
    LooseOctreeNode::Rebuild()
    {
        for(int i = 0; i < 8; i++)
        {
            if(m_pChildren[i])
            {
                m_pChildren[i]->Rebuild();
            }
            
            m_pObjects = NULL;
        }
    }
    
    
    
    //--
    void
    LooseOctreeNode::Preallocate(int i32Depth)
    {
        // [rad] Check conditions
        if(i32Depth && IsLeaf())
        {
            Vector3 vec3NewCenter;
            
            float f32Step = m_f32HalfWidth * 0.5f;
            
            for(int i = 0; i < 8; i++)
            {
                vec3NewCenter = m_vec3Center;
                
                if(i & 1)
                {
                    vec3NewCenter.x += f32Step;
                }
                else
                {
                    vec3NewCenter.x -= f32Step;
                }
                
                
                if(i & 2)
                {
                    vec3NewCenter.y += f32Step;
                }
                else
                {
                    vec3NewCenter.y -= f32Step;
                }
                
                
                if(i & 4)
                {
                    vec3NewCenter.z += f32Step;
                }
                else
                {
                    vec3NewCenter.z -= f32Step;
                }
                
                
                // [rad] Allocate node
                m_pChildren[i] = new LooseOctreeNode(this, vec3NewCenter, f32Step, m_i32Depth + 1);
                
                // [rad] Build node recursively
                m_pChildren[i]->Preallocate(i32Depth - 1);                
            }
        }
    }
    
    
    
}
