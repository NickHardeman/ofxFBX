//
//  ofxCachedSkeletonAnimation.cpp
//  DogImportTest
//
//  Created by Nick Hardeman on 10/4/14.
//

#include "ofxFBXCachedSkeletonAnimation.h"

//--------------------------------------------------------------
void ofxFBXCachedSkeletonAnimation::setup( shared_ptr<ofxFBXSkeleton> aSkeleton, ofxFBXAnimation aAnimation ) {
    int totalFrames = aAnimation.getTotalNumFrames();
    FbxTime ttime;
    for( int i = 0; i < totalFrames; i++ ) {
        ofxFBXSkeleton tskeleton = *aSkeleton.get();
        shared_ptr< ofxFBXSkeleton > skeleton( new ofxFBXSkeleton(tskeleton) );
        aAnimation.setFrame( i );
        ttime = aAnimation.getCurrentFbxTime();
        skeleton->updateWithAnimation( ttime, NULL );
        cachedSkeletons.push_back( skeleton );
    }
}

//--------------------------------------------------------------
void ofxFBXCachedSkeletonAnimation::update( int aFrameNum, shared_ptr<ofxFBXSkeleton> aSkeleton ) {
    int tframeNum = ofClamp( aFrameNum, 0, (int)cachedSkeletons.size()-1 );
    if( cachedSkeletons.size() == 0 ) {
        ofLogWarning() << "NO cahced skeletons! returning";
        return;
    }
    
//    cout << "Updating the skeleton from a cache " << aFrameNum << " | " << ofGetElapsedTimef() << endl;
    
    aSkeleton->updateFromCachedSkeleton( cachedSkeletons[tframeNum] );
    
    
}