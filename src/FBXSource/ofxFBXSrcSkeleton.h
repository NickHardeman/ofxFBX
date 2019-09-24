//
//  ofxFBXSkeleton.h
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 11/5/13.
//
//

#pragma once

#include "ofMain.h"
#include <fbxsdk.h>

#include "ofxFBXSrcNode.h"
#include "ofxFBXSrcBone.h"
#include <map>

namespace ofxFBXSource {

class Skeleton : public ofxFBXSource::Node {
public:
    Skeleton();
    ~Skeleton();
    
    ofxFBXSource::Node::NodeType getType() override { return OFX_FBX_SKELETON; }
    
    void setup( FbxNode *pNode ) override;
//    void reconstructNodeParenting();
    // get global transform update //
    void update( FbxTime& pTime, FbxPose* pPose ) override;
    // keyframes update //
    void update( int aAnimIndex, signed long aMillis ) override;
    void update( int aAnimIndex1, signed long aAnim1Millis, int aAnimIndex2, signed long aAnim2Millis, float aMixPct ) override;
    void lateUpdate();
    
//    void draw( float aLen = 6.f, bool aBDrawAxes = true );
    
    shared_ptr<ofxFBXSource::Bone> getBone( string aName );
    
//    void enableAnimationForBone( string aName, bool bRecursive );
//    void disableAnimationForBone( string aName, bool bRecursive );
    
//    void enableAnimation();
//    void disableAnimation();
    
    void clearKeyFrames() override;
    
    int getNumBones();
    string toString();
    
//    map< string, shared_ptr<ofxFBXSource::Bone> > getAllBones();
    
    shared_ptr<ofxFBXSource::Bone> root;
//    ofxFBXSource::Bone* root = NULL;
//    ofxFBXBone* rootSource;
    
private:
    
};

    
}





