/*
 *  SphereObject.h
 *  SpatialTest Project
 *
 *  Sphere Object.
 *
 *  Created by radix on 09/04/08.
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

#if !defined(ST_SPHEREOBJECT_H)
#define ST_SPHEREOBJECT_H

#include "ISpatialObject.h"

namespace SpatialTest
{
    class ISpatialCell;
    
    class SphereObject : public ISpatialObject
    {
        public:
            
            SphereObject(int i32Id);
            
        
        public:
        
            void                        SetRadius(float f32Radius);
        
        
        public:
        
            int                         VCheckCollision(const ISpatialObject* pObject) const;
        
            void                        VSetPosition(const Vector3& refPosition);
            const Vector3&              VGetPosition() const;
            
            void                        VSetDirection(const Vector3& refDirection);
            const Vector3&              VGetDirection() const;

            void                        VSetRadius(float f32Radius);
            float                       VGetRadius() const;
            
            int                         VGetId() const;
            
            void                        VSetCell(ISpatialCell* pCell);
            ISpatialCell*               VGetCell();
            
            void                        VCollisionOn();
            void                        VCollisionOff();
            int                         VGetCollisionStatus() const;
            
            ISpatialObject*             VGetNext();
            void                        VSetNext(ISpatialObject* pObject);
            
            void                        VSetLevel(int i32Level);
            int                         VGetLevel() const;
            
        protected:
        
            int                         m_i32Id;
            int                         m_i32Collision;
            
            int                         m_i32Level;
            
            float                       m_f32Radius;
            
            
            Vector3                     m_vec3Position;
            Vector3                     m_vec3Direction;
            
            ISpatialCell*               m_pSpatialCell;
            
            ISpatialObject*             m_pNextObject;
    
    };
};

#endif //!defined(ST_SPHEREOBJECT_H)