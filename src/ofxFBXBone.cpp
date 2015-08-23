//
//  ofxFBXBone.cpp
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 11/1/13.
//
//

#include "ofxFBXBone.h"

ofxFBXBone::ofxFBXBone() {
//    disableExternalControl();
    bExists         = false;
    sourceBone      = NULL;
    parentBoneName  = "";
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
void ofxFBXBone::setup( FbxNode* pNode ) {
    ofxFBXNode::setup( pNode );
    fbxNode = pNode;
    
//    FbxTime ttime(0);
//    parentGlobalTransform = ofGetGlobalTransform( fbxNode->GetParent(), FBXSDK_TIME_INFINITE, NULL );
//    origTransform = ofGetGlobalTransform( fbxNode, ttime, NULL );
    setTransformMatrix( ofGetGlobalTransform( fbxNode, FBXSDK_TIME_INFINITE, NULL ) );
//    origRot = toOf(pNode->LclRotation.Get());
    
//    origNode.setTransformMatrix( ofGetGlobalTransform( fbxNode, FBXSDK_TIME_INFINITE, NULL ) );
    
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
//            bones[ sbone->getName() ] = tbone;
        }
    }
}

//--------------------------------------------------------------
void ofxFBXBone::cacheStartTransforms() {
//    setParent( *aParent, true );
//    origMatrixLocal     = getLocalTransformMatrix();
    origLocalPosition   = getPosition();
    origLocalRotation   = getOrientationQuat();
    origGlobalRotation  = getGlobalOrientation();
//    control.cacheStartTransforms();
}

//--------------------------------------------------------------
void ofxFBXBone::update( FbxTime& pTime, FbxPose* pPose ) {
    
    if( isAnimationEnabled() ) {        
        if(getParent() != NULL ) {
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
    
//    string parentNull = getParent() == NULL ? "yes" : "no";
//    string sourceNull = sourceBone == NULL ? "yes" : "no";
//    cout << getName() << " bones.size(): " << bones.size() << " is source null: " << sourceNull << endl;
    
//    map<string, ofxFBXBone >::iterator it;
//    for(it = bones.begin(); it != bones.end(); ++it ) {
//        ofxFBXBone* bone    = &it->second;
    ofxFBXBone* sbone   = sourceBone;//sourceBones[bone->getName()];//getSourceBone( it->first );
    if( sbone != NULL ) {
//            sbone->setGlobalOrientation( bone->getGlobalOrientation() );
        sbone->setTransformMatrix( getLocalTransformMatrix() );
//        sbone->setGlobalPosition( getGlobalPosition() );
//        sbone->setGlobalOrientation( getGlobalOrientation() );
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
    if( getParent() != NULL ) {
        //ofSetColor(255, 0, 130 );
        ofVec3f ppos = getParent()->getGlobalPosition();
        ofLine( ppos, getGlobalPosition() );
    }
    
    map< string, ofxFBXBone >::iterator it;
    for(it = bones.begin(); it != bones.end(); ++it ) {
        it->second.draw( aLen );
    }
}

//--------------------------------------------------------------
void ofxFBXBone::onPositionChanged() {
//    updateFbxTransform();
}

//--------------------------------------------------------------
void ofxFBXBone::onOrientationChanged() {
//    updateFbxTransform();
}

//--------------------------------------------------------------
void ofxFBXBone::onScaleChanged() {
//    updateFbxTransform();
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
//void ofxFBXBone::enableExternalControl() {
//    bExternalControlEnabled = true;
//}
//
////--------------------------------------------------------------
//void ofxFBXBone::disableExternalControl() {
//    bExternalControlEnabled = false;
//}
//
////--------------------------------------------------------------
//bool ofxFBXBone::isExternalControlEnabled() {
//    return bExternalControlEnabled;
//}

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
ofQuaternion ofxFBXBone::getOriginalLocalRotation() {
    return origLocalRotation;
}

//--------------------------------------------------------------
ofVec3f ofxFBXBone::getOriginalLocalPosition() {
    return origLocalPosition;
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
//    cout << "is tbone null = " << (tbone == NULL?"yes":"no") << endl;
    if( tbone ) {
        return tbone;
    }
    return NULL;
}

//--------------------------------------------------------------
void ofxFBXBone::findBoneRecursive( string aName, ofxFBXBone*& returnBone ) {
    
    if( !returnBone ) {
//        cout << getName() << " search name: " << aName << endl;
        map< string, ofxFBXBone >::iterator it;
        for(it = bones.begin(); it != bones.end(); ++it ) {
            if( it->second.getName() == aName ) {
                returnBone = (&it->second);
//                returnBone = shared_ptr< ofxFBXBone >( &returnBone );
//                cout << "***** it has been found ****** but is it null? " << (returnBone == NULL?"yes":"no") << endl;
                break;
            }
            it->second.findBoneRecursive( aName, returnBone );
        }
    }
}
















