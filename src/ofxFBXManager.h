//
//  ofxFBXManager.h
//  ScenesSetup_Nick
//
//  Created by Nick Hardeman on 12/20/13.
//
//

#pragma once
#include "ofxFBX.h"

class ofxFBXManager : public ofNode {
public:
    ofxFBXManager();
    
    void setup( ofxFBXScene* aScene );
    void update();
    void lateUpdate();
    
    void draw();
    void drawMeshes();
    void drawMeshWireframes();
    void drawMeshNormals( float aLen = 4.f, bool aBFaceNormals = true );
    void drawSkeletons( float aLen = 10.f, bool aBDrawAxes = true );
    
    vector< ofMesh >& getMeshes();
    int getNumMeshes();
    string getMeshName( int aMeshIndex );
    
    ofxFBXScene* getFbxScene();
    
    int getNumAnimations();
    int getCurrentAnimationIndex();
    int getAnimationIndex( string aname );
    ofxFBXAnimation& getCurrentAnimation();
    ofxFBXAnimation& getAnimation( int aIndex );
    ofxFBXAnimation& getAnimation( string aname );
    void setAnimation( int aIndex );
    void setAnimation( string aname );
    void enableAnimations();
    void disableAnimations();
    void toggleAnimationsEnabled();
    bool areAnimationsEnabled();
    bool hasAnimations();
    
    vector< shared_ptr<ofxFBXSkeleton> >& getSkeletons();
    bool hasBones();
    int getNumSkeletons();
    int getNumBones();
    ofxFBXBone* getBone( string aBoneName, int aSkeletonIndex=0 );
    string getSkeletonInfo();
    
    bool hasPoses();
    bool arePosesEnabled();
    void enablePoses();
    void disablePoses();
    void togglePosesEnabled();
    int getNumPoses();
    void setPoseIndex( int aIndex );
    shared_ptr< ofxFBXPose > getCurrentPose();
    vector< shared_ptr< ofxFBXPose > > getPoses();
    
    vector<ofNode> meshTransforms;
    
    
protected:
    vector< shared_ptr<ofxFBXSkeleton> > skeletons;
    vector<ofMesh> meshes;
    vector< shared_ptr<ofxFBXPose> > poses;
    
    vector<ofxFBXAnimation> animations;
    
    FbxAnimLayer* currentFbxAnimationLayer;
    FbxAnimStack* currentAnimationStack;
    ofxFBXScene* fbxScene;
    
    int animationIndex;
    bool bAnimationsEnabled;
    bool bSkeletonsDirty;
    int poseIndex;
    bool bPosesEnabled;
};







