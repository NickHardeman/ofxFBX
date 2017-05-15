//
//  ofxFBXNode.h
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 10/31/13.
//
//

#pragma once

#include "ofMain.h"
#include <fbxsdk.h>
#include "ofxFBXUtils.h"

template<class T>
struct ofxFBXKey {
    T value;
    signed long millis;
};

class ofxFBXKeyCollection {
public:
    string name;
    vector< ofxFBXKey<float> > posKeysX, posKeysY, posKeysZ;
    vector< ofxFBXKey<ofQuaternion> > rotKeys;
    vector< ofxFBXKey<float> > scaleKeysX, scaleKeysY, scaleKeysZ;
};

class ofxFBXNode : public ofNode {
public:
    
    ofxFBXNode();
    ~ofxFBXNode();
    
    void setup( FbxNode *pNode );
    string getName();
    FbxString getFbxName();
    void setName( FbxString aName );
    
    void setUseKeyFrames( bool ab );
    bool usingKeyFrames();
    
    void cacheStartTransforms();
    
    virtual void update( int aAnimIndex, signed long aMillis );
    
    float getKeyValue( std::vector<ofxFBXKey<float> >& keys, signed long ms );
    ofQuaternion getKeyRotation(vector<ofxFBXKey<ofQuaternion> >& keys, signed long ms);
    
    ofxFBXKeyCollection& getKeyCollection( int aAnimIndex ) {
        if( mKeyCollections.count(aAnimIndex) < 1 ) {
            ofxFBXKeyCollection temp;
            mKeyCollections[ aAnimIndex ] = temp;
        }
        return mKeyCollections[aAnimIndex];
    }
    
protected:
    ofQuaternion origLocalRotation, origGlobalRotation;
    ofMatrix4x4 origGlobalTransform;
    ofMatrix4x4 origLocalTransform;
    
    ofVec3f origPos, origScale;
    
    string name = "";
    int mAnimIndex = 0;
    bool bUseKeyFrames = false;
    map<int, ofxFBXKeyCollection> mKeyCollections;
};


















