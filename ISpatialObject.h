/*
 *  ISpatialObject.h
 *  SpatialTest Project
 *
 *  Interface for all objects that wish to be stored in ISpatialStructure derived datastructures.
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

#if !defined(ST_INTERFACE_SPATIALOBJECT_H)
#define ST_INTERFACE_SPATIALOBJECT_H

#include "ISpatialCell.h"
#include "Vector3.h"

namespace SpatialTest
{
    class ISpatialObject
    {
        public:
            
            virtual ~ISpatialObject() {}
            
        public:
        
            virtual int                 VCheckCollision(const ISpatialObject* pObject) const = 0;
            
            virtual void                VCollisionOn() = 0;
            virtual void                VCollisionOff() = 0;
            virtual int                 VGetCollisionStatus() const = 0;
        
            virtual void                VSetPosition(const Vector3& refPosition) = 0;
            virtual const Vector3&      VGetPosition() const = 0;
            
            virtual void                VSetDirection(const Vector3& refDirection) = 0;
            virtual const Vector3&      VGetDirection() const = 0;
            
            virtual void                VSetRadius(float f32Radius) = 0;
            virtual float               VGetRadius() const = 0;
            
            virtual int                 VGetId() const = 0;
            
            virtual void                VSetCell(ISpatialCell* pCell) = 0;
            virtual ISpatialCell*       VGetCell() = 0;
            
            virtual ISpatialObject*     VGetNext() = 0;
            virtual void                VSetNext(ISpatialObject* pObject) = 0;
            
            virtual void                VSetLevel(int i32Level) = 0;
            virtual int                 VGetLevel() const = 0;
            
    };
}

#endif //!defined(ST_INTERFACE_SPATIALOBJECT_H)
