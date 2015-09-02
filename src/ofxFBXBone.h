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

//#if defined(TARGET_LINUX) && !defined(TARGET_OPENGLES)
//    #include <fbxsdk.h>
//#endif
//#if defined(TARGET_OSX)
//    #include "fbxsdk.h"
//#endif
#include "ofxFBXNode.h"


// note: Bones are called clusters when referring to them from an FbxMesh in the FBX SDK
class ofxFBXBone : public ofxFBXNode {
public:
    ofxFBXBone();
    ~ofxFBXBone();
    
    bool doesExist();
    
    void setAsRoot();
    void setup( FbxNode* pNode );
    void setupFromSourceBones();
    void cacheStartTransforms();
    void update( FbxTime& pTime, FbxPose* pPose );
    void lateUpdate();
    void draw( float aLen = 6.f, bool aBDrawAxes = true);
    
    void pointTo( ofVec3f aTarget );
    void pointTo( ofVec3f aTarget, ofVec3f aAxis );
    
    bool isLimb();
    bool hasSkeletonParent();
    
    void enableAnimation( bool bRecursively=false );
    void disableAnimation( bool bRecursively=false );
    bool isAnimationEnabled();
    
    void updateFbxTransform();
    void updateFbxTransformLocal();
    ofQuaternion& getOriginalLocalRotation();
    
    int getNumBones();
    map< string, ofxFBXBone* > getAllBones();
    
    string getAsString( int aLevel=0);
    ofxFBXBone* getBone( string aName );
    
    FbxSkeleton* getFbxSkeleton();
    FbxNode* fbxNode;
    FbxAMatrix fbxTransform;
    
    string parentBoneName;
    
//    int level;
    
    map< string, ofxFBXBone > bones;
    map< string, ofxFBXBone* > sourceBones;
    
    ofxFBXBone* sourceBone;
    
    ofQuaternion origLocalRotation, origGlobalRotation;
    ofMatrix4x4 origGlobalTransform;
    ofMatrix4x4 origLocalTransform;
    
protected:
    void findBoneRecursive( string aName, ofxFBXBone*& returnBone );
    void populateBonesRecursive( map< string, ofxFBXBone* >& aBoneMap );
    bool bExists, bIsRoot;
    
//    bool bExternalControlEnabled;
    bool bUpdateFromAnimation;
};







