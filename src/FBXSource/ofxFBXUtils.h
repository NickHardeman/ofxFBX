//
//  ofxFBXUtils.h
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 11/1/13.
//
//

#pragma once
#define FBXSDK_NEW_API

#include "ofMain.h"
#include <fbxsdk.h>
#include "GetPosition.h"
#include "glm/mat4x4.hpp"
#include "glm/gtx/euler_angles.hpp"

namespace ofxFBXSource {

// --- from Arturo Castro's ofxFBX -------
inline glm::vec4 fbxToOf(const FbxVector4 & avec4){
	return glm::vec4(avec4[0],avec4[1],avec4[2],avec4[3]);
}

// -----------------------------------
inline FbxVector4 toFbx(const glm::vec4 & avec4){
	return FbxVector4(avec4.x,avec4.y,avec4.z,avec4.w);
}

//// -----------------------------------
inline glm::mat4 fbxToOf(const FbxAMatrix & ainput){
    glm::mat4 m;
    for(int i=0;i<4;i++){
        m[i] = fbxToOf(ainput.GetRow(i));
    }
    return m;
}

// -----------------------------------
inline void fbxToGlmComponents( FbxAMatrix& ainput, glm::vec3& apos, glm::quat& aquat, glm::vec3& ascale ) {
    
    glm::vec3 tscale(ainput.GetS()[0], ainput.GetS()[1], ainput.GetS()[2]);
    glm::vec3 tpos(ainput.GetT()[0], ainput.GetT()[1], ainput.GetT()[2]);
    glm::quat trot( ainput.GetQ()[0], ainput.GetQ()[1], ainput.GetQ()[2], ainput.GetQ()[3] );
    
    FbxVector4 euler;// = ainput.GetQ().DecomposeSphericalXYZ();//ainput.GetR();
    euler = ainput.GetR();
    float ix = -1.f * glm::radians(euler[0]);//ofGetKeyPressed('1') ? 1.f : -1.f;
    float iy = -1.f * glm::radians(euler[1]);//ofGetKeyPressed('2') ? 1.f : -1.f;
    float iz = -1.f * glm::radians(euler[2]);//ofGetKeyPressed('3') ? 1.f : -1.f;
//    trot = toQuat(glm::eulerAngleXYZ(glm::radians(euler[0]) * ix,
//                                     glm::radians(euler[1]) * iy,
//                                     glm::radians(euler[2]) * iz));
    trot = glm::toQuat(glm::eulerAngleXYZ( ix, iy, iz ));
    
    ascale = tscale;
    apos = ( tpos );
    aquat = ( glm::conjugate(trot) );
}

// -----------------------------------
inline FbxAMatrix convertGlmToFbx( glm::vec3& apos, glm::quat& aquat, glm::vec3& ascale ) {
    FbxAMatrix fm;
    
    FbxVector4 tscale( ascale.x, ascale.y, ascale.z, 0.0 );
    FbxVector4 tpos( apos.x, apos.y, apos.z, 1.0 );
    FbxVector4 trot;
    
    glm::quat adjquat = glm::normalize(aquat);
    glm::vec3 euler = glm::eulerAngles(adjquat);
    
    trot.Set( glm::degrees(euler.x), glm::degrees(euler.y), glm::degrees(euler.z), 1.0 );
    fm.SetTRS( tpos, trot, tscale );
    return fm;
}

// -----------------------------------
inline FbxAMatrix toFbx( glm::vec3 apos, glm::quat arot, glm::vec3 ascale ) {
    return convertGlmToFbx(apos, arot, ascale);
}

// -----------------------------------
inline FbxAMatrix fbxGetGlobalTransform( FbxNode* pNode, const FbxTime& pTime, FbxPose* pPose, FbxAMatrix* pParentGlobalPosition=NULL ) {
    return GetGlobalPosition( pNode, pTime, pPose, pParentGlobalPosition);
}

// -----------------------------------
inline glm::mat4 glmGetGlobalTransform( FbxNode* pNode, const FbxTime& pTime, FbxPose* pPose, FbxAMatrix* pParentGlobalPosition=NULL ) {
    return fbxToOf( GetGlobalPosition( pNode, pTime, pPose, pParentGlobalPosition) );
}

// -----------------------------------
inline glm::mat4 glmGetLocalTransform( FbxNode* pNode, const FbxTime& pTime, FbxPose* pPose, FbxAMatrix* pParentGlobalPosition=NULL ) {
    return fbxToOf( GetLocalPositionForNode( pNode, pTime, pPose, pParentGlobalPosition) );
}
    
}







