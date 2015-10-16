// 湍流游戏

#include "intersection.h"

USING_NS_CC;
USING_NS_CC_MATH;
#define EPSILON 0.000001

bool triangle_intersection( const Vec3   V1,  // Triangle vertices
                          const Vec3   V2,
                          const Vec3   V3,
                          const Vec3    O,  //Ray origin
                          const Vec3    D,  //Ray direction
                          float* out )
{
    Vec3 e1, e2;  //Edge1, Edge2
    Vec3 P, Q, T;
    float det, inv_det, u, v;
    float t;

    //Find vectors for two edges sharing V1
    e1 = V2 - V1; //SUB(e1, V2, V1);
    e2 = V3 - V1; //SUB(e2, V3, V1);

    //Begin calculating determinant - also used to calculate u parameter
    Vec3::cross(D, e2, &P); //CROSS(P, D, e2);
    //if determinant is near zero, ray lies in plane of triangle
    det = e1.dot(P);//    det = DOT(e1, P);
    //NOT CULLING
    if(det > -EPSILON && det < EPSILON) return false;
    inv_det = 1.f / det;

    //calculate distance from V1 to ray origin
    T = O - V1;//SUB(T, O, V1);

    //Calculate u parameter and test bound
    u = T.dot(P) * inv_det;//u = DOT(T, P) * inv_det;
    //The intersection lies outside of the triangle
    if(u < 0.f || u > 1.f) return false;

    //Prepare to test v parameter
    Vec3::cross(T, e1, &Q);//CROSS(Q, T, e1);

    //Calculate V parameter and test bound
    v = D.dot(Q) * inv_det;//v = DOT(D, Q) * inv_det;
    //The intersection lies outside of the triangle
    if(v < 0.f || u + v  > 1.f) return false;

    t = e2.dot(Q) * inv_det;//t = DOT(e2, Q) * inv_det;

    if(t > EPSILON) { //ray intersection
        if (out) *out = t;
        return true;
    }
    
    // No hit, no win
    return false;
}