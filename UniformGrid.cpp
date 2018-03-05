/*
 *  UniformGrid.cpp
 *  SpatialTest Project
 *
 *  Uniform Grid Datastructure.
 *
 *  Created by radix on 08/04/08.
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

#include "UniformGrid.h"

// [rad] We need floorf, ceilf
#include <math.h>
#include <cstdlib>

namespace SpatialTest
{
  
    //--
    float UniformGrid::s_f32ObjectCellRatio = 4.0f;
    float UniformGrid::s_f32Epsilon = 0.0005f;
    
    
    
     
    //--
    unsigned int
    UniformGrid::ComputeHashValue(int i32HashBuckets, int i32X, int i32Y, int i32Z)
    {
        // [rad] Here we pick some large primes
        const int i32Prime1 = 563300407;
        const int i32Prime2 = 495250453;
        const int i32Prime3 = 236350427;
        
        // [rad] Compute hash value
        int i32Hash = i32Prime1 * i32X + i32Prime2 * i32Y + i32Prime3 * i32Z;
        
        // [rad] Find the corresponding bucket
        i32Hash %= i32HashBuckets;
        
        if(i32Hash < 0)
        {
            i32Hash += i32HashBuckets;
        }
        
        return(i32Hash);
    }
    
    
    
    //--
    UniformGrid::UniformGrid(int i32HashBuckets) :
        ISpatialStructure(),
        m_i32HashBuckets(i32HashBuckets),
        m_i32FrameCount(0),
        m_f32GridSize(1.0f)
    {
        UniformGridHashBucket* pBucket;
        
        // [rad] Initialize hashing buckets
        for(int i32Index = 0; i32Index < m_i32HashBuckets; i32Index++)
        {
            // [rad] Create new bucket
            pBucket = new UniformGridHashBucket();
                        
            // [rad] Store this bucket
            m_vecHashBuckets.push_back(pBucket);
        }
    }
    
    
    
    //--
    UniformGrid::~UniformGrid()
    {
        UniformGridHashBucket* pBucket;
        
        // [rad] Delete all buckets
        std::vector<UniformGridHashBucket*>::iterator iter_bucket;
        for(iter_bucket = m_vecHashBuckets.begin(); iter_bucket != m_vecHashBuckets.end(); iter_bucket++)
        {
            pBucket = (*iter_bucket);
            
            delete(pBucket);
            pBucket = NULL;
        }
    }
    
    
    
    //--
    void
    UniformGrid::VAddObjects(const std::vector<ISpatialObject*>& refObjects)
    {
        // [rad] We need to determine the size of the grid (size of each cell)
        int i32Hash;
        float f32Diameter;
        
        ISpatialObject* pObject;
        
        // [rad] Iterate through all objects, and find the biggest diameter
        std::vector<ISpatialObject*>::const_iterator iter_object;
        for(iter_object = refObjects.begin(); iter_object != refObjects.end(); iter_object++)
        {
            pObject = (*iter_object);
            
            f32Diameter = pObject->VGetRadius() * 2.0f;
            
            if(f32Diameter > m_f32GridSize)
            {
                // [rad] Found a bigger object, update
                m_f32GridSize = f32Diameter;
            }
            
            // [rad] We also locally store object pointers, so we could do a bulk update
            m_vecObjects.push_back(pObject);
        }
        
        
        // [rad] We'll make it so that cell's is 'x' times as big as the largest object
        m_f32GridSize *= s_f32ObjectCellRatio;

        
        // [rad] For each object, determine the cell and insert
        for(iter_object = refObjects.begin(); iter_object != refObjects.end(); iter_object++)
        {
            pObject = (*iter_object);
            
            // [rad] Compute hash for this object - we use object's center to
            // determine the proper cell, and then hash that value
            i32Hash = ComputeHashValue(m_i32HashBuckets, 
                        static_cast<int>(pObject->VGetPosition().x / m_f32GridSize),
                        static_cast<int>(pObject->VGetPosition().y / m_f32GridSize),
                        static_cast<int>(pObject->VGetPosition().z / m_f32GridSize));
                        
            
            // [rad] Insert at the end of this bucket
            (m_vecHashBuckets[i32Hash])->InsertObject(pObject);
        }
    }
    
 
    
    
    //--
    void
    UniformGrid::VUpdate()
    {
        int i32X1, i32X2;
        int i32Y1, i32Y2;
        int i32Z1, i32Z2;
        
        int i32Hash;
        
        float f32Delta;
        
        UniformGridHashBucket* pBucket;        
                
        std::vector<ISpatialObject*>::iterator iter_object;
        for(iter_object = m_vecObjects.begin(); iter_object != m_vecObjects.end(); iter_object++)
        {
            ISpatialObject* pObject = (*iter_object);
            
            // [rad] Retrieve the bucket in which this object is stored
            pBucket = static_cast<UniformGridHashBucket*>(pObject->VGetCell());
            
            
            
            // [rad] Retrieve new object position
            const Vector3& vec3Position = pObject->VGetPosition();
                    
            // [rad] Check if we need bucket update (compute hash)
            i32Hash = ComputeHashValue(m_i32HashBuckets, 
                        static_cast<int>(vec3Position.x / m_f32GridSize),
                        static_cast<int>(vec3Position.y / m_f32GridSize),
                        static_cast<int>(vec3Position.z / m_f32GridSize));
                        
            
            // [rad] Check if we need to switch buckets
            if(m_vecHashBuckets[i32Hash] != pBucket)
            {
                // [rad] Remove from old bucket
                pBucket->RemoveObject(pObject);
                
                // [rad] Add to new (other) bucket
                (m_vecHashBuckets[i32Hash])->InsertObject(pObject);
            } 
            
            
            // [rad] Update frame
            ++m_i32FrameCount;
        
    
            // [rad] Check collisions within the current bucket
            pBucket->CheckCollisions(m_i32FrameCount, pObject);
            
                        
            // [rad] Now we need to check adjacent buckets:
            // compute all cells which this object might overlap.
            // Because initially we picked size for our cells big enough to
            // encompass any object, we have to check at most 8 cells (3D).
            
            f32Delta = pObject->VGetRadius() + m_f32GridSize / s_f32ObjectCellRatio + s_f32Epsilon;
            
            i32X1 = static_cast<int>(floorf((vec3Position.x - f32Delta) / m_f32GridSize));
            i32X2 = static_cast<int>(ceilf((vec3Position.x + f32Delta) / m_f32GridSize));
            i32Y1 = static_cast<int>(floorf((vec3Position.y - f32Delta) / m_f32GridSize));
            i32Y2 = static_cast<int>(ceilf((vec3Position.y + f32Delta) / m_f32GridSize));
            i32Z1 = static_cast<int>(floorf((vec3Position.z - f32Delta) / m_f32GridSize));
            i32Z2 = static_cast<int>(ceilf((vec3Position.z + f32Delta) / m_f32GridSize));
            
            // [rad] Check all grid cells
            for(int i32XIndex = i32X1; i32XIndex <= i32X2; i32XIndex++)
            {
                for(int i32YIndex = i32Y1; i32YIndex <= i32Y2; i32YIndex++)
                {
                    for(int i32ZIndex = i32Z1; i32ZIndex <= i32Z2; i32ZIndex++)
                    {
                        i32Hash = ComputeHashValue(m_i32HashBuckets, i32XIndex, i32YIndex, i32ZIndex);
                        
                        // [rad] Check if we have checked this bucket already
                        if(m_vecHashBuckets[i32Hash]->GetLastFrame() == m_i32FrameCount)
                        {
                            continue;
                        }
                        
                        // [rad] Otherwise check collisions
                        (m_vecHashBuckets[i32Hash])->CheckCollisions(m_i32FrameCount, pObject);
                    }
                }
            }
        }
    }
    
    
    
    
        
    //--
    UniformGridHashBucket::UniformGridHashBucket() :
        ISpatialCell(),
        m_i32LastFrame(0),
        m_i32ObjectCount(0),
        m_pObjects(NULL)
    {
    
    }
    
    
    //--
    inline 
    int
    UniformGridHashBucket::GetLastFrame() const
    {
        return(m_i32LastFrame);
    }
    
    
    //--
    void
    UniformGridHashBucket::InsertObject(ISpatialObject* pObject)
    {
        // [rad] Set owner, for fast reverse look-up
        pObject->VSetCell(this);
        
        // [rad] Store object in this bucket
        //m_vecObjects.push_back(pObject);
        
        pObject->VSetNext(m_pObjects);
        m_pObjects = pObject;
    }
    
    
    //--
    void
    UniformGridHashBucket::RemoveObject(ISpatialObject* pObject)
    {
        /*
        // [rad] Iterate over all objects in this bucket
        std::vector<ISpatialObject*>::iterator iter_object;
        for(iter_object = m_vecObjects.begin(); iter_object != m_vecObjects.end(); iter_object++)
        {
            if(pObject == (*iter_object))
            {
                // [rad] Remove this object
                m_vecObjects.erase(iter_object);
                return;
            }
        }
        */

        if(m_pObjects == pObject)
        {
            m_pObjects = m_pObjects->VGetNext();
        }
        else
        {
            // [rad] traverse list and remove
            ISpatialObject* pIter = m_pObjects;
            ISpatialObject* pPrev;
                    
            while(pIter)
            {
                pPrev = pIter;
                pIter = pIter->VGetNext();
                        
                if(pIter == pObject)
                {
                    pPrev->VSetNext(pIter->VGetNext());
                    break;
                }
            }
        }

    }
    
    
    //--
    void
    UniformGridHashBucket::CheckCollisions(int i32LastFrame, ISpatialObject* pObject)
    {        
        // [rad] Update timestamp
        m_i32LastFrame = i32LastFrame;
        
        
        /*
        // [rad] Iterate over all objects in this bucket
        std::vector<ISpatialObject*>::iterator iter_object;
        for(iter_object = m_vecObjects.begin(); iter_object != m_vecObjects.end(); iter_object++)
        {
            // [rad] Skip self
            if(pObject != (*iter_object))
            {
                // [rad] Check and report collision if there's one..
                if((*iter_object)->VCheckCollision(pObject))
                {
                    // [rad] Mark both as in collision
                    pObject->VCollisionOn();
                    (*iter_object)->VCollisionOn();
                }

            }
        }
        */
        
        // [rad] Iterate over all objects in this bucket
        ISpatialObject* pIter = m_pObjects;
        while(pIter)
        {
            if(pIter != pObject)
            {
                if(pIter->VCheckCollision(pObject))
                {
                    pObject->VCollisionOn();
                    pIter->VCollisionOn();
                }
            }
            
            pIter = pIter->VGetNext();
        }
    }
    
}