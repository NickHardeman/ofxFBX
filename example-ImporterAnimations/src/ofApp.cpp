#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    ofDisableArbTex();
    
    ofxFBXSource::Scene::Settings settings;//ofxFBXSceneSettings settings;
    settings.filePath = "humanoid.fbx";
    settings.printInfo = true;
    
    if( fbx.load(settings) ) {
        cout << "ofApp :: loaded the scene OK" << endl;
    } else {
        cout << "ofApp :: Error loading the scene" << endl;
    }
    
    cam.lookAt( ofVec3f(0,0,0) );
    cam.setDistance( 550 );
    cam.setFarClip(6000);
    cam.setNearClip( .5f );
    
//    fbxMan.setup( &scene );
    fbx.setAnimation(0);
    
    bRenderNormals  = false;
    bRenderMeshes   = true;
    bDrawBones      = false;
    
    cout << "-------------------------------------" << endl;
    cout << fbx.getInfoString() << endl;
    
}

//--------------------------------------------------------------
void ofApp::update() {
    fbx.update();
}

//--------------------------------------------------------------
void ofApp::draw() {
    
    ofSetColor(255, 255, 255);
    
    glEnable( GL_DEPTH_TEST );
    
    
    cam.begin();
    
    ofEnableLighting();
    light.enable();
    
    if( bRenderMeshes ) {
        ofSetColor( 255, 255, 255 );
        fbx.draw();
    }
    
    light.disable();
    ofDisableLighting();
    
    if(bDrawBones) {
        fbx.drawSkeletons( 6. );
    }
    
    if( bRenderNormals ) {
        ofSetColor( 255, 0, 255 );
        fbx.drawMeshNormals( 1, false );
    }
    
    
    ofSetColor( light.getDiffuseColor() );
    light.draw();
    
    cam.end();
    
    glDisable( GL_DEPTH_TEST );
    
    int numBones = fbx.getNumBones();
//    vector< shared_ptr<ofxFBXSkeleton> >& skeletons = fbxMan.getSkeletons();
//    for( int i = 0; i < skeletons.size(); i++ ) {
//        numBones += skeletons[i]->getNumBones();
//    }
    
    ofSetColor( 60, 60, 60 );
    stringstream ds;
    ds << "Render normals (n): " << bRenderNormals << endl;
    ds << "Render meshes (m): " << bRenderMeshes << endl;
    ds << "Render " << numBones << " bones (b): " << bDrawBones << endl;
    if( fbx.areAnimationsEnabled() ) {
        ds << "Toggle play/pause (spacebar): playing: " << fbx.getCurrentAnimation().isPlaying() << endl;
        ds << "Previous/Next animation (up/down): " << fbx.getCurrentAnimation().name << endl;
    }
    ds << "Scale is " << fbx.getScale() << endl;
    if( fbx.getNumPoses() > 0 ) {
        ds << "Pose: " << fbx.getCurrentPose()->getName() << " num poses: " << fbx.getNumPoses() << " enabled (p): " << fbx.arePosesEnabled() << endl;
    }
    ofDrawBitmapString( ds.str(), 50, 30 );
    
    
    for(int i = 0; i < fbx.getNumAnimations(); i++ ) {
        stringstream ss;
        ofxFBXAnimation& anim = fbx.getAnimation( i );
        if( i == fbx.getCurrentAnimationIndex() ) {
            ss << "- ";
        }
        ss << "name: " << anim.name << " " << ofToString(anim.getPositionSeconds(), 3) << " | " << ofToString(anim.getDurationSeconds(), 3) << " frame: " << anim.getFrameNum() << " / " << anim.getTotalNumFrames() << endl;
        ofDrawBitmapString( ss.str(), 50, i * 30 + 450 );
    }
}

#ifdef TARGET_OPENGLES
//--------------------------------------------------------------
void ofApp::touchDown(ofTouchEventArgs & touch) {
    
}

//--------------------------------------------------------------
void ofApp::touchMoved(ofTouchEventArgs & touch) {
    
}

//--------------------------------------------------------------
void ofApp::touchUp(ofTouchEventArgs & touch) {
    
}

//--------------------------------------------------------------
void ofApp::touchDoubleTap(ofTouchEventArgs & touch) {
    if(scene.getNumAnimations() > 1) {
        int newAnimIndex = fbx.getCurrentAnimationIndex()+1;
        if(newAnimIndex > scene.getNumAnimations()-1 ) {
            newAnimIndex = 0;
        }
        fbx.setAnimation( newAnimIndex );
    }
}

//--------------------------------------------------------------
void ofApp::touchCancelled(ofTouchEventArgs & touch) {
    
}
#else

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if(fbx.getNumAnimations() > 1) {
        if(key == OF_KEY_DOWN ) {
            int newAnimIndex = fbx.getCurrentAnimationIndex()+1;
            if(newAnimIndex > fbx.getNumAnimations()-1 ) {
                newAnimIndex = 0;
            }
            fbx.setAnimation( newAnimIndex );
            
        }
        if(key == OF_KEY_UP ) {
            int newAnimIndex = fbx.getCurrentAnimationIndex()-1;
            if(newAnimIndex < 0 ) {
                newAnimIndex = fbx.getNumAnimations()-1;
            }
            fbx.setAnimation( newAnimIndex );
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
    if(key == ' ') {
        fbx.getCurrentAnimation().togglePlayPause();
    }
    if(key == 'n') {
        bRenderNormals = !bRenderNormals;
    }
    if(key == 'm') {
        bRenderMeshes = !bRenderMeshes;
    }
    if(key == 'b') {
        bDrawBones = !bDrawBones;
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)    {
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {
    
}
#endif
















