//
//  ofxFBXAnimation.h
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 10/30/13.
//
//

#pragma once
#include "ofMain.h"
#include <math.h>
#include <fbxsdk.h>

class ofxFBXAnimation {
public:
    ofxFBXAnimation();
    ~ofxFBXAnimation();
    
    void setup( FbxTime aStartTime, FbxTime aStopTime, FbxTime aFrameTime );
    
    void update(float aElapsedTimeSeconds=-1);
    
    bool isFrameNew();
    void setSpeed( float aSpeed );
    float getSpeed();
    
    void play();
    void stop();
    void pause();
    void togglePlayPause();
    bool isPlaying();
    bool isPaused();
    bool isDone();
    
    void setLoops( bool aB );
    bool getLoops();
    
    float getPosition();
    float getPositionSeconds();
    unsigned int getPositionMillis();
    int getFrameNum();
    FbxTime getCurrentFbxTime();
    
    void setFramerate( float aFramerate );
    float getFramerate();
    
    void setPosition( float aPct );
    void setFrame( int aFrameNum );
    
    unsigned int getDurationMillis();
    float getDurationSeconds();
    
    int getTotalNumFrames();
    
    string name = "";
//    int ID;
    int index = 0;
    FbxString fbxname;
    
    unsigned int startTimeMillis=0, stopTimeMillis=1000, frameTimeMillis=1/30*1000;
    
    FbxTime fbxStartTime, fbxStopTime, fbxCurrentTime, fbxFrameTime;
    
protected:
    bool bPlaying = true;
    bool bLoop = true;
    bool bNewFrame = true;
    bool bDone = false;
    float _speed = 1.f;
    float _framerate = 30;
    
    int lastUpdateTimeMillis = -1;
};






