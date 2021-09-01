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
    
    startTimeMillis = fbxStartTime.GetMilliSeconds();
    stopTimeMillis  = fbxStopTime.GetMilliSeconds();
    frameTimeMillis = fbxFrameTime.GetMilliSeconds();
    
//    cout << "ofxFbxAnimation :: setup : " << name << " _framerate: " << _framerate << " frtime: " << (1.f/_framerate) << " fbxframetime: " << ((float)frameTimeMillis / 1000.0f) << " fbx frame rate: " << (1.f/((float)frameTimeMillis / 1000.0f)) << endl;
    
    fbxCurrentTime  = fbxStartTime;
    
    lastUpdateTimeMillis = -1;
}

//--------------------------------------------------------------
void ofxFBXAnimation::update( float aElapsedTimeSeconds ) {
    bNewFrame = false;
    
    if( bDone && bLoop ) {
        if( _speed >= 0.0 ) {
            fbxCurrentTime = fbxStartTime;
        } else {
            fbxCurrentTime = fbxStopTime;
        }
        bNewFrame = true;
        mFrameTimeDelta = 0.0;
    }
    
//    cout << "speed: " << _speed << endl;
    float tspeed            = _speed;
    if( tspeed < 0 ) tspeed *= -1.f;
    float clampFrameTime    = getFramerate() * tspeed;//ofClamp( getFramerate() * tspeed, 0.0001, 600);
    int tframeTime        = (1.f / clampFrameTime) * 1000;
    
    uint64_t etimeMillis;// = ofGetElapsedTimeMillis();
    if (aElapsedTimeSeconds < 0) {
        etimeMillis = ofGetElapsedTimeMillis();
    }
    else {
        etimeMillis = aElapsedTimeSeconds * 1000.f;
    }
    
    if(lastUpdateTimeMillis < 0 ) {
        lastUpdateTimeMillis = etimeMillis;
    }
    
    if(bPlaying) {
		
		//int etimeMillis = aElapsedTimeSeconds * 1000;
		//cout << "etimeMillis: " << etimeMillis << " argMillis: " << (aElapsedTimeSeconds * 1000) << " secs: " << aElapsedTimeSeconds << " | " << ofGetFrameNum() << endl;
        
        uint64_t ftime = (etimeMillis - lastUpdateTimeMillis);
        
        mFbxFrameDeltaTime.SetMilliSeconds(ftime * tspeed);
//        fbxCurrentTime
        mFrameTimeDelta += ftime;
        
//        if( tspeed > 1.0 ){
//            fbxCurrentTime += (mFbxFrameDeltaTime) * tspeed;
//        }else{
            if( _speed >= 0 ) {
                fbxCurrentTime += (mFbxFrameDeltaTime);
            } else {
                fbxCurrentTime -= (mFbxFrameDeltaTime);
            }
//        }
        
        if( mFrameTimeDelta >= tframeTime ) {
            mFrameTimeDelta -= tframeTime;
            bNewFrame = true;
        }
        
        lastUpdateTimeMillis = etimeMillis;
        
    } else {
        mFrameTimeDelta = 0;
        lastUpdateTimeMillis = etimeMillis;
    }
    
        
    if( _speed >= 0 ) {
        if(fbxCurrentTime >= fbxStopTime ) {
            bDone = true;
            if(bLoop) {
                // just in case //
                bNewFrame = true;
//                fbxCurrentTime = fbxStartTime;
                // we check for the loop at the beginning of update
                fbxCurrentTime = fbxStopTime;
            } else {
                fbxCurrentTime = fbxStopTime;
            }
        } else {
            bDone = false;
        }
    } else {
        if(fbxCurrentTime < fbxStartTime ) {
            bDone = true;
            if(bLoop) {
                fbxCurrentTime = fbxStartTime;
//                fbxCurrentTime = fbxStopTime;
                bNewFrame = true;
            } else {
                fbxCurrentTime = fbxStartTime;
            }
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
    mFrameTimeDelta = 0;
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
    
    mFrameTimeDelta = 0;
    
    if( _speed >= 0 ) {
        if(fbxCurrentTime >= fbxStopTime || aPct >= 1.0f ) {
            bDone = true;
        } else {
            bDone = false;
        }
    } else {
        if(fbxCurrentTime < fbxStartTime ) {
            bDone = true;
        } else {
            bDone = false;
        }
    }
}

//--------------------------------------------------------------
void ofxFBXAnimation::setFrame( int aFrameNum ) {
    float tframe    = ofClamp( aFrameNum, 0, getTotalNumFrames() );
    setPosition( tframe / ((float)getTotalNumFrames()-1.f) );
//    fbxCurrentTime  = fbxStartTime + fbxFrameTime * tframe;
//    lastUpdateTimeMillis = -1; // force update of new frame //
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







