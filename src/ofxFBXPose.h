//
//  ofxFBXPose.h
//  DogImportTest
//
//  Created by Nick Hardeman on 9/22/14.
//

#pragma once
#include "ofxFBXUtils.h"

class ofxFBXPose {
public:
    
    ofxFBXPose();
    void setup( FbxPose* aPose, int aFbxIndex );
    
    string getName();
    int getFbxIndex();
    FbxPose* getFbxPose();
    
    bool isBindPose();
    bool isRestPose();
    
private:
    string name;
    int fbxIndex;
    FbxPose* fbxPose;
};
