//
//  ofxFBXBone.cpp
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 11/1/13.
//
//

#include "ofxFBXBone.h"

//--------------------------------------------------------------
ofxFBXBone::ofxFBXBone() {
    bExists         = false;
    sourceBone      = NULL;
    parentBoneName  = "";
    bIsRoot         = false;
    enableAnimation();
}

//--------------------------------------------------------------
ofxFBXBone::~ofxFBXBone() {
    
}

//--------------------------------------------------------------
bool ofxFBXBone::doesExist() {
    return bExists;
}

//--------------------------------------------------------------
void ofxFBXBone::setAsRoot() {
    bIsRoot = true;
}

//--------------------------------------------------------------
void ofxFBXBone::setup( FbxNode* pNode ) {
    ofxFBXNode::setup( pNode );
    fbxNode = pNode;
    
    setTransformMatrix( ofGetGlobalTransform( fbxNode, FBXSDK_TIME_INFINITE, NULL ) );
    bExists = true;
    
    updateFbxTransform();
}

//--------------------------------------------------------------
void ofxFBXBone::setupFromSourceBones() {
    
    map<string, ofxFBXBone* >::iterator it;
    for(it = sourceBones.begin(); it != sourceBones.end(); ++it ) {
        ofxFBXBone* sbone   = it->second;
        if( sbone != NULL ) {
            ofxFBXBone tbone = *sbone;
//            cout << "adding source bone: " << bones[ sbone->getName() ].sourceBone->getName() << endl;
            bones[ sbone->getName() ]            = tbone;
            bones[ sbone->getName() ].sourceBone = sbone;
            bones[ sbone->getName() ].fbxNode    = sbone->fbxNode;
            bones[ sbone->getName() ].setupFromSourceBones();
        }
    }
}

//--------------------------------------------------------------
void ofxFBXBone::cacheStartTransforms() {
    // cache the orientations for use later //
    origGlobalRotation  = getGlobalOrientation();
    origLocalRotation   = getOrientationQuat();
    origGlobalTransform = getGlobalTransformMatrix();
    origLocalTransform  = getLocalTransformMatrix();
}

//--------------------------------------------------------------
void ofxFBXBone::update( FbxTime& pTime, FbxPose* pPose ) {
    
    if( isAnimationEnabled() ) {
        if( !bIsRoot ) {
            setTransformMatrix( ofGetLocalTransform( fbxNode, pTime, pPose, NULL ));
        } else {
            FbxAMatrix tmatrix = fbxNode->EvaluateGlobalTransform( pTime );
            setTransformMatrix( toOf(tmatrix) );
        }
    }
    
    map<string, ofxFBXBone >::iterator it;
    for(it = bones.begin(); it != bones.end(); ++it ) {
        it->second.update( pTime, pPose );
    }
}

//--------------------------------------------------------------
void ofxFBXBone::lateUpdate() {
    ofxFBXBone* sbone   = sourceBone;
    if( sbone != NULL ) {
        sbone->setTransformMatrix( getLocalTransformMatrix() );
        sbone->updateFbxTransform();
    }

    map<string, ofxFBXBone >::iterator it;
    for(it = bones.begin(); it != bones.end(); ++it ) {
        it->second.lateUpdate();
    }
}

//--------------------------------------------------------------
void ofxFBXBone::draw( float aLen, bool aBDrawAxes ) {
    if(aBDrawAxes) {
        transformGL(); {
            ofDrawAxis( aLen );
        } restoreTransformGL();
    }
//    if(!hasSkeletonParent()) return;
//    if(!isLimb()) return;
    if( getParent() != NULL && !bIsRoot ) {
        //ofSetColor(255, 0, 130 );
        ofVec3f ppos = getParent()->getGlobalPosition();
        ofDrawLine( ppos, getGlobalPosition() );
    }
    
    map< string, ofxFBXBone >::iterator it;
    for(it = bones.begin(); it != bones.end(); ++it ) {
        it->second.draw( aLen );
    }
}

//--------------------------------------------------------------
void ofxFBXBone::updateFbxTransform() {
    fbxTransform = toFbx( getGlobalTransformMatrix() );
}

//--------------------------------------------------------------
void ofxFBXBone::updateFbxTransformLocal() {
    if(getParent() != NULL ) {
        fbxTransform = toFbx( getGlobalTransformMatrix() * ofMatrix4x4::getInverseOf( getParent()->getGlobalTransformMatrix() ) );
    } else {
        updateFbxTransform();
    }
}

//--------------------------------------------------------------
void ofxFBXBone::pointTo( ofVec3f aTarget ) {
    ofVec3f axis( 1, 0, 0 );
    pointTo( aTarget, axis );
}

//--------------------------------------------------------------
void ofxFBXBone::pointTo( ofVec3f aTarget, ofVec3f aAxis ) {
    ofVec3f diff = aTarget - getGlobalPosition();
    diff.normalize();
    ofQuaternion tquat;
    ofVec3f txaxis = aAxis * origGlobalTransform.getRotate();
    tquat.makeRotate( txaxis, diff );
    setGlobalOrientation( origGlobalRotation * tquat );
}

//--------------------------------------------------------------
bool ofxFBXBone::isLimb() {
    FbxSkeleton* lSkeleton = getFbxSkeleton();
    if(lSkeleton) {
        lSkeleton->GetSkeletonType() == FbxSkeleton::eLimbNode;
        return true;
    }
    return false;
}

//--------------------------------------------------------------
bool ofxFBXBone::hasSkeletonParent() {
    if(!fbxNode->GetParent()) return false;
    if(!fbxNode->GetParent()->GetNodeAttribute()) return false;
    return fbxNode->GetParent()->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton;
}

//--------------------------------------------------------------
FbxSkeleton* ofxFBXBone::getFbxSkeleton() {
    return (FbxSkeleton*) fbxNode->GetNodeAttribute();
}

//--------------------------------------------------------------
void ofxFBXBone::enableAnimation( bool bRecursively ) {
    bUpdateFromAnimation = true;
    if( bRecursively ) {
        map< string, ofxFBXBone >::iterator it;
        for(it = bones.begin(); it != bones.end(); ++it ) {
            it->second.enableAnimation( bRecursively );
        }
    }
}

//--------------------------------------------------------------
void ofxFBXBone::disableAnimation( bool bRecursively) {
    bUpdateFromAnimation = false;
    if( bRecursively ) {
        map< string, ofxFBXBone >::iterator it;
        for(it = bones.begin(); it != bones.end(); ++it ) {
            it->second.disableAnimation( bRecursively );
        }
    }
}

//--------------------------------------------------------------
bool ofxFBXBone::isAnimationEnabled() {
    return bUpdateFromAnimation;
}

//--------------------------------------------------------------
ofQuaternion& ofxFBXBone::getOriginalLocalRotation() {
    return origLocalRotation;
}

//--------------------------------------------------------------
int ofxFBXBone::getNumBones() {
    int ttotal = bones.size();
    map<string, ofxFBXBone >::iterator it;
    for(it = bones.begin(); it != bones.end(); ++it ) {
        ttotal += it->second.getNumBones();
    }
    return ttotal;
}

//--------------------------------------------------------------
map< string, ofxFBXBone* > ofxFBXBone::getAllBones() {
    map< string, ofxFBXBone* > tbones;
    tbones[ getName() ] = this;
    populateBonesRecursive( tbones );
    return tbones;
}

//--------------------------------------------------------------
void ofxFBXBone::populateBonesRecursive( map< string, ofxFBXBone* >& aBoneMap ) {
    
    map<string, ofxFBXBone >::iterator it;
    for(it = bones.begin(); it != bones.end(); ++it ) {
        if( !aBoneMap.count(it->first) ) {
            aBoneMap[ it->first ] = &(it->second);
        }
        it->second.populateBonesRecursive( aBoneMap );
    }
}

//--------------------------------------------------------------
string ofxFBXBone::getAsString( int aLevel ) {
    string oStr = "";
    for( int i = 0; i < aLevel; i++ ) {
        oStr += "  ";
    }
    if( bones.size() ) {
        oStr += "+ ";
    } else {
        oStr += "- ";
    }
    string pname = "";
    if( getParent() != NULL ) {
        pname = " parent: " + parentBoneName;
    }
    oStr += getName() + " kids: " +ofToString( bones.size(), 0) + pname + " anim: " + ofToString( isAnimationEnabled(), 0) + "\n";
    
    map<string, ofxFBXBone >::iterator it;
    for(it = bones.begin(); it != bones.end(); ++it ) {
        oStr += it->second.getAsString( aLevel + 1);
    }
    return oStr;
}

//--------------------------------------------------------------
ofxFBXBone* ofxFBXBone::getBone( string aName ) {
    ofxFBXBone* tbone = NULL;
    map< string, ofxFBXBone >::iterator it;
    findBoneRecursive( aName, tbone );
    if( tbone ) {
        return tbone;
    }
    return NULL;
}

//--------------------------------------------------------------
void ofxFBXBone::findBoneRecursive( string aName, ofxFBXBone*& returnBone ) {
    
    if( !returnBone ) {
        map< string, ofxFBXBone >::iterator it;
        for(it = bones.begin(); it != bones.end(); ++it ) {
            if( it->second.getName() == aName ) {
                returnBone = (&it->second);
                break;
            }
            it->second.findBoneRecursive( aName, returnBone );
        }
    }
}
















