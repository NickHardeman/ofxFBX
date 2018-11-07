#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    
    ofSetFrameRate(60);
    
    ofDisableArbTex();
    ofLoadImage(teddyTex, "tex.png" );
    
    ofxFBXSceneSettings settings;
    settings.importTextures = false;
    settings.importMaterials = false;
    // must use keyframes when blending animations //
    settings.useKeyFrames = true;
    
    // model from https://www.turbosquid.com/FullPreview/Index.cfm/ID/615227
    string filename = "teddy.fbx";
    
    if( scene.load(filename, settings) ) {
        cout << "ofApp :: loaded the scene OK" << endl;
    } else {
        cout << "ofApp :: Error loading the scene" << endl;
    }
    
    cam.lookAt( ofVec3f(0,0,0) );
    cam.setDistance( 550 );
    cam.setFarClip(6000);
    cam.setNearClip( .5f );
    
    fbxMan.setup( &scene );
    fbxMan.setAnimation(0);
    fbxMan.setScale( 0.15 );
    fbxMan.setPosition(-150, 0, 0 );
    
    fbxManSmooth.setup( &scene );
    fbxManSmooth.setAnimation(0);
    fbxManSmooth.setScale( 0.15 );
    fbxManSmooth.setPosition( 150, 0, 0 );
    
    bRenderNormals  = false;
}

//--------------------------------------------------------------
void ofApp::update() {
    
    fbxMan.update();
    // perform any bone manipulation here //
    fbxMan.lateUpdate();
    
    // align the meshes with y axis //
    for( int i = 0; i < fbxMan.meshTransforms.size(); i++ ) {
        fbxMan.meshTransforms[i].panDeg(-90);
    }
    
    if( fbxMan.getCurrentAnimation().name == "walk" ) {
        fbxMan.panDeg( 1 );
    }
    
    
    fbxManSmooth.update();
    // perform any bone manipulation here //
    fbxManSmooth.lateUpdate();
    
    // align the meshes with y axis //
    for( int i = 0; i < fbxManSmooth.meshTransforms.size(); i++ ) {
        fbxManSmooth.meshTransforms[i].panDeg(-90);
    }
    
    if( fbxManSmooth.getCurrentAnimation().name == "walk" ) {
        if(!fbxManSmooth.isTransitioning()) fbxManSmooth.panDeg( 1 );
    }
    
}

//--------------------------------------------------------------
void ofApp::draw() {
    
    ofSetColor(255, 255, 255);
    ofBackgroundGradient( ofColor::white, ofColor::gray );
    
    glEnable( GL_DEPTH_TEST );
    cam.begin();
    
    ofSetColor( 160 );
    ofPushMatrix();
    ofTranslate( 0, -150, 0 );
    ofRotateYDeg(90);
    ofRotateZDeg(90);
//    float stepSize = 1.25f, size_t numberOfSteps = 8, bool labels = false
    ofDrawGridPlane(40, 50, false );
    ofPopMatrix();
    
    ofEnableLighting();
    light.enable();
    
    ofSetColor( 200 );
    teddyTex.bind();
    fbxMan.draw();
    fbxManSmooth.draw();
    teddyTex.unbind();
    
    light.disable();
    ofDisableLighting();
    
    
    if( bRenderNormals ) {
        ofSetColor( 255, 0, 255 );
        fbxMan.drawMeshNormals( 0.1, false );
    }
    
    
    ofSetColor( light.getDiffuseColor() );
    light.draw();
    
    cam.end();
    
    glDisable( GL_DEPTH_TEST );
    
    int numBones = 0;
    vector< shared_ptr<ofxFBXSkeleton> >& skeletons = fbxMan.getSkeletons();
    for( int i = 0; i < skeletons.size(); i++ ) {
        numBones += skeletons[i]->getNumBones();
    }
    
    ofSetColor( 60, 60, 60 );
    stringstream ds;
    ds << "Render normals (n): " << bRenderNormals << endl;
    ds << "Render " << numBones << " bones " << endl;
    if( fbxMan.areAnimationsEnabled() ) {
        ds << "Toggle play/pause (spacebar): playing: " << fbxMan.getCurrentAnimation().isPlaying() << endl;
        ds << "Previous/Next animation (up/down): " << fbxMan.getCurrentAnimation().name << endl;
    }
//    ds << "Scale is " << fbxMan.getScale() << endl;
//    if( fbxMan.getNumPoses() > 0 ) {
//        ds << "Pose: " << fbxMan.getCurrentPose()->getName() << " num poses: " << fbxMan.getNumPoses() << " enabled (p): " << fbxMan.arePosesEnabled() << endl;
//    }
    ofDrawBitmapString( ds.str(), 50, 30 );
    
    
    for(int i = 0; i < fbxMan.getNumAnimations(); i++ ) {
        stringstream ss;
        ofxFBXAnimation& anim = fbxMan.getAnimation( i );
        if( i == fbxMan.getCurrentAnimationIndex() ) {
            ss << "- ";
        }
        ss << "name: " << anim.name << " " << ofToString(anim.getPositionSeconds(), 3) << " | " << ofToString(anim.getDurationSeconds(), 3) << " frame: " << anim.getFrameNum() << " / " << anim.getTotalNumFrames() << endl;
        ofDrawBitmapString( ss.str(), 50, i * 30 + 650 );
    }
    
    stringstream fs;
    fs << "TEDDY SMOOOOOTH" << endl;
    fs << "transition: " << ofToString(fbxManSmooth.getTransition().percent* 100.0, 0) << "% " << " is transitioning: " << fbxManSmooth.isTransitioning() << endl;
    ofDrawBitmapString( fs.str(), ofGetWidth()/2 + 200, 650 );
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
        int newAnimIndex = fbxMan.getCurrentAnimationIndex()+1;
        if(newAnimIndex > scene.getNumAnimations()-1 ) {
            newAnimIndex = 0;
        }
        fbxMan.setAnimation( newAnimIndex );
        
        fbxManSmooth.transition( newAnimIndex, 1.f );
    }
}

//--------------------------------------------------------------
void ofApp::touchCancelled(ofTouchEventArgs & touch) {
    
}
#else

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if(scene.getNumAnimations() > 1) {
        if(key == OF_KEY_DOWN ) {
            int newAnimIndex = fbxMan.getCurrentAnimationIndex()+1;
            if(newAnimIndex > scene.getNumAnimations()-1 ) {
                newAnimIndex = 0;
            }
            fbxMan.setAnimation( newAnimIndex );
            fbxManSmooth.transition( newAnimIndex, 1.f );
            
        }
        if(key == OF_KEY_UP ) {
            int newAnimIndex = fbxMan.getCurrentAnimationIndex()-1;
            if(newAnimIndex < 0 ) {
                newAnimIndex = scene.getNumAnimations()-1;
            }
            fbxMan.setAnimation( newAnimIndex );
            fbxManSmooth.transition( newAnimIndex, 1.f );
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
    if(key == ' ') {
        fbxMan.getCurrentAnimation().togglePlayPause();
    }
    if(key == 'n') {
        bRenderNormals = !bRenderNormals;
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
















