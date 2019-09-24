//
//  ofxFBXPose.cpp
//  DogImportTest
//
//  Created by Nick Hardeman on 9/22/14.
//

#include "ofxFBXSrcPose.h"

using namespace ofxFBXSource;

//--------------------------------------------------------------
Pose::Pose() {
    name        = "default";
    fbxIndex    = 0;
    fbxPose     = NULL;
}

//--------------------------------------------------------------
void Pose::setup( FbxPose* aPose, int aFbxIndex ) {
    fbxPose     = aPose;
    if( fbxPose != NULL ) {
        name        = fbxPose->GetName();
    }
    fbxIndex    = aFbxIndex;
}

//--------------------------------------------------------------
string Pose::getName() {
    return name;
}

//--------------------------------------------------------------
int Pose::getFbxIndex() {
    return fbxIndex;
}

//--------------------------------------------------------------
FbxPose* Pose::getFbxPose() {
    return fbxPose;
}

//--------------------------------------------------------------
bool Pose::isBindPose() {
    if( fbxPose == NULL ) return false;
    return fbxPose->IsBindPose();
}

//--------------------------------------------------------------
bool Pose::isRestPose() {
    if( fbxPose == NULL ) return false;
    return fbxPose->IsRestPose();
}












