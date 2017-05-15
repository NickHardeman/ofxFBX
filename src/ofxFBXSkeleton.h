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
//#if defined(TARGET_LINUX) && !defined(TARGET_OPENGLES)
//    #include <fbxsdk.h>
//#endif
//#if defined(TARGET_OSX)
//    #include "fbxsdk.h"
//#endif
#include "ofxFBXNode.h"
#include "ofxFBXBone.h"
#include <map>

class ofxFBXSkeleton : public ofxFBXNode {
public:
    ofxFBXSkeleton();
    ~ofxFBXSkeleton();
    
    void setup( FbxNode *pNode );
    void reconstructNodeParenting();
    // get global transform update //
    void update( FbxTime& pTime, FbxPose* pPose );
    // keyframes update //
    void update( int aAnimIndex, signed long aMillis );
    void lateUpdate();
    
    void draw( float aLen = 6.f, bool aBDrawAxes = true );
    
    ofxFBXBone* getBone( string aName );
    
    void enableAnimationForBone( string aName, bool bRecursive );
    void disableAnimationForBone( string aName, bool bRecursive );
    
    void enableAnimation();
    void disableAnimation();
    
    int getNumBones();
    string toString();
    
    map< string, ofxFBXBone* > getAllBones();
    
    ofxFBXBone root;
    ofxFBXBone* rootSource;
    
private:
    
};






