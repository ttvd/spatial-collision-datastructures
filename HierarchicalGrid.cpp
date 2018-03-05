/*
 *  HierarchicalGrid.cpp
 *  SpatialTest Project
 *
 *  Hierarchical Grid Datastructure.
 *
 *  Created by radix on 13/04/08.
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

#include "HierarchicalGrid.h"

#include <limits>
#include <math.h>
#include <cstdlib> 

namespace SpatialTest
{
    //--
    // [rad] Largest object in cell is 1/4 cell's size
    float HierarchicalGrid::s_f32ObjectCellRatio        = 4.0f;
    
    // [rad] Cells on next level are twice the size
    float HierarchicalGrid::s_f32CellGrowth             = 4.0f;
    
    // [rad] Epsilon value for checks
    float HierarchicalGrid::s_f32Epsilon                = 0.0005f;
    
    
    
    //--
    unsigned int
    HierarchicalGrid::ComputeHashValue(int i32HashBuckets, int i32X, int i32Y, int i32Z, int i32Level)
    {
        // [rad] Here we pick some large primes
        const int i32Prime1 = 563300407;
        const int i32Prime2 = 495250453;
        const int i32Prime3 = 236350427;
        const int i32Prime4 = 153950359;
        
        // [rad] Compute hash value
        int i32Hash = i32Prime1 * i32X + i32Prime2 * i32Y + i32Prime3 * i32Z + i32Prime4  * i32Level;
        
        // [rad] Find the corresponding bucket
        i32Hash %= i32HashBuckets;
        
        if(i32Hash < 0)
        {
            i32Hash += i32HashBuckets;
        }
        
        return(i32Hash);
    }



    //--
    HierarchicalGrid::HierarchicalGrid(int i32HashBuckets) :
        ISpatialStructure(),
        m_i32FrameCount(0),
        m_i32HashBuckets(i32HashBuckets),
        m_i32Layers(1)
    {
        HierarchicalGridHashBucket* pBucket;
        
        // [rad] Initialize hashing buckets
        for(int i32Index = 0; i32Index < m_i32HashBuckets; i32Index++)
        {
            // [rad] Create new bucket
            pBucket = new HierarchicalGridHashBucket();
                        
            // [rad] Store this bucket
            m_vecHashBuckets.push_back(pBucket);
        }
        
        
        m_f32CellSizeMin = std::numeric_limits<float>::max();
        m_f32CellSizeMax = std::numeric_limits<float>::min();
    }
    
    
    
    //--
    HierarchicalGrid::~HierarchicalGrid()
    {
        HierarchicalGridHashBucket* pBucket;
        
        // [rad] Delete all buckets
        std::vector<HierarchicalGridHashBucket*>::iterator iter_bucket;
        for(iter_bucket = m_vecHashBuckets.begin(); iter_bucket != m_vecHashBuckets.end(); iter_bucket++)
        {
            pBucket = (*iter_bucket);
            
            delete(pBucket);
            pBucket = NULL;
        }
    }
    
    
    
    //--
    void
    HierarchicalGrid::VAddObjects(const std::vector<ISpatialObject*>& refObjects)
    {
        int i32Hash;
        float f32Diameter;
        
        ISpatialObject* pObject;
        
        // [rad] Iterate through all objects, and find smallest and biggest diameters
        std::vector<ISpatialObject*>::const_iterator iter_object;
        for(iter_object = refObjects.begin(); iter_object != refObjects.end(); iter_object++)
        {
            pObject = (*iter_object);
            
            f32Diameter = pObject->VGetRadius() * 2.0f;
            
            if(f32Diameter > m_f32CellSizeMax)
            {
                // [rad] Found a bigger object, update
                m_f32CellSizeMax = f32Diameter;
            }
            
            if(f32Diameter < m_f32CellSizeMin)
            {
                // [rad] Found a smaller object, update
                m_f32CellSizeMin = f32Diameter;
            }
            
            // [rad] We also locally store object pointers, so we could do a bulk update
            m_vecObjects.push_back(pObject);
        }


        // [rad] We'll make it so that cell's is 'x' times as big as the smallest/largest diameter
        m_f32CellSizeMin *= s_f32ObjectCellRatio;
        m_f32CellSizeMax *= s_f32ObjectCellRatio;        
    
    
        // [rad] Now compute how many levels we require
        
        int i32Levels = 0;
        float f32GridSize = m_f32CellSizeMin;
        
        while(f32GridSize <= m_f32CellSizeMax)
        {
            f32GridSize *= s_f32CellGrowth;

            ++i32Levels;
        }
        
        
        // [rad] Store the number of layers
        m_i32Layers = i32Levels + 1;
        
        
        // [rad] Initialize layer counts
        m_vecLayerCounts = std::vector<int>(m_i32Layers, 0);
    
        
        
        
        // [rad] Add each object into the hierarchical grid
        for(iter_object = refObjects.begin(); iter_object != refObjects.end(); iter_object++)
        {
            AddObject(*iter_object);
        }
    }
    
    
    
    //--
    void
    HierarchicalGrid::VUpdate()
    {   
        int i32Level;
        int i32Hash;
        
        HierarchicalGridHashBucket* pBucket;
        ISpatialObject* pObject;
        
        float f32Diameter;
        float f32Size;
        float f32Delta;
        
        int i32X1, i32X2;
        int i32Y1, i32Y2;
        int i32Z1, i32Z2;
        
        std::vector<ISpatialObject*>::iterator iter_object;
        for(iter_object = m_vecObjects.begin(); iter_object != m_vecObjects.end(); iter_object++)
        {
            pObject = (*iter_object);
            
            // [rad] Retrieve the bucket in which this object is stored
            pBucket = static_cast<HierarchicalGridHashBucket*>(pObject->VGetCell());
            
        
            // [rad] Retrieve new object position
            const Vector3& vec3Position = pObject->VGetPosition();
        
        
            f32Diameter = pObject->VGetRadius() * 2.0f;
            f32Size = m_f32CellSizeMin;    
            
            // [rad] Find the lowest layer where object fully fits
            for(i32Level = 0; f32Size / s_f32ObjectCellRatio < f32Diameter; i32Level++)
            {
                f32Size *= s_f32CellGrowth;
            }
        
        
            // [rad] Check if we need bucket update
            i32Hash = ComputeHashValue(m_i32HashBuckets, 
                        static_cast<int>(pObject->VGetPosition().x / f32Size),
                        static_cast<int>(pObject->VGetPosition().y / f32Size),
                        static_cast<int>(pObject->VGetPosition().z / f32Size),
                        i32Level);
                        
                        
            if(m_vecHashBuckets[i32Hash] != pBucket)
            {
                pBucket->RemoveObject(pObject);
            
                // [rad] Add to new (other) bucket
                (m_vecHashBuckets[i32Hash])->InsertObject(pObject);
            }
        
        
            // [rad] Update frame
            ++m_i32FrameCount;
            
            
            // [rad] Check collisions within the current bucket
            pBucket->CheckCollisions(m_i32FrameCount, pObject);
            
            
            // [rad] Can probably simplify here - start at required layer
            
            // [rad] Go through layers
            f32Size = m_f32CellSizeMin;
            for(i32Level = 0; i32Level < m_i32Layers; i32Level++)
            {
                // [rad] Check if there are objects at this level
                if(m_vecLayerCounts[i32Level])
                {
                    f32Delta = pObject->VGetRadius() + f32Size / s_f32ObjectCellRatio + s_f32Epsilon;
                    //f32Delta = pObject->VGetRadius();
                
                    i32X1 = static_cast<int>(floorf((vec3Position.x - f32Delta) / f32Size));
                    i32X2 = static_cast<int>(ceilf((vec3Position.x + f32Delta) / f32Size));
                    i32Y1 = static_cast<int>(floorf((vec3Position.y - f32Delta) / f32Size));
                    i32Y2 = static_cast<int>(ceilf((vec3Position.y + f32Delta) / f32Size));
                    i32Z1 = static_cast<int>(floorf((vec3Position.z - f32Delta) / f32Size));
                    i32Z2 = static_cast<int>(ceilf((vec3Position.z + f32Delta) / f32Size));
            
                    // [rad] Check all grid cells
                    for(int i32XIndex = i32X1; i32XIndex <= i32X2; i32XIndex++)
                    {
                        for(int i32YIndex = i32Y1; i32YIndex <= i32Y2; i32YIndex++)
                        {
                            for(int i32ZIndex = i32Z1; i32ZIndex <= i32Z2; i32ZIndex++)
                            {
                                i32Hash = ComputeHashValue(m_i32HashBuckets, i32XIndex, i32YIndex, i32ZIndex, i32Level);
                        
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
                
                f32Size *= s_f32CellGrowth;
            }
            
        }

    
    }



    //--
    void
    HierarchicalGrid::AddObject(ISpatialObject* pObject)
    {
        int i32Level;
        float f32Diameter = pObject->VGetRadius() * 2.0f;
        float f32Size = m_f32CellSizeMin;    
            
        // [rad] Find the lowest layer where object fully fits and insert it
        // there
        for(i32Level = 0; f32Size / s_f32ObjectCellRatio < f32Diameter; i32Level++)
        {
            f32Size *= s_f32CellGrowth;
        }
            
            
            
        // [rad] Compute hash for this object - we use object's center to
        // determine the proper cell, and then hash that value.
        // Treat layer info as an additonal hashing parameter
        int i32Hash = ComputeHashValue(m_i32HashBuckets, 
                        static_cast<int>(pObject->VGetPosition().x / f32Size),
                        static_cast<int>(pObject->VGetPosition().y / f32Size),
                        static_cast<int>(pObject->VGetPosition().z / f32Size),
                        i32Level);
                        
            
        // [rad] Insert at the end of this bucket
        (m_vecHashBuckets[i32Hash])->InsertObject(pObject);
        
        // [rad] Set proper level
        //pObject->VSetLevel(i32Level);
            
        // [rad] Increment count for this level
        m_vecLayerCounts[i32Level]++;
        
    }
        
            
            

    //--
    HierarchicalGridHashBucket::HierarchicalGridHashBucket() :
        ISpatialCell(),
        m_i32LastFrame(0),
        m_i32ObjectCount(0),
        m_pObjects(NULL)
    {
    
    }



    //--
    inline 
    int
    HierarchicalGridHashBucket::GetLastFrame() const
    {
        return(m_i32LastFrame);
    }
    
    
    //--
    void
    HierarchicalGridHashBucket::InsertObject(ISpatialObject* pObject)
    {
        // [rad] Set owner, for fast reverse look-up
        pObject->VSetCell(this);
        
        // [rad] Store object in this bucket        
        pObject->VSetNext(m_pObjects);
        m_pObjects = pObject;
    }
    
    
    //--
    void
    HierarchicalGridHashBucket::RemoveObject(ISpatialObject* pObject)
    {
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
    HierarchicalGridHashBucket::CheckCollisions(int i32LastFrame, ISpatialObject* pObject)
    {        
        // [rad] Update timestamp
        m_i32LastFrame = i32LastFrame;
                
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