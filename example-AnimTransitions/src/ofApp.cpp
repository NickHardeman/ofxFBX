#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    
    ofSetBackgroundColor( 220 );
    
    ofSetLogLevel( OF_LOG_VERBOSE );
    
    ofSetFrameRate(60);
    
    ofDisableArbTex();
    if(!ofLoadImage(teddyTex, "tex.png" )) {
        ofLogError("Could not load tex.png texture");
    }
    
    ofxFBXSource::Scene::Settings settings;
    settings.importTextures = false;
    settings.importMaterials = false;
    // must use keyframes when blending animations //
    settings.useKeyFrames = true; // on by default //
    // model from https://www.turbosquid.com/FullPreview/Index.cfm/ID/615227
    settings.filePath = "teddy.fbx";
    // we don't want to manipulate the bones, so caching the meshes will reduce the bone transform calls
    // and should speed it up with a lot of the same animated meshes that have bones //
    settings.cacheMeshKeyframes = true;
    
    cam.lookAt( ofVec3f(0,0,0) );
    cam.setDistance( 550 );
    cam.setFarClip(6000);
    cam.setNearClip( .5f );
    
//    fbxMan.setup( &scene );
    fbx.load( settings );
    fbx.setAnimation(0);
    fbx.setScale( 0.15 );
    fbx.setPosition(-150, 0, 0 );
//    fbx.cacheMeshKeyframes(true);
    
//    fbxManSmooth.setup( &scene );
    fbxSmooth.load( settings );
    fbxSmooth.setAnimation(0);
    fbxSmooth.setScale( 0.15 );
    fbxSmooth.setPosition( 150, 0, 0 );
//    fbxSmooth.cacheMeshKeyframes(true);
    
    for( int i = 0; i < fbx.getNumMeshes(); i++ ) {
        cout << i << " - " << " num tex coords: " << fbx.getMeshes()[i]->getMesh().getNumTexCoords() << endl;
        
    }
    fbx.setMaterialsEnabled(false);
    
    bRenderNormals  = false;
}

//--------------------------------------------------------------
void ofApp::update() {
    
    fbx.earlyUpdate();
    
    // align the meshes with y axis //
    for( int i = 0; i < fbx.getMeshes().size(); i++ ) {
        fbx.getMeshes()[i]->panDeg(-90);
    }
    
    if( fbx.getCurrentAnimation().name == "walk" ) {
        fbx.panDeg( 1 );
    }
    fbx.lateUpdate();
    
    // update internally calls earlyUpdate and then lateUpdate
    fbxSmooth.update();
    
    // align the meshes with y axis //
    for( int i = 0; i < fbxSmooth.getMeshes().size(); i++ ) {
        fbxSmooth.getMeshes()[i]->panDeg(-90);
    }
    
    if( fbxSmooth.getCurrentAnimation().name == "walk" ) {
        if(!fbxSmooth.isTransitioning()) fbxSmooth.panDeg( 1 );
    }
    
}

//--------------------------------------------------------------
void ofApp::draw() {
    
    ofSetColor(255, 255, 255);
    // this causes the texture not to render on the models on iOS
    #ifndef TARGET_OPENGLES
    ofBackgroundGradient( ofColor::white, ofColor::gray );
    #endif
    
    ofEnableDepthTest();
    cam.begin(); {
        // this causes the texture not to render on the models on iOS
        #ifndef TARGET_OPENGLES
        ofSetColor( 160 );
        ofPushMatrix(); {
            ofTranslate( 0, -150, 0 );
            ofRotateYDeg(90);
            ofRotateZDeg(90);
        //    float stepSize = 1.25f, size_t numberOfSteps = 8, bool labels = false
            ofDrawGridPlane(40, 50, false );
        } ofPopMatrix();
        #endif
        
        ofEnableLighting();
        light.enable();
        
        ofSetColor( 200 );
        teddyTex.bind();
        fbx.draw();
        fbxSmooth.draw();
        teddyTex.unbind();
        
        light.disable();
        ofDisableLighting();
        
        
        if( bRenderNormals ) {
            ofSetColor( 255, 0, 255 );
            fbx.drawMeshNormals( 0.1, false );
        }
        
        
        ofSetColor( light.getDiffuseColor() );
        light.draw();
        
    } cam.end();
    
    ofDisableDepthTest();
    
    int numBones = fbx.getNumBones();
    
    ofSetColor( 60, 60, 60 );
    stringstream ds;
    ds << "Render normals (n): " << bRenderNormals << endl;
    ds << "Render " << numBones << " bones " << endl;
    if( fbx.areAnimationsEnabled() ) {
        ds << "Toggle play/pause (spacebar): playing: " << fbx.getCurrentAnimation().isPlaying() << endl;
        ds << "Previous/Next animation (up/down): " << fbx.getCurrentAnimation().name << endl;
    }
//    ds << "Scale is " << fbxMan.getScale() << endl;
//    if( fbxMan.getNumPoses() > 0 ) {
//        ds << "Pose: " << fbxMan.getCurrentPose()->getName() << " num poses: " << fbxMan.getNumPoses() << " enabled (p): " << fbxMan.arePosesEnabled() << endl;
//    }
    ofDrawBitmapString( ds.str(), 50, 30 );
    
    
    for(int i = 0; i < fbx.getNumAnimations(); i++ ) {
        stringstream ss;
        ofxFBXAnimation& anim = fbx.getAnimation( i );
        if( i == fbx.getCurrentAnimationIndex() ) {
            ss << "- ";
        }
        ss << "name: " << anim.name << " " << ofToString(anim.getPositionSeconds(), 3) << " | " << ofToString(anim.getDurationSeconds(), 3) << " frame: " << anim.getFrameNum() << " / " << anim.getTotalNumFrames() << endl;
        ofDrawBitmapString( ss.str(), 50, i * 30 + 650 );
    }
    
    stringstream fs;
    fs << "TEDDY SMOOOOOTH" << endl;
    fs << "transition: " << ofToString(fbxSmooth.getTransition().percent* 100.0, 0) << "% " << " is transitioning: " << fbxSmooth.isTransitioning() << endl;
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
    if(fbx.getNumAnimations() > 1) {
        int newAnimIndex = fbx.getCurrentAnimationIndex()+1;
        if(newAnimIndex > fbx.getNumAnimations()-1 ) {
            newAnimIndex = 0;
        }
        fbx.setAnimation( newAnimIndex );
        fbxSmooth.transition( newAnimIndex, 1.f );
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
            fbxSmooth.transition( newAnimIndex, 1.f );
            
        }
        if(key == OF_KEY_UP ) {
            int newAnimIndex = fbx.getCurrentAnimationIndex()-1;
            if(newAnimIndex < 0 ) {
                newAnimIndex = fbx.getNumAnimations()-1;
            }
            fbx.setAnimation( newAnimIndex );
            fbxSmooth.transition( newAnimIndex, 1.f );
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
















