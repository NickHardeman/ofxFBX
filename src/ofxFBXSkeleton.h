//
//  ofxFBXSkeleton.h
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 11/5/13.
//
//

#pragma once

#include "ofMain.h"
#if defined(TARGET_LINUX) && !defined(TARGET_OPENGLES)
    #include <fbxsdk.h>
#endif
#if defined(TARGET_OSX)
    #include "fbxsdk.h"
#endif
#include "ofxFBXNode.h"
#include "ofxFBXBone.h"
#include <map>

class ofxFBXSkeleton : public ofxFBXNode {
public:
    ofxFBXSkeleton();
    ~ofxFBXSkeleton();
    
    void setup( FbxNode *pNode );
    void reconstructNodeParenting();
    void update( FbxTime& pTime, FbxPose* pPose );
    void lateUpdate();
    
    void updateWithAnimation( FbxTime& pTime, FbxPose* pPose );
    void lateUpdateWithAnimation();
    
    void updateFromCachedSkeleton( shared_ptr< ofxFBXSkeleton> aSkeleton );
    
    void draw( float aLen = 6.f );
    
    void addBone( string aName, ofxFBXBone* aBone );
    ofxFBXBone* getBone( string aName );
    ofxFBXBone* getSourceBone( string aName );
    
    void enableExternalControl( string aName );
    void enableExternalControlRecursive( ofxFBXBone* aBone );
    
    void enableAnimation();
    void disableAnimation();
    
    int getNumBones();
    void reset();
    string toString();
    
private:
    void updateBoneProperties();
    
    void printOutBoneRecursive( stringstream& aSS, string aBoneName, map< string, bool >& aPrintMap, string aSpacer );
    map< string, ofxFBXBone* > sourceBones;
    map< string, ofxFBXBone > bones;
    int numBones;
};






