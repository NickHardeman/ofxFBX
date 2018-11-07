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
    origScale = glm::vec3(1,1,1);
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
    
    ofVec3f tpos = getKeyTranslation( aAnimIndex, aMillis );
    ofVec3f cpos = getPosition();
    if( cpos.x != tpos.x || cpos.y != tpos.y || cpos.z != tpos.z ) {
        setPosition( tpos );
    }
    
    ofVec3f tscale = getKeyScale( aAnimIndex, aMillis );
    ofVec3f cscale = getScale();
    if( cscale.x != tscale.x || cscale.y != tscale.y || cscale.z != tscale.z ) {
        setScale( tscale );
    }
    
    ofQuaternion tquat = getKeyRotation( aAnimIndex, aMillis );
    ofQuaternion cquat = getOrientationQuat();
    if( cquat.x() != tquat.x() || cquat.y() != tquat.y() || cquat.z() != tquat.z() || cquat.w() != tquat.w() ) {
        setOrientation( tquat );
    }
}

//----------------------------------------
void ofxFBXNode::update( int aAnimIndex1, signed long aAnim1Millis, int aAnimIndex2, signed long aAnim2Millis, float aMixPct ) {
    if( mKeyCollections.size() == 0 ) return;
    if( aAnimIndex1 < 0 ) return;
    if( aAnimIndex1 >= mKeyCollections.size() ) return;
    if( aAnimIndex2 < 0 ) return;
    if( aAnimIndex2 >= mKeyCollections.size() ) return;
    
    aMixPct = ofClamp(aMixPct, 0.0, 1.0);
    float invpct = 1.0 - aMixPct;
    
    ofVec3f tpos1 = getKeyTranslation( aAnimIndex1, aAnim1Millis );
    ofVec3f tpos2 = getKeyTranslation( aAnimIndex2, aAnim2Millis);
    
    ofVec3f tscale1 = getKeyScale( aAnimIndex1, aAnim1Millis );
    ofVec3f tscale2 = getKeyScale( aAnimIndex2, aAnim2Millis );
    
    ofQuaternion tquat1 = getKeyRotation( aAnimIndex1, aAnim1Millis );
    ofQuaternion tquat2 = getKeyRotation( aAnimIndex2, aAnim2Millis );
    
    
    if( aMixPct <= 0.0f ) {
        setPosition( tpos1 );
        setScale(tscale1);
        setOrientation(tquat1);
    } else if( aMixPct >= 1.0 ) {
        setPosition( tpos2 );
        setScale(tscale2);
        setOrientation(tquat2);
    } else {
        setPosition( tpos1 * invpct + tpos2 * aMixPct );
        setScale( tscale1 * invpct + tscale2 * aMixPct );
        tquat1.slerp( aMixPct, tquat1, tquat2 );
        setOrientation( tquat1 );
    }
}

//----------------------------------------
ofVec3f ofxFBXNode::getKeyTranslation( int aAnimIndex, signed long aMillis ) {
    if( aAnimIndex < 0 ) return origPos;
    if( mKeyCollections.size() == 0 ) return origPos;
    if( aAnimIndex >= mKeyCollections.size() ) return origPos;
    
    ofxFBXKeyCollection& tcollection = mKeyCollections[aAnimIndex];
    mAnimIndex = aAnimIndex;
    ofVec3f tpos = origPos;
    if(tcollection.posKeysX.size() > 0) tpos.x = getKeyValue( tcollection.posKeysX, aMillis );
    if(tcollection.posKeysY.size() > 0) tpos.y = getKeyValue( tcollection.posKeysY, aMillis );
    if(tcollection.posKeysZ.size() > 0) tpos.z = getKeyValue( tcollection.posKeysZ, aMillis );
    return tpos;
}

//----------------------------------------
ofQuaternion ofxFBXNode::getKeyRotation( int aAnimIndex, signed long aMillis ) {
    if( aAnimIndex < 0 ) return origLocalRotation;
    if( mKeyCollections.size() == 0 ) return origLocalRotation;
    if( aAnimIndex >= mKeyCollections.size() ) return origLocalRotation;
    
    ofxFBXKeyCollection& tcollection = mKeyCollections[aAnimIndex];
    mAnimIndex = aAnimIndex;
    
    return getKeyRotation( tcollection.rotKeys, aMillis );
}

//----------------------------------------
ofVec3f ofxFBXNode::getKeyScale( int aAnimIndex, signed long aMillis ) {
    if( aAnimIndex < 0 ) return origScale;
    if( mKeyCollections.size() == 0 ) return origScale;
    if( aAnimIndex >= mKeyCollections.size() ) return origScale;
    
    ofxFBXKeyCollection& tcollection = mKeyCollections[aAnimIndex];
    mAnimIndex = aAnimIndex;
    
    ofVec3f tscale = origScale;
    if(tcollection.scaleKeysX.size() > 0) tscale.x = getKeyValue( tcollection.scaleKeysX, aMillis );
    if(tcollection.scaleKeysY.size() > 0) tscale.y = getKeyValue( tcollection.scaleKeysY, aMillis );
    if(tcollection.scaleKeysZ.size() > 0) tscale.z = getKeyValue( tcollection.scaleKeysZ, aMillis );
    
    return tscale;
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



