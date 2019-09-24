//
//  ofxFBXSkeleton.cpp
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 11/5/13.
//
//

#include "ofxFBXSrcSkeleton.h"
using namespace ofxFBXSource;

ofxFBXSource::Bone dummyBone;

//--------------------------------------------------------------
Skeleton::Skeleton() {
    
}

//--------------------------------------------------------------
Skeleton::~Skeleton() {
    
}

//--------------------------------------------------------------
void Skeleton::setup( FbxNode *pNode ) {
    ofxFBXSource::Node::setup( pNode );
    root->setup( pNode );
    root->setParent(*this);
    root->setAsRoot();
}

//--------------------------------------------------------------
//void ofxFBXSkeleton::reconstructNodeParenting() {
//
////    cout << "ofxFBXSkeleton :: rootSounce->hasParent() : " << (rootSource->getParent()==NULL) << " | " << ofGetFrameNum() << endl;
//
//    ofxFBXBone tbone = *rootSource;
//    root = tbone;
//
//    root.setAsRoot();
//    root.sourceBone = rootSource;
//    root.setupFromSourceBones();
//
//    // now we need to reassociate the bone parent pointers, so they are not pointing to the source bones //
//    // we need a way to get all of the bones as pointers, so we can easily make the association //
//    map< string, ofxFBXBone* > tempBones = root.getAllBones();
//    map< string, ofxFBXBone* >::iterator it;
//
//    // clear the parents to remove the previous listeners //
//    for( it = tempBones.begin(); it != tempBones.end(); ++it ) {
//        if( it->second->getParent() != NULL ) {
//            it->second->clearParent();
//        }
//    }
//
//    for( it = tempBones.begin(); it != tempBones.end(); ++it ) {
//        if( it->second->hasSkeletonParent() ) {
//            if( tempBones.count(it->second->parentBoneName) ) {
//                it->second->setParent( *tempBones[it->second->parentBoneName] );
//            }
//        }
//    }
//}

//--------------------------------------------------------------
void Skeleton::update( FbxTime& pTime, FbxPose* pPose ) {
    root->update( pTime, pPose );
}

//--------------------------------------------------------------
void Skeleton::update( int aAnimIndex, signed long aMillis ) {
    root->update( aAnimIndex, aMillis );
}

//--------------------------------------------------------------
void Skeleton::update( int aAnimIndex1, signed long aAnim1Millis, int aAnimIndex2, signed long aAnim2Millis, float aMixPct ) {
    root->update( aAnimIndex1, aAnim1Millis, aAnimIndex2, aAnim2Millis, aMixPct );
}

//--------------------------------------------------------------
void Skeleton::lateUpdate() {
    root->lateUpdate();
}

//--------------------------------------------------------------
//void Skeleton::draw( float aLen, bool aBDrawAxes ) {
//    root->draw( aLen, aBDrawAxes );
//}

//--------------------------------------------------------------
shared_ptr<ofxFBXSource::Bone> Skeleton::getBone( string aName ) {
    if( root->getName() == aName ) return root;
    return root->getBone( aName );
}

//--------------------------------------------------------------
//void ofxFBXSkeleton::enableAnimationForBone( string aName, bool bRecursive ) {
//    ofxFBXBone* bone = getBone( aName );
//    if(bone == NULL) {
//        ofLogWarning("ofxFBXSkeleton :: enableExternalControl : can not find bone with name ") << aName;
//        return;
//    }
//    bone->enableAnimation( bRecursive );
//}
//
////--------------------------------------------------------------
//void ofxFBXSkeleton::disableAnimationForBone( string aName, bool bRecursive ) {
//    ofxFBXBone* bone = getBone( aName );
//    if(bone == NULL) {
//        ofLogWarning("ofxFBXSkeleton :: enableExternalControl : can not find bone with name ") << aName;
//        return;
//    }
//    bone->disableAnimation( bRecursive );
//}
//
////--------------------------------------------------------------
//void ofxFBXSkeleton::enableAnimation() {
//    root.enableAnimation( true );
//}
//
////--------------------------------------------------------------
//void ofxFBXSkeleton::disableAnimation() {
//    root.disableAnimation( true );
//}

//--------------------------------------------------------------
void Skeleton::clearKeyFrames() {
    root->clearKeyFrames();
}

//--------------------------------------------------------------
int Skeleton::getNumBones() {
    return root->getNumBones();
}

//--------------------------------------------------------------
string Skeleton::toString() {
    string tstr = "Skeleton: " + root->getName()+ " total bones: " + ofToString( getNumBones(), 0 ) + "\n ";
    tstr += root->getAsString();
    return tstr;
}

//--------------------------------------------------------------
//map< string, shared_ptr<ofxFBXSource::Bone> > Skeleton::getAllBones() {
//    return root->getAllBones();
//}










