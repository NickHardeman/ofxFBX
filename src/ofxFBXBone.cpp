//
//  ofxFBXBone.cpp
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 11/1/13.
//
//

#include "ofxFBXBone.h"

ofxFBXBone::ofxFBXBone() {
    disableExternalControl();
    bExists         = false;
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
void ofxFBXBone::reset() 
{
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
void ofxFBXBone::cacheStartTransforms() {
//    setParent( *aParent, true );
//    origMatrixLocal     = getLocalTransformMatrix();
    origLocalPosition   = getPosition();
    origLocalRotation   = getOrientationQuat();
//    control.cacheStartTransforms();
}

//--------------------------------------------------------------
void ofxFBXBone::update( FbxTime& pTime, FbxPose* pPose ) {
    if( isAnimationEnabled() ) {
//        cout << "Animation for bone : " << getName() << " | " << ofGetElapsedTimef() << endl;
        if(getParent() != NULL ) {
//            setTransformMatrix( ofGetGlobalTransform( fbxNode, pTime, NULL ) * ofMatrix4x4::getInverseOf( getParent()->getGlobalTransformMatrix() ) );
//            ofNode pnode;
//            pnode.setTransformMatrix( ofGetGlobalTransform( fbxNode, pTime, NULL ) );
//            setGlobalOrientation( pnode.getOrientationQuat() );
//            setGlobalPosition( pnode.getPosition() );
//            EvaluateGlobalTransform(FbxTime pTime=FBXSDK_TIME_INFINITE, FbxNode::EPivotSet pPivotSet=FbxNode::eSourcePivot, bool pApplyTarget=false, bool pForceEval=false)
            
            
//            FbxAMatrix tmatrix = fbxNode->EvaluateLocalTransform( pTime, FbxNode::eSourcePivot, false, false );
//            setTransformMatrix( toOf(tmatrix) );
            
//            FbxAMatrix GetGlobalPosition(FbxNode* pNode, const FbxTime& pTime, FbxPose* pPose, FbxAMatrix* pParentGlobalPosition)
//            FbxAMatrix tmatrix = GetGlobalPosition( fbxNode, pTime, pPose );
//            setTransformMatrix( ofGetGlobalTransform( fbxNode, pTime, pPose ) * ofMatrix4x4::getInverseOf( getParent()->getGlobalTransformMatrix() ) );
//            setTransformMatrix( toOf(tmatrix) );
            
//            ofxFBXBone* boneParent = (ofxFBXBone*)getParent();
            
            setTransformMatrix( ofGetLocalTransform( fbxNode, pTime, pPose, NULL ));
            
        } else {
//            FbxAMatrix tmatrix = fbxNode->EvaluateGlobalTransform( pTime, FbxNode::eSourcePivot, false, false );
//            setTransformMatrix( toOf(tmatrix) );
//            setTransformMatrix( ofGetGlobalTransform( fbxNode, pTime, NULL ) );
            FbxAMatrix tmatrix = fbxNode->EvaluateGlobalTransform( pTime, FbxNode::eSourcePivot, false, false );
            setTransformMatrix( toOf(tmatrix) );
        }
    }
//    updateFbxTransform();
//    if(hasSkeletonParent()) {
//        parentGlobalTransform = ofGetGlobalTransform( fbxNode->GetParent(), pTime, NULL );
//    }
//    setTransformMatrix(ofGetGlobalTransform( fbxNode, pTime, NULL ));
//    setTransformMatrix( globalTransform );
}

//--------------------------------------------------------------
void ofxFBXBone::draw( float aLen ) {
    transformGL(); {
        ofDrawAxis( aLen );
    } restoreTransformGL();
//    if(!hasSkeletonParent()) return;
//    if(!isLimb()) return;
    if(getParent() != NULL) {
        ofSetColor(255, 255, 30 );
        ofVec3f ppos = getParent()->getGlobalPosition();
        ofSetColor(255, 255, 30 );
        ofLine( ppos, getGlobalPosition() );
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
void ofxFBXBone::enableExternalControl() {
    bExternalControlEnabled = true;
}

//--------------------------------------------------------------
void ofxFBXBone::disableExternalControl() {
    bExternalControlEnabled = false;
}

//--------------------------------------------------------------
bool ofxFBXBone::isExternalControlEnabled() {
    return bExternalControlEnabled;
}

//--------------------------------------------------------------
void ofxFBXBone::enableAnimation() {
    bUpdateFromAnimation = true;
}

//--------------------------------------------------------------
void ofxFBXBone::disableAnimation() {
    bUpdateFromAnimation = false;
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
















