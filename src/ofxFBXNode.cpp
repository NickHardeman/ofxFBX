//
//  ofxFBXNode.cpp
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 10/31/13.
//
//

#include "ofxFBXNode.h"

//----------------------------------------
ofxFBXNode::ofxFBXNode() {
//    parent          = NULL;
//    globalParent    = NULL;
    origScale.set(1,1,1);
}

//----------------------------------------
ofxFBXNode::~ofxFBXNode() {
    if( getParent() != NULL ) {
        clearParent();
    }
}

//----------------------------------------
void ofxFBXNode::setup( FbxNode *pNode ) {
    setName( pNode->GetNameOnly() );
}

//----------------------------------------
string ofxFBXNode::getName() {
    return name;
}

//----------------------------------------
FbxString ofxFBXNode::getFbxName() {
    return FbxString( name.c_str() );
}

//----------------------------------------
void ofxFBXNode::setName( FbxString aName ) {
    name = aName;
}

//--------------------------------------------------------------
void ofxFBXNode::setUseKeyFrames( bool ab ) {
    bUseKeyFrames = ab;
}

//--------------------------------------------------------------
bool ofxFBXNode::usingKeyFrames() {
    return bUseKeyFrames;
}

//--------------------------------------------------------------
void ofxFBXNode::cacheStartTransforms() {
    // cache the orientations for use later //
    origGlobalRotation  = getGlobalOrientation();
    origLocalRotation   = getOrientationQuat();
    origGlobalTransform = getGlobalTransformMatrix();
    origLocalTransform  = getLocalTransformMatrix();
    origPos             = getPosition();
    origScale           = getScale();
}

//----------------------------------------
void ofxFBXNode::update( int aAnimIndex, signed long aMillis ) {
    if( aAnimIndex < 0 ) return;
    if( mKeyCollections.size() == 0 ) return;
    if( aAnimIndex >= mKeyCollections.size() ) return;
    
    ofxFBXKeyCollection& tcollection = mKeyCollections[aAnimIndex];
    mAnimIndex = aAnimIndex;
    ofVec3f tpos = origPos;
    if(tcollection.posKeysX.size() > 0) tpos.x = getKeyValue( tcollection.posKeysX, aMillis );
    if(tcollection.posKeysY.size() > 0) tpos.y = getKeyValue( tcollection.posKeysY, aMillis );
    if(tcollection.posKeysZ.size() > 0) tpos.z = getKeyValue( tcollection.posKeysZ, aMillis );
    
    ofVec3f cpos = getPosition();
    if( cpos.x != tpos.x || cpos.y != tpos.y || cpos.z != tpos.z ) {
        setPosition( tpos );
    }
    
    ofVec3f tscale = origScale;
    if(tcollection.scaleKeysX.size() > 0) tscale.x = getKeyValue( tcollection.scaleKeysX, aMillis );
    if(tcollection.scaleKeysY.size() > 0) tscale.y = getKeyValue( tcollection.scaleKeysY, aMillis );
    if(tcollection.scaleKeysZ.size() > 0) tscale.z = getKeyValue( tcollection.scaleKeysZ, aMillis );
    
    ofVec3f cscale = getScale();
    if( cscale.x != tscale.x || cscale.y != tscale.y || cscale.z != tscale.z ) {
        setScale( tscale );
    }
    
    ofQuaternion tquat = getKeyRotation( tcollection.rotKeys, aMillis );
    ofQuaternion cquat = getOrientationQuat();
    if( cquat.x() != tquat.x() || cquat.y() != tquat.y() || cquat.z() != tquat.z() || cquat.w() != tquat.w() ) {
        setOrientation( tquat );
    }
    
//    cout << "anim index: " << aAnimIndex << " millis: " << aMillis << " updating bone: " << getName() << " rot: " << tquat << " num rot keys: " << tcollection.rotKeys.size() << " scale: " << tscale << " | " << ofGetFrameNum() << endl;
    
    
}

//----------------------------------------
// from Arturo Castro's ofxFBX ///
float ofxFBXNode::getKeyValue( vector<ofxFBXKey<float> >& keys, signed long ms ) {
    for(int i=0;i<keys.size();i++){
        if(keys[i].millis==ms){
            return keys[i].value;
        }else if(keys[i].millis>ms){
            if(i>0){
                signed long delta = ms - keys[i-1].millis;
                float pct = double(delta) / double(keys[i].millis - keys[i-1].millis);
                return ofLerp(keys[i-1].value,keys[i].value,pct);
            }else{
                return keys[0].value;
                //u_long delta = ms;
                //float pct = double(delta) / double(keys[i].millis);
                //return ofLerp(0.0f,keys[i].value,pct);
            }
        }
    }
    if(keys.empty()){
        return 0.0f;
    }else{
        return keys.back().value;
    }
    return 0.0f;
}

//----------------------------------------
// from Arturo Castro's ofxFBX ///
ofQuaternion ofxFBXNode::getKeyRotation(vector<ofxFBXKey<ofQuaternion> >& keys, signed long ms) {
    for(int i=0;i<keys.size();i++){
        if(keys[i].millis==ms){
            return keys[i].value;
        }else if(keys[i].millis>ms){
            if(i>0){
                signed long delta = ms - keys[i-1].millis;
                float pct = double(delta) / double(keys[i].millis - keys[i-1].millis);
                ofQuaternion q;
                q.slerp(pct,keys[i-1].value,keys[i].value);
                return q;
            }else{
                signed long delta = ms;
                float pct = double(delta) / double(keys[i].millis);
                ofQuaternion q = keys[i].value;
                q.slerp(pct,origLocalRotation,keys[i].value);
                return q;
            }
        }
    }
    if(keys.empty()){
        return origLocalRotation;
    }else{
        return keys.back().value;
    }
    return origLocalRotation;
}



