#pragma once
#include "ofMain.h"

#ifdef TARGET_OPENGLES
#include "ofxiOS.h"
#endif

#include "ofxFBX.h"

#ifdef TARGET_OPENGLES
class ofApp : public ofxiOSApp {
#else
class ofApp : public ofBaseApp {
#endif

public:
    
    void setup();
    void update();
    void draw();
    
#ifdef TARGET_OPENGLES
    void touchDown(ofTouchEventArgs & touch);
    void touchMoved(ofTouchEventArgs & touch);
    void touchUp(ofTouchEventArgs & touch);
    void touchDoubleTap(ofTouchEventArgs & touch);
    void touchCancelled(ofTouchEventArgs & touch);
#else
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
#endif
    
//    ofxFBXScene scene;
    ofEasyCam cam;
    ofxFBX fbx, fbxSmooth;
    
    bool bRenderNormals;
    ofTexture teddyTex;
    
    ofLight light;
    
    
};
