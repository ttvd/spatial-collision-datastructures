/*
 *  Vector3.h
 *  SpatialTest Project
 *
 *  3 dimensional vector.
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
 
#if !defined(ST_VECTOR3_H)
#define ST_VECTOR3_H

namespace SpatialTest
{
    class Vector3
    {
        public:
        
            Vector3();
            Vector3(float f32X, float f32Y, float f32Z);
            Vector3(const Vector3& refVec);
        
            
        public:
        
            Vector3&    operator-=(const Vector3& refVector);
            Vector3&    operator+=(const Vector3& refVector);
        
            float&      operator[](unsigned int i32Index);
            
            float       Dot(const Vector3 &refVector) const;
            
            
        public:
        
            float x;
            float y;
            float z;
    
    };
    
    //--
    inline 
    Vector3::Vector3() : 
        x(0.0f), 
        y(0.0f), 
        z(0.0f)
    {
    
    }
    
    
    //--
    inline
    Vector3::Vector3(const Vector3& refVec) :
        x(refVec.x),
        y(refVec.y),
        z(refVec.z)
    {
    
    }
    

    //--
    inline
    Vector3::Vector3(float f32X, float f32Y, float f32Z) :
        x(f32X), 
        y(f32Y), 
        z(f32Z)
    {
    
    }
    
    
    //--
    inline
    float&
    Vector3::operator[](unsigned int i32Index)
    {
        switch(i32Index)
        {
            case 0:
                return(x);
                
            case 1:
                return(y);
                
            case 2:
                return(z);
        }
    }
    
    
    
    //--
    inline
    float
    Vector3::Dot(const Vector3 &refVector) const
    {
        return(x * refVector.x + y * refVector.y + z * refVector.z);
    }
    
    
    
    //--
    inline
    Vector3& 
    Vector3::operator-=(const Vector3& refVector)
    {
        x -= refVector.x;
        y -= refVector.y;
        z -= refVector.z;
        
        return(*this);
    }
    
    
    
    //--
    inline
    Vector3& 
    Vector3::operator+=(const Vector3& refVector)
    {
        x += refVector.x;
        y += refVector.y;
        z += refVector.z;
        
        return(*this);
    }

    
    
    //--
    inline
    Vector3
    operator-(const Vector3& refLeft, const Vector3& refRight)
    {
        return(Vector3(refLeft.x - refRight.x, 
                        refLeft.y - refRight.y,
                        refLeft.z - refRight.z));
    }
    
    inline
    Vector3
    operator+(const Vector3& refLeft, const Vector3& refRight)
    {
        return(Vector3(refLeft.x + refRight.x, 
                        refLeft.y + refRight.y,
                        refLeft.z + refRight.z));
    }
    
    
    inline
    Vector3
    operator*(const Vector3& refLeft, float f32Right)
    {
        return(Vector3(refLeft.x * f32Right, 
                        refLeft.y * f32Right,
                        refLeft.z * f32Right));
    }


    inline
    Vector3
    operator*(float f32Left, const Vector3& refRight)
    {
        return(Vector3(refRight.x * f32Left, 
                        refRight.y * f32Left,
                        refRight.z * f32Left));
    }


    
}

#endif //!defined(ST_VECTOR3_H)
