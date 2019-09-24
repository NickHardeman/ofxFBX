//
//  ofxFBXPose.h
//  DogImportTest
//
//  Created by Nick Hardeman on 9/22/14.
//

#pragma once
#include "ofxFBXUtils.h"

namespace ofxFBXSource {

class Pose {
public:
    
    Pose();
    void setup( FbxPose* aPose, int aFbxIndex );
    
    string getName();
    int getFbxIndex();
    FbxPose* getFbxPose();
    
    bool isBindPose();
    bool isRestPose();
    
private:
    string name;
    int fbxIndex = 0;
    FbxPose* fbxPose = NULL;
};
    
}
