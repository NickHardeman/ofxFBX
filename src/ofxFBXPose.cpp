//
//  ofxFBXPose.cpp
//  DogImportTest
//
//  Created by Nick Hardeman on 9/22/14.
//

#include "ofxFBXPose.h"

//--------------------------------------------------------------
ofxFBXPose::ofxFBXPose() {
    name        = "default";
    fbxIndex    = 0;
    fbxPose     = NULL;
}

//--------------------------------------------------------------
void ofxFBXPose::setup( FbxPose* aPose, int aFbxIndex ) {
    fbxPose     = aPose;
    if( fbxPose != NULL ) {
        name        = fbxPose->GetName();
    }
    fbxIndex    = aFbxIndex;
}

//--------------------------------------------------------------
string ofxFBXPose::getName() {
    return name;
}

//--------------------------------------------------------------
int ofxFBXPose::getFbxIndex() {
    return fbxIndex;
}

//--------------------------------------------------------------
FbxPose* ofxFBXPose::getFbxPose() {
    return fbxPose;
}

//--------------------------------------------------------------
bool ofxFBXPose::isBindPose() {
    if( fbxPose == NULL ) return false;
    return fbxPose->IsBindPose();
}

//--------------------------------------------------------------
bool ofxFBXPose::isRestPose() {
    if( fbxPose == NULL ) return false;
    return fbxPose->IsRestPose();
}












