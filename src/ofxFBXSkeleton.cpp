//
//  ofxFBXSkeleton.cpp
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 11/5/13.
//
//

#include "ofxFBXSkeleton.h"
ofxFBXBone dummyBone;

//--------------------------------------------------------------
ofxFBXSkeleton::ofxFBXSkeleton() {
    
}

//--------------------------------------------------------------
ofxFBXSkeleton::~ofxFBXSkeleton() {
    
}

//--------------------------------------------------------------
void ofxFBXSkeleton::setup( FbxNode *pNode ) {
    ofxFBXNode::setup( pNode );
    root.setup( pNode );
}

//--------------------------------------------------------------
void ofxFBXSkeleton::reconstructNodeParenting() {
    ofxFBXBone tbone = *rootSource;
    root = tbone;
    
    root.setAsRoot();
    root.sourceBone = rootSource;
    root.setupFromSourceBones();
    
    // now we need to reassociate the bone parent pointers, so they are not pointing to the source bones //
    // we need a way to get all of the bones as pointers, so we can easily make the association //
    map< string, ofxFBXBone* > tempBones = root.getAllBones();
    map< string, ofxFBXBone* >::iterator it;
    
    // clear the parents to remove the previous listeners //
    for( it = tempBones.begin(); it != tempBones.end(); ++it ) {
        if( it->second->getParent() != NULL ) {
            it->second->clearParent();
        }
    }
    
    for( it = tempBones.begin(); it != tempBones.end(); ++it ) {
        if( it->second->hasSkeletonParent() ) {
            if( tempBones.count(it->second->parentBoneName) ) {
                it->second->setParent( *tempBones[it->second->parentBoneName] );
            }
        }
    }
}

//--------------------------------------------------------------
void ofxFBXSkeleton::update( FbxTime& pTime, FbxPose* pPose ) {
    root.update( pTime, pPose );
}

//--------------------------------------------------------------
void ofxFBXSkeleton::lateUpdate() {
    root.lateUpdate();
}

//--------------------------------------------------------------
void ofxFBXSkeleton::draw( float aLen, bool aBDrawAxes ) {
    root.draw( aLen, aBDrawAxes );
}

//--------------------------------------------------------------
ofxFBXBone* ofxFBXSkeleton::getBone( string aName ) {
    if( root.getName() == aName ) return &root;
    return root.getBone( aName );
}

//--------------------------------------------------------------
void ofxFBXSkeleton::enableAnimationForBone( string aName, bool bRecursive ) {
    ofxFBXBone* bone = getBone( aName );
    if(bone == NULL) {
        ofLogWarning("ofxFBXSkeleton :: enableExternalControl : can not find bone with name ") << aName;
        return;
    }
    bone->enableAnimation( bRecursive );
}

//--------------------------------------------------------------
void ofxFBXSkeleton::disableAnimationForBone( string aName, bool bRecursive ) {
    ofxFBXBone* bone = getBone( aName );
    if(bone == NULL) {
        ofLogWarning("ofxFBXSkeleton :: enableExternalControl : can not find bone with name ") << aName;
        return;
    }
    bone->disableAnimation( bRecursive );
}

//--------------------------------------------------------------
void ofxFBXSkeleton::enableAnimation() {
    root.enableAnimation( true );
}

//--------------------------------------------------------------
void ofxFBXSkeleton::disableAnimation() {
    root.disableAnimation( true );
}

//--------------------------------------------------------------
int ofxFBXSkeleton::getNumBones() {
    return root.getNumBones();
}

//--------------------------------------------------------------
string ofxFBXSkeleton::toString() {
    string tstr = "Skeleton: " + root.getName()+ " total bones: " + ofToString( getNumBones(), 0 ) + "\n ";
    tstr += root.getAsString();
    return tstr;
}










