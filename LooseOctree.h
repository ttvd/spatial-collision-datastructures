/*
 *  LooseOctree.h
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
 
#if !defined(ST_LOOSEOCTREE_H)
#define ST_LOOSEOCTREE_H

#include "ISpatialStructure.h"
#include "Vector3.h"

#include <list>

namespace SpatialTest
{
    class LooseOctreeNode;
    
    class LooseOctree : public ISpatialStructure
    {
        public:
            
            LooseOctree(const Vector3& refCenter, float f32HalfWidth, int i32Rebuild = 0);
            ~LooseOctree();
            
        public:
        
            void                Preallocate(int i32Depth);
            
        public:
            
            void                VAddObjects(const std::vector<ISpatialObject*>& refObjects);
            void                VUpdate();
            
        
        protected:
        
            static int                              s_i32MaxDepth;
            static float                            s_f32MaxObjectNodeRatio;
            
            
        protected:
        
        
            LooseOctreeNode*                        m_pRootNode;
            int                                     m_i32Rebuild;
            float                                   m_f32HalfWidth;
            
            std::vector<ISpatialObject*>            m_vecObjects;
            
            

    };
    
    
    
    class LooseOctreeNode : public ISpatialCell
    {      
        friend class LooseOctree;
        
        public:
        
            LooseOctreeNode(LooseOctreeNode* pParent, const Vector3& refCenter, float f32HalfWidth, int i32Depth);
            ~LooseOctreeNode();
            
            
        public:
        
            int                             CheckContains(ISpatialObject* pObject);
            int                             CheckContainsNonLoose(ISpatialObject* pObject);
            int                             CheckBoundaries(ISpatialObject* pObject);
            
            void                            AddObject(ISpatialObject* pObject);
                        
            bool                            IsLeaf() const;
            
            void                            Preallocate(int i32Depth);
            void                            Rebuild();
            
            void                            CheckCollisions(ISpatialObject* pObject);
            
            void                            Free();
            
            
        protected:
        
            
            
        protected:
        
            LooseOctreeNode*                m_pParent;
        
            // [rad] Children of this node
            LooseOctreeNode*                m_pChildren[8];
            
            // [rad] All objects stored at this node
            ISpatialObject*                 m_pObjects;
                    
            // [rad] Center of this node
            Vector3                         m_vec3Center;
                    
            // [rad] Half Width of this node
            float                           m_f32HalfWidth;
            
            // [rad] Depth of this node
            int                             m_i32Depth;
            
            
    };

}

#endif //!defined(ST_LOOSEOCTREE_H)

