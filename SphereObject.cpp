/*
 *  SphereObject.cpp
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

#include "Base.h"
#include "SphereObject.h"
#include <cstdlib>

namespace SpatialTest
{
    //--
    SphereObject::SphereObject(int i32Id) :
        ISpatialObject(),
        m_i32Id(i32Id),
        m_pSpatialCell(NULL),
        m_i32Collision(0),
        m_pNextObject(NULL),
        m_i32Level(0)
    {
    
    }
     
        
    
    //--
    void
    SphereObject::VSetLevel(int i32Level)
    {
        m_i32Level = i32Level;
    }
    
    
    
    //--
    int
    SphereObject::VGetLevel() const
    {
        return(m_i32Level);
    }
    
    
    
    //--
    void
    SphereObject::VCollisionOn()
    {
        m_i32Collision = 1;
    }
    
    
    
    //--
    void
    SphereObject::VCollisionOff()
    {
        m_i32Collision = 0;
    }
    
    
    
    //--
    int
    SphereObject::VGetCollisionStatus() const
    {
        return(m_i32Collision);
    }
    
    
            
    //--
    int
    SphereObject::VCheckCollision(const ISpatialObject* pObject) const
    {
        Vector3 vec3Temp = m_vec3Position - pObject->VGetPosition();
        
        float f32DistanceSq = vec3Temp.Dot(vec3Temp);
        float f32RadiusSum = m_f32Radius + pObject->VGetRadius();
        
        return(f32DistanceSq <= f32RadiusSum * f32RadiusSum);
    }
     
        
    
    //--
    void
    SphereObject::VSetPosition(const Vector3& refPosition)
    {
        m_vec3Position = refPosition;
    }
    
    
    
    //--
    const Vector3&
    SphereObject::VGetPosition() const
    {
        return(m_vec3Position);
    }
    
    
    
    //--
    void
    SphereObject::VSetDirection(const Vector3& refDirection)
    {
        m_vec3Direction = refDirection;
    }
    
    
    
    //--
    const Vector3&
    SphereObject::VGetDirection() const
    {
        return(m_vec3Direction);
    }
    
    
            
    //--
    void
    SphereObject::VSetRadius(float f32Radius)
    {
        m_f32Radius = f32Radius;
    }
    
    
    
    //--
    float
    SphereObject::VGetRadius() const
    {
        return(m_f32Radius);
    }
    
    
    
    //--
    int
    SphereObject::VGetId() const
    {
        return(m_i32Id);
    }
    
    
    
    //--
    void
    SphereObject::VSetCell(ISpatialCell* pCell)
    {
        m_pSpatialCell = pCell;
    }
    
    
    
    //--
    ISpatialCell*
    SphereObject::VGetCell()
    {
        return(m_pSpatialCell);
    }
            
            
    //--
    ISpatialObject*
    SphereObject::VGetNext()
    {
        return(m_pNextObject);
    }
    
    
    //--
    void
    SphereObject::VSetNext(ISpatialObject* pObject)
    {
        m_pNextObject = pObject;
    }
            
            
}