//
//  ofxFBXBone.cpp
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 11/1/13.
//
//

#include "ofxFBXBone.h"
#include "ofxFBXUtils.h"

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
//    map<string, ofxFBXBone >::iterator it;
//    for(it = bones.begin(); it != bones.end(); ++it ) {
//        it->second.clearParent();
//    }
//    
//    if( getParent() != NULL ) {
//        clearParent();
//    }
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
    
//    setTransformMatrix( ofGetGlobalTransform( fbxNode, FBXSDK_TIME_INFINITE, NULL ) );
    setLocalTransformMatrix( ofFbxGetGlobalTransform( fbxNode, FBXSDK_TIME_INFINITE, NULL ) );
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
void ofxFBXBone::update( FbxTime& pTime, FbxPose* pPose ) {
    
    if( isAnimationEnabled() ) {
        if( !bIsRoot ) {
//            setTransformMatrix( ofGetLocalTransform( fbxNode, pTime, pPose, NULL ));
            FbxAMatrix& tmatrix = fbxNode->EvaluateLocalTransform( pTime );
//            setTransformMatrix( fbxToOf(tmatrix) );
//            setTransformMatrix(tmatrix);
            setLocalTransformMatrix(tmatrix);
//            setGlobalTransformMatrix(tmatrix);
        } else {
            FbxAMatrix& tmatrix = fbxNode->EvaluateGlobalTransform( pTime );
//            setTransformMatrix(tmatrix);
            setLocalTransformMatrix(tmatrix);
//            setTransformMatrix( fbxToOf(tmatrix) );
            
//            setTransformMatrix( ofGetLocalTransform( fbxNode, pTime, pPose, NULL ));
            //FbxAMatrix& tmatrix = fbxNode->EvaluateLocalTransform( pTime );
            //setTransformMatrix( toOf(tmatrix) );
        }
    }
    
    map<string, ofxFBXBone >::iterator it;
    for(it = bones.begin(); it != bones.end(); ++it ) {
        it->second.update( pTime, pPose );
    }
}

//--------------------------------------------------------------
void ofxFBXBone::update( int aAnimIndex, signed long aMillis ) {
    if( isAnimationEnabled() ) {
        ofxFBXNode::update( aAnimIndex, aMillis );
    }
    
    map<string, ofxFBXBone >::iterator it;
    for(it = bones.begin(); it != bones.end(); ++it ) {
        it->second.update( aAnimIndex, aMillis );
    }
}

//--------------------------------------------------------------
void ofxFBXBone::update( int aAnimIndex1, signed long aAnim1Millis, int aAnimIndex2, signed long aAnim2Millis, float aMixPct ) {
    if( isAnimationEnabled() ) {
        ofxFBXNode::update( aAnimIndex1, aAnim1Millis, aAnimIndex2, aAnim2Millis, aMixPct );
    }
    
    map<string, ofxFBXBone >::iterator it;
    for(it = bones.begin(); it != bones.end(); ++it ) {
        it->second.update( aAnimIndex1, aAnim1Millis, aAnimIndex2, aAnim2Millis, aMixPct );
    }
}

//--------------------------------------------------------------
void ofxFBXBone::lateUpdate() {
    ofxFBXBone* sbone   = sourceBone;
    if( sbone != NULL ) {
        sbone->setPosition( getPosition() );
        sbone->setOrientation( getOrientationQuat() );
        sbone->setScale( getScale() );
//        sbone->setGlobalPosition( getGlobalPosition() );
//        sbone->setGlobalOrientation( getGlobalOrientation() );
//        sbone->setScale( getScale() );
//        sbone->setGlobalSca( getGlobalScale() );
        
//        sbone->setTransformMatrix( getLocalTransformMatrix() );
//        sbone->setTransformMatrix( getGlobalTransformMatrix() );
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
        glm::vec3 ppos = getParent()->getGlobalPosition();
        ofDrawLine( ppos, getGlobalPosition() );
    }
    
    map< string, ofxFBXBone >::iterator it;
    for(it = bones.begin(); it != bones.end(); ++it ) {
        it->second.draw( aLen );
    }
}

//--------------------------------------------------------------
void ofxFBXBone::updateFbxTransform() {
    // we need to convert this back before sending //
    fbxTransform = toFbx( getGlobalPosition(), getGlobalOrientation(), getGlobalScale() );
}

//--------------------------------------------------------------
void ofxFBXBone::pointTo( glm::vec3 aTarget ) {
    glm::vec3 axis( 1, 0, 0 );
    pointTo( aTarget, axis );
}

//--------------------------------------------------------------
void ofxFBXBone::pointTo( glm::vec3 aTarget, glm::vec3 aAxis ) {
    
    // TODO: FIX 
//    ofVec3f diff = aTarget - getGlobalPosition();
//    diff.normalize();
//    ofQuaternion tquat;
//    ofVec3f txaxis = aAxis * origGlobalTransform.getRotate();
//    tquat.makeRotate( txaxis, diff );
//    setGlobalOrientation( origGlobalRotation * tquat );

    auto relPosition = (getGlobalPosition() - aTarget);
    auto radius = glm::length(relPosition);
    if(radius>0){
        float latitude = acos(relPosition.y / radius) - glm::half_pi<float>();
        float longitude = atan2(relPosition.x , relPosition.z);
        glm::quat q = glm::angleAxis(latitude, glm::vec3(1,0,0)) * glm::angleAxis(longitude, glm::vec3(0,1,0)) * glm::angleAxis(0.f, glm::vec3(0,0,1));
        setGlobalOrientation(q);
    }
    

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
glm::quat& ofxFBXBone::getOriginalLocalRotation() {
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
    oStr += getName() + " kids: " +ofToString( bones.size(), 0) + pname + " anim: " + ofToString( isAnimationEnabled(), 0) + " num keys: " +ofToString(mKeyCollections.size(),0) + "\n";
    
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
















