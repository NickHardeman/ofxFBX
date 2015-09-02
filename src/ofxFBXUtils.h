//
//  ofxFBXUtils.h
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 11/1/13.
//
//

#pragma once
#define FBXSDK_NEW_API
//#if defined(TARGET_LINUX) && !defined(TARGET_OPENGLES)
//    #include <fbxsdk.h>
//#endif
//
//#if defined(TARGET_OSX)
//    #include "fbxsdk.h"
//#endif
#include "ofMain.h"
#include <fbxsdk.h>
#include "GetPosition.h"

// --- from Arturo Castro's ofxFBX -------
inline ofVec4f toOf(const FbxVector4 & vec4){
	return ofVec4f(vec4[0],vec4[1],vec4[2],vec4[3]);
}

// -----------------------------------
inline FbxVector4 toFbx(const ofVec4f & vec4){
	return FbxVector4(vec4.x,vec4.y,vec4.z,vec4.w);
}

// -----------------------------------
inline ofMatrix4x4 toOf(const FbxAMatrix & matrix){
	ofMatrix4x4 m;
	for(int i=0;i<4;i++){
		m._mat[i] = toOf(matrix.GetRow(i));
	}
	return m;
}

// -----------------------------------
inline FbxAMatrix toFbx(const ofMatrix4x4& matrix){
	FbxAMatrix m;
	for(int i=0;i<4;i++){
        m.SetRow(i, toFbx( matrix.getRowAsVec4f(i)) );
	}
	return m;
}


// -----------------------------------
inline ofMatrix4x4 ofGetGlobalTransform( FbxNode* pNode, const FbxTime& pTime, FbxPose* pPose, FbxAMatrix* pParentGlobalPosition=NULL ) {
    return toOf( GetGlobalPosition( pNode, pTime, pPose, pParentGlobalPosition) );
}

// -----------------------------------
inline ofMatrix4x4 ofGetLocalTransform( FbxNode* pNode, const FbxTime& pTime, FbxPose* pPose, FbxAMatrix* pParentGlobalPosition=NULL ) {
    return toOf( GetLocalPositionForNode( pNode, pTime, pPose, pParentGlobalPosition) );
}






