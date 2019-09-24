//
//  ofxFBXSkeleton.cpp
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 11/5/13.
//
//

#include "ofxFBXSkeleton.h"

//----------------------------------------
ofxFBXSource::Node::NodeType ofxFBXSkeleton::getType() {
    return ofxFBXSource::Node::OFX_FBX_SKELETON;
}

//--------------------------------------------------------------------------------
void ofxFBXSkeleton::setup( shared_ptr<ofxFBXSource::Node> anode ) {
    ofxFBXNode::setup( anode );
    if( anode && anode->getType() == ofxFBXSource::Node::OFX_FBX_SKELETON ) {
        auto tskel = dynamic_pointer_cast<ofxFBXSource::Skeleton>(anode);
        if( tskel ) {
            root = make_shared<ofxFBXBone>();
            root->setParent(*this);
            //root->setup( anode );
            root->setBoneSource( tskel->root, root );
            addChild(root);
        }
    }
}

//--------------------------------------------------------------
void ofxFBXSkeleton::setupRoot( shared_ptr<ofxFBXNode> aparent ) {
    root->setParentNode( aparent );
}

//--------------------------------------------------------------
void ofxFBXSkeleton::update( FbxTime& pTime, FbxPose* pPose ) {
    _checkSrcSkel();
    if(mSrcSkel) {
        mSrcSkel->update( pTime, pPose );
        
    }
}

//--------------------------------------------------------------
void ofxFBXSkeleton::update( int aAnimIndex, signed long aMillis ) {
    _checkSrcSkel();
//    cout << "ofxFBXSkeleton::update " << getName() << " : src skel: " << ( mSrcSkel ? "Good" : "Bad" ) << " | " << ofGetFrameNum() << endl;
    if(mSrcSkel) {
        mSrcSkel->update( aAnimIndex, aMillis );
    }
}

//--------------------------------------------------------------
void ofxFBXSkeleton::update( int aAnimIndex1, signed long aAnim1Millis, int aAnimIndex2, signed long aAnim2Millis, float aMixPct ) {
    _checkSrcSkel();
    if(mSrcSkel) {
        mSrcSkel->update( aAnimIndex1, aAnim1Millis, aAnimIndex2, aAnim2Millis, aMixPct );
    }
}

//----------------------------------------------------------------
void ofxFBXSkeleton::update() {
    root->update();
}

//----------------------------------------------------------------
void ofxFBXSkeleton::lateUpdate(FbxTime& pTime, FbxAnimLayer* pAnimLayer, FbxPose* pPose) {
    root->lateUpdate();
}

//----------------------------------------------------------------
void ofxFBXSkeleton::draw( float aLen, bool aBDrawAxes ) {
//    transformGL(); {
        root->draw( aLen, aBDrawAxes );
//    } restoreTransformGL();
}

//----------------------------------------------------------------
int ofxFBXSkeleton::getNumBones() {
    return root->getNumBones();
}

//----------------------------------------------------------------
shared_ptr<ofxFBXBone> ofxFBXSkeleton::getBone( string aName ) {
    if( root->getName() == aName ) return root;
    return root->getBone( aName );
}

//----------------------------------------------------------------
string ofxFBXSkeleton::getAsString( int aLevel ) {
    string tstr = "Skeleton: " + root->getName()+ " total bones: " + ofToString( getNumBones(), 0 ) + "\n ";
    return (tstr + root->getAsString(1));
}

//----------------------------------------------------------------
void ofxFBXSkeleton::enableAnimationForBone( string aName, bool bRecursive ) {
    shared_ptr<ofxFBXBone> bone = getBone( aName );
    if(bone == NULL) {
        ofLogWarning("Skeleton :: enableExternalControl : can not find bone with name ") << aName;
        return;
    }
    bone->enableAnimation( bRecursive );
}

//----------------------------------------------------------------
void ofxFBXSkeleton::disableAnimationForBone( string aName, bool bRecursive ) {
    shared_ptr<ofxFBXBone> bone = getBone( aName );
    if(bone == NULL) {
        ofLogWarning("Skeleton :: enableExternalControl : can not find bone with name ") << aName;
        return;
    }
    bone->disableAnimation( bRecursive );
}

//----------------------------------------------------------------
void ofxFBXSkeleton::enableAnimation() {
    root->enableAnimation( true );
}

//----------------------------------------------------------------
void ofxFBXSkeleton::disableAnimation() {
    root->disableAnimation( true );
}

//----------------------------------------------------------------
void ofxFBXSkeleton::_checkSrcSkel() {
    if( !mSrcSkel && mSrcNode && mSrcNode->getType() == ofxFBXSource::Node::OFX_FBX_SKELETON ) {
        mSrcSkel = dynamic_pointer_cast<ofxFBXSource::Skeleton>(mSrcNode);
    }
}
