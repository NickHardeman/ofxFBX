//
//  ofxFBXBone.h
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 11/1/13.
//
//

#pragma once
#include "ofMain.h"

#include <fbxsdk.h>

/*
#if defined(TARGET_LINUX) 
    #include <fbxsdk.h>
#else
	#include "fbxsdk.h"
#endif
*/
#include "ofxFBXNode.h"


// note: Bones are called clusters when referring to them from an FbxMesh in the FBX SDK
class ofxFBXBone : public ofxFBXNode {
public:
    ofxFBXBone();
    ~ofxFBXBone();
    
    bool doesExist();
    
    void setup( FbxNode* pNode );
    void cacheStartTransforms();
    void update( FbxTime& pTime, FbxPose* pPose );
    void draw( float aLen = 6.f);
    
    void reset();
    void onPositionChanged();
	void onOrientationChanged();
	void onScaleChanged();
    
    bool isLimb();
    bool hasSkeletonParent();
    
    void enableExternalControl();
    void disableExternalControl();
    bool isExternalControlEnabled();
    
    void enableAnimation();
    void disableAnimation();
    bool isAnimationEnabled();
    
    void updateFbxTransform();
    void updateFbxTransformLocal();
    ofQuaternion getOriginalLocalRotation();
    ofVec3f getOriginalLocalPosition();
    
    FbxSkeleton* getFbxSkeleton();
    FbxNode* fbxNode;
    FbxAMatrix fbxTransform;
    
    string parentBoneName;
    
protected:
    
    bool bExists;
    ofQuaternion origLocalRotation;
    ofVec3f origLocalPosition;
    bool bExternalControlEnabled;
    bool bUpdateFromAnimation;
};







