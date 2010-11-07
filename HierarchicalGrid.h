/*
 *  HierarchicalGrid.h
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

#if !defined(ST_HIERARCHICALGRID_H)
#define ST_HIERARCHICALGRID_H

#include "ISpatialStructure.h"
#include "ISpatialCell.h"

namespace SpatialTest
{
    class HierarchicalGridHashBucket;
    

    
    class HierarchicalGrid : public ISpatialStructure
    {
        public:
        
            HierarchicalGrid(int i32HashBuckets = 1024);
            ~HierarchicalGrid();
            
        public:
        
        
            void                                        VAddObjects(const std::vector<ISpatialObject*>& refObjects);
            void                                        VUpdate();       
            
            
        protected:
        
            void                                        AddObject(ISpatialObject* pObject);


            
        protected:
                
            static unsigned int                         ComputeHashValue(int i32HashBuckets, int i32X, int i32Y, int i32Z, int i32Level);
            
            
            
        protected:

            static float                                s_f32ObjectCellRatio;
            static float                                s_f32CellGrowth;
            static float                                s_f32Epsilon;
            
            
            
            
        protected:
        
            float                                       m_f32CellSizeMin;
            float                                       m_f32CellSizeMax;
            
            int                                         m_i32HashBuckets;
            int                                         m_i32FrameCount;
            
            int                                         m_i32Layers;
            
            std::vector<HierarchicalGridHashBucket*>    m_vecHashBuckets;
            std::vector<ISpatialObject*>                m_vecObjects;
            
            std::vector<int>                            m_vecLayerCounts;
    
    };
    
    
    
    class HierarchicalGridHashBucket : public ISpatialCell
    {
        public:

            HierarchicalGridHashBucket();
            
        public:
            
            void                                        InsertObject(ISpatialObject* pObject);
            void                                        RemoveObject(ISpatialObject* pObject);
            
            void                                        CheckCollisions(int i32LastFrame, ISpatialObject* pObject);
            
            int                                         GetLastFrame() const;

    
        
        protected:
        
            int                                         m_i32LastFrame;
            int                                         m_i32ObjectCount;
            
            ISpatialObject*                             m_pObjects;
    
    };
}

#endif //!defined(ST_HIERARCHICALGRID_H)
