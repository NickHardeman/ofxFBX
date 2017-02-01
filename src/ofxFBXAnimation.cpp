//
//  ofxFBXAnimation.cpp
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 10/30/13.
//
//

#include "ofxFBXAnimation.h"

//--------------------------------------------------------------
ofxFBXAnimation::ofxFBXAnimation() {
    bPlaying    = true;
    bLoop       = true;
//    fbxname     = NULL;
    bNewFrame   = false;
    _speed      = 1.f;
    setFramerate( 30.f );
    bDone       = false;
}

//--------------------------------------------------------------
ofxFBXAnimation::~ofxFBXAnimation() {
//    if(fbxname != NULL) {
//        delete fbxname;
//        fbxname = NULL;
//    }
}

//--------------------------------------------------------------
void ofxFBXAnimation::setup( FbxTime aStartTime, FbxTime aStopTime, FbxTime aFrameTime ) {
    fbxStartTime    = aStartTime;
    fbxStopTime     = aStopTime;
    fbxFrameTime    = aFrameTime;
    
    startTimeMillis = (unsigned int)fbxStartTime.GetMilliSeconds();
    stopTimeMillis  = (unsigned int)fbxStopTime.GetMilliSeconds();
    frameTimeMillis = (unsigned int)fbxFrameTime.GetMilliSeconds();
    
    fbxCurrentTime  = fbxStartTime;
    
    lastUpdateTimeMillis = -1;
}

//--------------------------------------------------------------
void ofxFBXAnimation::update() {
    bNewFrame = false;
    
//    cout << "speed: " << _speed << endl;
    float tspeed            = _speed;
    if( tspeed < 0 ) tspeed *= -1.f;
    float clampFrameTime    = getFramerate() * tspeed;//ofClamp( getFramerate() * tspeed, 0.0001, 600);
    float tframeTime        = (1.f / clampFrameTime) * 1000.f;
    
    if(bPlaying) {
        uint64_t etimeMillis = ofGetElapsedTimeMillis();
        if( etimeMillis - lastUpdateTimeMillis >= tframeTime ) {
            bNewFrame = true;
            if( _speed >= 0 ) {
                fbxCurrentTime += (fbxFrameTime);
            } else {
                fbxCurrentTime -= (fbxFrameTime);
            }
            lastUpdateTimeMillis = etimeMillis;
        }
    }
    
        
    if( _speed >= 0 ) {
        if(fbxCurrentTime > fbxStopTime ) {
            bDone = true;
            if(bLoop) fbxCurrentTime = fbxStartTime;
            else fbxCurrentTime = fbxStopTime;
        } else {
            bDone = false;
        }
    } else {
        if(fbxCurrentTime < fbxStartTime ) {
            bDone = true;
            if(bLoop) fbxCurrentTime = fbxStopTime;
            else fbxCurrentTime = fbxStartTime;
        } else {
            bDone = false;
        }
    }
}

//--------------------------------------------------------------
bool ofxFBXAnimation::isFrameNew() {
    return bNewFrame;
}

//--------------------------------------------------------------
void ofxFBXAnimation::setSpeed( float aSpeed ) {
    _speed = aSpeed;
}

//--------------------------------------------------------------
float ofxFBXAnimation::getSpeed() {
    return _speed;
}

//--------------------------------------------------------------
void ofxFBXAnimation::play() {
    bPlaying = true;
    if( bDone && !bLoop ) {
        if( _speed >= 0 ) {
            fbxCurrentTime = fbxStartTime;
        } else {
            fbxCurrentTime = fbxStopTime;
        }
    }
    bDone = false;
}

//--------------------------------------------------------------
void ofxFBXAnimation::pause() {
    bPlaying = false;
}

//--------------------------------------------------------------
void ofxFBXAnimation::stop() {
    bPlaying = false;
}

//--------------------------------------------------------------
void ofxFBXAnimation::togglePlayPause() {
    bPlaying = !bPlaying;
}

//--------------------------------------------------------------
bool ofxFBXAnimation::isPlaying() {
    return bPlaying;
}

//--------------------------------------------------------------
bool ofxFBXAnimation::isPaused() {
    return !bPlaying;
}

//--------------------------------------------------------------
bool ofxFBXAnimation::isDone() {
    return bDone;
}

//--------------------------------------------------------------
void ofxFBXAnimation::setLoops( bool aB ) {
    bLoop = aB;
}

//--------------------------------------------------------------
bool ofxFBXAnimation::getLoops() {
    return bLoop;
}

//--------------------------------------------------------------
float ofxFBXAnimation::getPosition() {
    return ofMap( (unsigned int)fbxCurrentTime.GetMilliSeconds(), startTimeMillis, stopTimeMillis, 0, 1, true );
}

//--------------------------------------------------------------
float ofxFBXAnimation::getPositionSeconds() {
    return (float)fbxCurrentTime.GetMilliSeconds() / 1000.f;
}

//--------------------------------------------------------------
unsigned int ofxFBXAnimation::getPositionMillis() {
    return fbxCurrentTime.GetMilliSeconds();
}

//--------------------------------------------------------------
int ofxFBXAnimation::getFrameNum() {
    return round(getPosition() * (float)getTotalNumFrames());
}

//--------------------------------------------------------------
FbxTime ofxFBXAnimation::getCurrentFbxTime() {
    return fbxCurrentTime;
}

//--------------------------------------------------------------
void ofxFBXAnimation::setFramerate( float aFramerate ) {
    _framerate = aFramerate;
}

//--------------------------------------------------------------
float ofxFBXAnimation::getFramerate() {
    return _framerate;
}

//--------------------------------------------------------------
void ofxFBXAnimation::setPosition( float aPct ) {
    int ttime = ofMap(aPct, 0.f, 1.f, startTimeMillis, stopTimeMillis, true );
    fbxCurrentTime.SetMilliSeconds(ttime);
    lastUpdateTimeMillis = -1; // force update of new frame //
}

//--------------------------------------------------------------
void ofxFBXAnimation::setFrame( int aFrameNum ) {
    float tframe    = ofClamp( aFrameNum, 0, getTotalNumFrames() );
    fbxCurrentTime  = fbxStartTime + fbxFrameTime * tframe;
    lastUpdateTimeMillis = -1; // force update of new frame //
}

//--------------------------------------------------------------
unsigned int ofxFBXAnimation::getDurationMillis() {
    return (stopTimeMillis - startTimeMillis);
}

//--------------------------------------------------------------
float ofxFBXAnimation::getDurationSeconds() {
    return (float)getDurationMillis() / 1000.f;
}

//--------------------------------------------------------------
int ofxFBXAnimation::getTotalNumFrames() {
    return ceil( getDurationMillis() / frameTimeMillis );
}







