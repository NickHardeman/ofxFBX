//
//  ofxCachedSkeletonAnimation.h
//  DogImportTest
//
//  Created by Nick Hardeman on 10/4/14.
//

#pragma once
#include "ofxFBXSkeleton.h"
#include "ofxFBXAnimation.h"

class ofxFBXCachedSkeletonAnimation {
public:
    void setup( shared_ptr<ofxFBXSkeleton> aSkeleton, ofxFBXAnimation aAnimation );
    void update( int aFrameNum, shared_ptr<ofxFBXSkeleton> aSkeleton );
    
protected:
    // map the frame numbers to the skeletons //
//    map< int, shared_ptr<ofxFBXSkeleton> > cachedSkeletons;
    vector< shared_ptr<ofxFBXSkeleton> > cachedSkeletons;
};





