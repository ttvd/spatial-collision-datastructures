/*
 *  Kdtree.cpp
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


#include "Kdtree.h"

#include <math.h>
#include <limits>
#include <cstdlib> 

namespace SpatialTest
{
    //--
    int KDTree::s_i32MaxDepth       = 12;




    //--
    KDTree::KDTree(const Vector3& refCenter, float f32HalfWidth) :
        ISpatialStructure(),
        m_vec3Center(refCenter),
        m_f32HalfWidth(f32HalfWidth)
    {
        m_pRootNode = new KDTreeNode(NULL, 0);
    }



    //--
    KDTree::~KDTree()
    {
        if(m_pRootNode)
        {
            delete(m_pRootNode);
        }
    }



    //--
    inline
    void
    KDTree::Preallocate(int i32Depth)
    {
        m_pRootNode->Preallocate(i32Depth);
    }



    //--
    void
    KDTree::VAddObjects(const std::vector<ISpatialObject*>& refObjects)
    {
        ISpatialObject* pObjectList = NULL;
        ISpatialObject* pObjectTemp;


        std::vector<ISpatialObject*>::const_iterator iter_object;
        for(iter_object = refObjects.begin(); iter_object != refObjects.end(); iter_object++)
        {
            pObjectTemp = (*iter_object);


            pObjectTemp->VSetNext(pObjectList);
            pObjectList = pObjectTemp;


            // [rad] Store objects locally for fast update / iteration
            m_vecObjects.push_back((*iter_object));
        }


        // [rad] Pre-allocate the tree
        Preallocate(s_i32MaxDepth);


        Vector3 vec3Min(m_vec3Center.x - m_f32HalfWidth, m_vec3Center.y - m_f32HalfWidth, m_vec3Center.z - m_f32HalfWidth);
        Vector3 vec3Max(m_vec3Center.x + m_f32HalfWidth, m_vec3Center.y + m_f32HalfWidth, m_vec3Center.z + m_f32HalfWidth);


        // [rad] Based on this info, construct recursively, starting at root
        m_pRootNode->Construct(pObjectTemp, refObjects.size(), vec3Min, vec3Max);
    }



    //--
    void
    KDTree::VUpdate()
    {
        // [rad] Remove / Insert elements

        KDTreeNode* pNode;
        ISpatialObject* pObject;
        ISpatialObject* pIter;
        ISpatialObject* pPrev;

        std::vector<ISpatialObject*>::iterator iter_object;
        for(iter_object = m_vecObjects.begin(); iter_object != m_vecObjects.end(); iter_object++)
        {
            pIter = (*iter_object);
            pNode = static_cast<KDTreeNode*>(pIter->VGetCell());

            // [rad] Check if this node still contains the element
            // Also need to make sure that object is straddling (if children exist)
            if(pNode->CheckContains(pIter) && pNode->CheckStraddle(pIter))
            {
                // [rad] If so, do nothing
                continue;
            }
            else
            {
                pNode->RemoveObject(pIter);

                /*
                // [rad] We need to insert the object
                // [rad] Go up one node
                pNode = pNode->m_pParent;
                pObject = (*iter_object);

                // [rad] Check if this node still contains this object
                while(pNode)
                {
                    // [rad] Check if contains
                    if(pNode->CheckContains(pObject))
                    {
                        // [rad] If so, add this object
                        pNode->AddObject(pObject);
                        break;
                    }
                    else
                    {
                        // [rad] If not, iterate up
                        pNode = pNode->m_pParent;
                    }
                }
                */

                m_pRootNode->AddObject(pIter);
            }
        }


        // [rad] Traverse the tree and check if any of the nodes are invalidated
        // If they are, rebuild that portion of the tree
        m_pRootNode->Rebuild();


        // [rad] Do collision detection
        // [rad] Do top-down collision testing
        for(iter_object = m_vecObjects.begin(); iter_object != m_vecObjects.end(); iter_object++)
        {
            m_pRootNode->CheckCollisions((*iter_object));
        }
    }






    //--
    int KDTreeNode::s_i32BinCount                               = 32;

    float KDTreeNode::s_f32AcceptableRatioLower                 = 0.4f;
    float KDTreeNode::s_f32AcceptableRatioUpper                 = 0.6f;

    std::vector<std::pair<int, int> > KDTreeNode::s_vecBins     = std::vector<std::pair<int, int> >(s_i32BinCount, std::pair<int, int>());
    std::vector<std::pair<int, int> > KDTreeNode::s_vecSums     = std::vector<std::pair<int, int> >(s_i32BinCount, std::pair<int, int>());



    //--
    KDTreeNode::KDTreeNode(KDTreeNode* pParent, int i32SplitPane) :
        m_pParent(pParent),
        m_pChildLeft(NULL),
        m_pChildRight(NULL),
        m_i32SplitPane(i32SplitPane),
        m_f32SplitPosition(0.0f),
        m_pObjects(NULL),
        m_i32ObjectCount(0),
        m_i32ObjectTotal(0)
    {

    }



    //--
    KDTreeNode::~KDTreeNode()
    {
        if(m_pChildLeft)
        {
            delete(m_pChildLeft);
            m_pChildLeft = NULL;
        }

        if(m_pChildRight)
        {
            delete(m_pChildRight);
            m_pChildRight = NULL;
        }
    }



    //--
    void
    KDTreeNode::Preallocate(int i32Depth)
    {
        if(i32Depth)
        {
            m_pChildLeft = new KDTreeNode(this, (m_i32SplitPane + 1) % 3);
            m_pChildLeft->Preallocate(i32Depth - 1);

            m_pChildRight = new KDTreeNode(this, (m_i32SplitPane + 1) % 3);
            m_pChildRight->Preallocate(i32Depth - 1);
        }
    }



    //--
    inline
    int
    KDTreeNode::CheckContains(ISpatialObject* pObject)
    {
        Vector3 vec3Center = pObject->VGetPosition();
        float f32Radius = pObject->VGetRadius();

        if(vec3Center.x - f32Radius >= m_vec3Min.x &&
            vec3Center.y - f32Radius >= m_vec3Min.y &&
            vec3Center.z - f32Radius >= m_vec3Min.z &&
            vec3Center.x + f32Radius <= m_vec3Max.x &&
            vec3Center.y + f32Radius <= m_vec3Max.y &&
            vec3Center.z + f32Radius <= m_vec3Max.z)
        {
            return(1);
        }

        return(0);
    }



    //--
    inline
    int
    KDTreeNode::CheckStraddle(ISpatialObject* pObject)
    {
        // [rad] If this is a leaf, return straddle, but technically
        // it doesn't matter (it's just easier for the calling procedure)
        if(!m_pChildLeft || !m_pChildRight)
        {
            return(1);
        }



        Vector3 vec3Center = pObject->VGetPosition();
        float f32Radius = pObject->VGetRadius();

        // [rad] Check if object is intersecting the splitting plane
        if(vec3Center[m_i32SplitPane] + f32Radius <= m_f32SplitPosition)
        {
            // [rad] No, object belongs to left child
            return(0);
        }
        else if(vec3Center[m_i32SplitPane] - f32Radius >= m_f32SplitPosition)
        {
            // [rad] No, object belongs to right child
            return(0);
        }


        // [rad] Otherwise object is intersecting the splitting plane
        return(1);
    }



    //--
    void
    KDTreeNode::RemoveObject(ISpatialObject* pObject)
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

                if(pObject == pIter)
                {
                    // [rad] Remove node and decrement node count
                    pPrev->VSetNext(pObject->VGetNext());

                    break;
                }
            }
        }

        // [rad] Decrement counts
        m_i32ObjectCount--;
        m_i32ObjectTotal--;

        // [rad] Traverse up and decrement parent total counts
        KDTreeNode* pNode = m_pParent;

        // [rad] Check if this node still contains this object
        while(pNode)
        {
            pNode->m_i32ObjectTotal--;

            // [rad] Go up
            pNode = pNode->m_pParent;
        }
    }



    //--
    void
    KDTreeNode::AddObject(ISpatialObject* pObject)
    {
        Vector3 vec3Center = pObject->VGetPosition();
        float f32Radius = pObject->VGetRadius();

        // [rad] Check where this object belongs
        if(m_pChildLeft && vec3Center[m_i32SplitPane] + f32Radius <= m_f32SplitPosition)
        {
            // [rad] Left child
            m_pChildLeft->AddObject(pObject);
            m_i32ObjectTotal++;
        }
        else if(m_pChildRight && vec3Center[m_i32SplitPane] - f32Radius >= m_f32SplitPosition)
        {
            // [rad] Right child
            m_pChildRight->AddObject(pObject);
            m_i32ObjectTotal++;
        }
        else
        {
            // [rad] If we are here, then either object is straddling the split plane
            // or we have no children (leaf), add to this node

            pObject->VSetCell(this);
            pObject->VSetNext(m_pObjects);
            m_pObjects = pObject;

            m_i32ObjectCount++;
            m_i32ObjectTotal++;
        }
    }



    //--
    void
    KDTreeNode::CheckCollisions(ISpatialObject* pObject)
    {
        Vector3 vec3Center = pObject->VGetPosition();
        float f32Radius = pObject->VGetRadius();

        // [rad] Check collisions against objects in this node
        if(m_i32ObjectCount)
        {
            ISpatialObject* pIter = m_pObjects;
            while(pIter)
            {
                if(pIter != pObject)
                {
                    if(pIter->VCheckCollision(pObject))
                    {
                        pIter->VCollisionOn();
                        pObject->VCollisionOn();
                    }
                }

                pIter = pIter->VGetNext();
            }
        }



        // [rad] Recurse into left child
        if(m_pChildLeft && m_pChildLeft->m_i32ObjectCount && vec3Center[m_i32SplitPane] + f32Radius <= m_f32SplitPosition)
        {
           m_pChildLeft->CheckCollisions(pObject);
        }

        if(m_pChildRight && m_pChildRight->m_i32ObjectCount && vec3Center[m_i32SplitPane] - f32Radius >= m_f32SplitPosition)
        {
            // [rad] Recurse into right child
            m_pChildRight->CheckCollisions(pObject);
        }
    }



    //--
    void
    KDTreeNode::Rebuild()
    {

        // [rad] If this is a leaf, no need to rebuild anything
        if(!m_pChildLeft || !m_pChildRight)
        {
            return;
        }

        // [rad] Get the total count of objects in children
        //float f32TotalCount = static_cast<float>(m_pChildLeft->m_i32ObjectCount + m_pChildRight->m_i32ObjectCount);

        // [rad] Get total number of objects in children nodes (ignore objects that are straddling)
        float f32TotalCount = static_cast<float>(m_i32ObjectTotal - m_i32ObjectCount);

        float f32RatioLeft = static_cast<float>(m_pChildLeft->m_i32ObjectTotal) / f32TotalCount;
        float f32RatioRight = static_cast<float>(m_pChildRight->m_i32ObjectTotal) / f32TotalCount;

        // [rad] Check if ratio of # right : # left is accetable
        if(s_f32AcceptableRatioLower <= f32RatioLeft && f32RatioLeft <= s_f32AcceptableRatioUpper &&
            s_f32AcceptableRatioLower <= f32RatioRight && f32RatioRight <= s_f32AcceptableRatioUpper)
        {
            // [rad] No need to rebuild, descend into children and check if they require rebuilding
            m_pChildLeft->Rebuild();
            m_pChildRight->Rebuild();
        }
        else
        {
            // [rad] Otherwise rebuild this node
            int i32ObjectTotal = m_i32ObjectTotal;

            // [rad] Invalidate this node and children
            ISpatialObject* pObjectList = Invalidate();

            // [rad] Reconstruct this node
            Construct(pObjectList, i32ObjectTotal, m_vec3Min, m_vec3Max);
        }

        /*
        // [rad] Otherwise rebuild this node
            int i32ObjectTotal = m_i32ObjectTotal;

            // [rad] Invalidate this node and children
            ISpatialObject* pObjectList = Invalidate();

            int i32Count = 0;
            ISpatialObject* pIter = pObjectList;
            while(pIter)
            {
                pIter = pIter->VGetNext();
                i32Count++;
            }

            // [rad] Reconstruct this node
            Construct(pObjectList, i32ObjectTotal, m_vec3Min, m_vec3Max);
        */
    }



    //--
    ISpatialObject*
    KDTreeNode::Invalidate()
    {
        ISpatialObject* pIter;
        ISpatialObject* pTemp;
        ISpatialObject* pObjects = NULL;

        int i32ObjectCount;

        // [rad] If this is a leaf
        if(!m_pChildLeft || !m_pChildRight)
        {
            pObjects = m_pObjects;
            i32ObjectCount = m_i32ObjectCount;

            m_i32ObjectCount = 0;
            m_i32ObjectTotal = 0;
            m_pObjects = NULL;

            return(pObjects);
        }


        ISpatialObject* pObjectsLeft = NULL;
        if(m_pChildLeft->m_i32ObjectTotal)
        {
            pObjectsLeft = m_pChildLeft->Invalidate();
        }

        ISpatialObject* pObjectsRight = NULL;
        if(m_pChildRight->m_i32ObjectTotal)
        {
            pObjectsRight = m_pChildRight->Invalidate();
        }



        pIter = pObjectsLeft;
        while(pIter)
        {
            pTemp = pIter->VGetNext();

            pIter->VSetNext(pObjects);
            pObjects = pIter;

            pIter = pTemp;
        }


        pIter = pObjectsRight;
        while(pIter)
        {
            pTemp = pIter->VGetNext();

            pIter->VSetNext(pObjects);
            pObjects = pIter;

            pIter = pTemp;
        }


        pIter = m_pObjects;
        while(pIter)
        {
            pTemp = pIter->VGetNext();

            pIter->VSetNext(pObjects);
            pObjects = pIter;

            pIter = pTemp;
        }


        /*
        ISpatialObject* pIterLeft = NULL;
        ISpatialObject* pIterParent = NULL;
        ISpatialObject* pTemp;


        // [rad] traverse left link list looking for end
        if(pObjectsLeft)
        {
            pIterLeft = pObjectsLeft;
            while(1)
            {
                pTemp = pIterLeft->VGetNext();

                if(!pTemp)
                {
                    break;
                }

                pIterLeft = pTemp;
            }
        }


        // [rad] traverse straddling collection (stored at this node) looking for end
        if(m_pObjects)
        {
            pIterParent = m_pObjects;
            while(1)
            {
                pTemp = pIterParent->VGetNext();

                if(!pTemp)
                {
                    break;
                }

                pIterParent = pTemp;
            }
        }


        // [rad] Glue 3 single link lists together
        if(pIterLeft && pIterParent)
        {
            pIterParent->VSetNext(pObjectsRight);
            pIterLeft->VSetNext(pIterParent);

            pObjects = pIterLeft;
        }
        else if(pIterLeft)
        {
            pIterLeft->VSetNext(pObjectsRight);

            pObjects = pIterLeft;

        }
        else if(pIterParent)
        {
            pIterParent->VSetNext(pObjectsRight);

            pObjects = pIterParent;
        }
        else
        {
            pObjects = pObjectsRight;
        }
        */

        m_i32ObjectCount = 0;
        m_i32ObjectTotal = 0;
        m_pObjects = NULL;

        return(pObjects);
    }


    //--
    void
    KDTreeNode::Construct(ISpatialObject* pObjectList, int i32ObjectCount,
                                         const Vector3& refVectorMin, const Vector3& refVectorMax)
    {

        // [rad] Store voxel info
        m_vec3Max = refVectorMax;
        m_vec3Min = refVectorMin;

        // [rad] Split plane position is already stored


        ISpatialObject* pIter = pObjectList;
        ISpatialObject* pObjectTemp;
        ISpatialObject* pObject;

        // [rad] If there are no children, or there's less objects than bins,
        // store everything in this node
        if(!m_pChildLeft || !m_pChildRight || i32ObjectCount < s_i32BinCount)
        {
            // [rad] There's no splitpane and position is not important
            m_f32SplitPosition = 0.0f;
            //m_i32SplitPane = -1;

            // [rad] Copy elements...
            m_pObjects = pObjectList;
            m_i32ObjectCount = i32ObjectCount;
            m_i32ObjectTotal = i32ObjectCount;


            // [rad] Iterate through all elements and set this node as a containing cell
            pIter = m_pObjects;
            while(pIter)
            {
                pIter->VSetCell(this);
                pIter = pIter->VGetNext();
            }

            return;
        }



        // [rad] Compute sizes
        float f32Span = m_vec3Max[m_i32SplitPane] - m_vec3Min[m_i32SplitPane];
        float f32BucketSize = f32Span / static_cast<float>(s_i32BinCount);
        float f32Offset = -m_vec3Min[m_i32SplitPane];

        int i32BinIndex;
        int i32Index;



        // [rad] Clean prefix sums and previous values
        for(i32Index = 0; i32Index < s_i32BinCount; i32Index++)
        {
            s_vecBins[i32Index].first = 0;
            s_vecBins[i32Index].second = 0;

            s_vecSums[i32Index].first = 0;
            s_vecSums[i32Index].second = 0;
        }




        // [rad] Go linearly through the list and do binning
        pIter = pObjectList;
        Vector3 vec3Center;
        float f32Radius;

        while(pIter)
        {
            // [rad] Get center and radius of this object
            vec3Center = pIter->VGetPosition();
            f32Radius = pIter->VGetRadius();

            // [rad] Increment proper bins
            i32BinIndex = static_cast<int>(floorf((f32Offset + vec3Center[m_i32SplitPane] - f32Radius) * s_i32BinCount / f32Span));
            s_vecBins[i32BinIndex].first++;

            i32BinIndex = static_cast<int>(floorf((f32Offset + vec3Center[m_i32SplitPane] + f32Radius) * s_i32BinCount / f32Span));
            s_vecBins[i32BinIndex].second++;


            // [rad] iterate to next element in list
            pIter = pIter->VGetNext();
        }




        // [rad] Compute prefix sums
        int i32SumMin = 0;
        int i32SumMax = 0;


        for(i32Index = 0; i32Index < s_i32BinCount; i32Index++)
        {
            i32SumMin += s_vecBins[i32Index].first;
            i32SumMax += s_vecBins[i32Index].second;

            s_vecSums[i32Index].first = i32SumMin;
            s_vecSums[i32Index].second = i32SumMax;
        }



        // [rad] Compute split candidate
        int i32MinDiff = std::numeric_limits<int>::max();
        int i32BinDiff;
        int i32SplitPosition = 0;

        for(i32Index = 0; i32Index < s_i32BinCount; i32Index++)
        {
            i32BinDiff = fabs((float) s_vecSums[i32Index].first - s_vecSums[s_i32BinCount - i32Index - 1].second);

            if(i32BinDiff < i32MinDiff)
            {
                // [rad] This is a good candidate
                i32MinDiff = i32BinDiff;
                i32SplitPosition = i32Index;
            }
        }



        // [rad] Find real split position
        //m_f32SplitPosition = ((i32SplitPosition * f32Span) / f32BucketSize) - f32Offset;
        m_f32SplitPosition = m_vec3Min[m_i32SplitPane] + i32SplitPosition * f32BucketSize;


        // [rad] Now go insert objects into child nodes
        ISpatialObject* pObjectListLeft = NULL;
        ISpatialObject* pObjectListRight = NULL;

        int i32CountLeft = 0;
        int i32CountRight = 0;

        pIter = pObjectList;
        while(pIter)
        {
            pObject = pIter->VGetNext();

            // [rad] Get center and radius of this object
            vec3Center = pIter->VGetPosition();
            f32Radius = pIter->VGetRadius();


            // [rad] Check where this object belongs
            if(vec3Center[m_i32SplitPane] + f32Radius <= m_f32SplitPosition)
            {
                // [rad] Left child

                pIter->VSetNext(pObjectListLeft);
                pObjectListLeft = pIter;

                i32CountLeft++;
            }
            else if(vec3Center[m_i32SplitPane] - f32Radius >= m_f32SplitPosition)
            {
                // [rad] Right child

                pIter->VSetNext(pObjectListRight);
                pObjectListRight = pIter;

                i32CountRight++;
            }
            else
            {

                pIter->VSetCell(this);
                pIter->VSetNext(m_pObjects);
                m_pObjects = pIter;

                m_i32ObjectCount++;
            }

            pIter = pObject;
        }


        // [rad] We'll keep track of how many objects we have in this node
        // and in children
        m_i32ObjectTotal += i32ObjectCount;


        Vector3 vec3Min = refVectorMin;
        vec3Min[m_i32SplitPane] = m_f32SplitPosition;

        Vector3 vec3Max = refVectorMax;
        vec3Max[m_i32SplitPane] = m_f32SplitPosition;

        // [rad] Recurse into left child
        m_pChildLeft->Construct(pObjectListLeft, i32CountLeft,
                                            refVectorMin, vec3Max);


        // [rad] Recurse into right child
        m_pChildRight->Construct(pObjectListRight, i32CountRight,
                                            vec3Min, refVectorMax);
    }
}
