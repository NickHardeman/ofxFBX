//
//  ofxFBXSkeleton.h
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 11/5/13.
//
//

#pragma once
#include "ofxFBXBone.h"
#include "ofxFBXSrcSkeleton.h"
#include <map>

class ofxFBXSkeleton : public ofxFBXNode {
public:
    
    ofxFBXSource::Node::NodeType getType() override;
    
    void setup( shared_ptr<ofxFBXSource::Node> anode ) override;
    void setupRoot( shared_ptr<ofxFBXNode> aparent );
    
    void update( FbxTime& pTime, FbxPose* pPose ) override;
    void update( int aAnimIndex, signed long aMillis ) override;
    void update( int aAnimIndex1, signed long aAnim1Millis, int aAnimIndex2, signed long aAnim2Millis, float aMixPct ) override;
    
    void update() override;
    void lateUpdate( FbxTime& pTime, FbxAnimLayer * pAnimLayer, FbxPose* pPose ) override;
    
    void draw( float aLen, bool aBDrawAxes );
    
    int getNumBones();
    shared_ptr<ofxFBXBone> getBone( string aName );
    
    string getAsString( int aLevel=0 ) override;
    
    void enableAnimationForBone( string aName, bool bRecursive );
    void disableAnimationForBone( string aName, bool bRecursive );
    void enableAnimation();
    void disableAnimation();
    
private:
    shared_ptr<ofxFBXBone> root;
    
    void _checkSrcSkel();
    
    shared_ptr<ofxFBXSource::Skeleton> mSrcSkel;
};






