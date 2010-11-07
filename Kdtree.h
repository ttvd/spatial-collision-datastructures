/*
 *  Kdtree.h
 *  SpatialTest Project
 *
 *  A Kd-tree which uses SAH (simple area heuristic).
 *
 *  Created by radix on 12/04/08.
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

#if !defined(ST_KDTREE_H)
#define ST_KDTREE_H


#include "ISpatialStructure.h"
#include "ISpatialObject.h"
#include "Vector3.h"


namespace SpatialTest
{
    class KDTreeNode;
    
    
    class KDTree : public ISpatialStructure
    {
        public:
        
            KDTree(const Vector3& refCenter, float f32HalfWidth);
            ~KDTree();
            
        public:
        
            void                                VAddObjects(const std::vector<ISpatialObject*>& refObjects);
            void                                VUpdate();
            
        protected:
        
            void                                Preallocate(int i32Depth);
            
            
        
        protected:
        
            static int                          s_i32MaxDepth;
            
            
            
        protected:
        
            KDTreeNode*                         m_pRootNode;
        
            Vector3                             m_vec3Center;
            float                               m_f32HalfWidth;
            
            std::vector<ISpatialObject*>        m_vecObjects;
    
    };



    class KDTreeNode : public ISpatialCell
    {
        friend class KDTree;
        
        
        public:
        
            KDTreeNode(KDTreeNode* pParent, int i32SplitPane);
            ~KDTreeNode();
    
        public:
            
            
            void                                            Construct(ISpatialObject* pObjectList,
                                                                        int i32ObjectCount,
                                                                        const Vector3& refVectorMin,
                                                                        const Vector3& refVectorMax);
            
            void                                            Preallocate(int i32Depth);
            
            int                                             CheckContains(ISpatialObject* pObject);
            int                                             CheckStraddle(ISpatialObject* pObject);
            
            void                                            AddObject(ISpatialObject* pObject);
            void                                            RemoveObject(ISpatialObject* pObject);
            
            void                                            CheckCollisions(ISpatialObject* pObject);
            
            void                                            Rebuild();

            ISpatialObject*                                 Invalidate();

            
        protected:
        
            static int                                      s_i32BinCount;
            
            static std::vector<std::pair<int, int> >        s_vecBins;
            static std::vector<std::pair<int, int> >        s_vecSums;
                        
            static float                                    s_f32AcceptableRatioLower;
            static float                                    s_f32AcceptableRatioUpper;
            
            
    
        protected:
        
            KDTreeNode*                                     m_pParent;
            KDTreeNode*                                     m_pChildLeft;
            KDTreeNode*                                     m_pChildRight;
            
            ISpatialObject*                                 m_pObjects;
            
            int                                             m_i32ObjectCount;
            int                                             m_i32ObjectTotal;
            
            int                                             m_i32SplitPane;
            float                                           m_f32SplitPosition;
            
            Vector3                                         m_vec3Min;
            Vector3                                         m_vec3Max;
            
            
            
    
    
    };
}


#endif //!defined(ST_KDTREE_H)