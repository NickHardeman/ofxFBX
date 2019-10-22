#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    ofDisableArbTex();
    
    ofSetBackgroundColor( 220 );
    
    // uncomment this to convert scene to meters //
    //ofxFBXSource::Scene::FbxUnits = FbxSystemUnit::m;
    
    ofxFBXSource::Scene::Settings settings;
    settings.filePath = "astroBoy_walk.fbx";
//    settings.useKeyFrames = false;
    settings.printInfo = true;
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    if( fbx.load(settings) ) {
        cout << "ofApp :: loaded the scene OK" << endl;
    } else {
        cout << "ofApp :: Error loading the scene" << endl;
    }
    
    cam.lookAt( ofVec3f(0,0,0) );
    cam.setDistance( 20 );
    cam.setFarClip(100);
    cam.setNearClip( .5f );
    
    #ifdef TARGET_OPENGLES
    cam.disableMouseInput();
    #endif
    
    fbx.setAnimation(0);
    fbx.setPosition( 0, -7, 0 );
    
    //cout << fbx.getSkeletonInfo() << endl;
    
    bRenderNormals  = false;
    bRenderMeshes   = true;
    bDrawBones      = false;
}

//--------------------------------------------------------------
void ofApp::update() {
    
    light.setPosition( cos(ofGetElapsedTimef()*2.) * 7, 4 + sin( ofGetElapsedTimef() ) * 2.5, 10  );
    
    ofVec3f target( ofMap( ofGetMouseX(), 0, ofGetWidth(), -10, 10, true), fbx.getPosition().y, fbx.getPosition().z+10 );
    fbx.lookAt( target );
    fbx.panDeg( 180 );
    
    fbx.getCurrentAnimation().setSpeed( ofMap( ofGetMouseY(), 100, ofGetHeight()-100, 0.5, 2.5, true ));
    
//    fbx.update();
    
    #ifndef TARGET_OPENGLES
    // change colors of the materials //
    for( int i = 0; i < fbx.getNumMeshes(); i++ ) {
//        cout << i << " - " << fbx.getMeshes()[i]->getName() << " num materials: " << fbx.getMeshes()[i]->getNumMaterials() << endl;
        if( fbx.getMeshes()[i]->getNumMaterials() > 2 ) {
            auto mat = fbx.getMeshes()[i]->getMaterials()[2];
            mat->setDiffuseColor( ofFloatColor(sin(ofGetElapsedTimef()*4.)*0.5+0.5, 1, 1, 1 ));
            mat->disableTextures();
        }
    }
    #endif
    
    
    // moves the bones into place based on the animation //
    fbx.earlyUpdate();

    // perform any bone manipulation here //
    shared_ptr<ofxFBXBone> bone = fbx.getBone("head");
    if( bone ) {
        bone->pointTo( light.getPosition(), ofVec3f(-1,0,0) ) ;
    }
    
    // manipulates the mesh around the positioned bones //
    fbx.lateUpdate();
    
}

//--------------------------------------------------------------
void ofApp::draw() {
    
    ofSetColor(255, 255, 255);
    
    ofEnableDepthTest();
    
    
    cam.begin(); {
    
        ofEnableLighting();
        light.enable();
        
        if( bRenderMeshes ) {
            ofSetColor( 255, 255, 255 );
            fbx.draw();
        }

        light.disable();
        ofDisableLighting();

        if(bDrawBones) {
            fbx.drawSkeletons( 0.5 );
        }

        if( bRenderNormals ) {
            ofSetColor( 255, 0, 255 );
            fbx.drawMeshNormals( 0.5, false );
        }
        
        ofNoFill();
        ofSetColor( 50, 50, 50 );
        ofDrawBox( 0, 0, 0, 14 );
        ofFill();
        
        ofSetColor( light.getDiffuseColor() );
        ofDrawSphere( light.getPosition(), 1 );
        
    } cam.end();
    
    ofDisableDepthTest();
    
    int numBones = fbx.getNumBones();
    
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
    if( bRenderMeshes ) {
        bRenderMeshes = false;
        bDrawBones = true;
    } else {
        bRenderMeshes = true;
        bDrawBones = false;
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















