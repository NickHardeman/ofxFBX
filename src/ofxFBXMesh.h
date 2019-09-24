//
//  ofxFBXMesh.h
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 10/31/13.
//
//

#pragma once
#define FBXSDK_NEW_API
#include "ofVboMesh.h"
#include "ofxFBXNode.h"
#include "ofxFBXSrcMesh.h"
#include "ofxFBXMeshMaterial.h"

class ofxFBXMesh : public ofxFBXNode {
public:
    
    ofxFBXSource::Node::NodeType getType() override;
    
    void setup( shared_ptr<ofxFBXSource::Node> anode ) override;
    
    void update( FbxTime& pTime, FbxPose* pPose ) override;
    void update( int aAnimIndex, signed long aMillis ) override;
    void update( int aAnimIndex1, signed long aAnim1Millis, int aAnimIndex2, signed long aAnim2Millis, float aMixPct ) override;
    
    void update() override;
    void lateUpdate(FbxTime& pTime, FbxAnimLayer * pAnimLayer, FbxPose* pPose) override;
    
    void draw();
    void drawWireframe();
    void drawNormals( float length, bool bFaceNormals);
    
    ofVbo& getVbo();
    ofMesh& getMesh();
    int getNumMaterials();
    vector< shared_ptr<ofxFBXMeshMaterial> > getMaterials();
    void setMaterialsEnabled(bool ab);
    
    vector< shared_ptr<ofxFBXSource::MeshTexture> > getTextures();
    bool hasTexture();
    
    void setMeshDirty( bool ab ) { bMeshDirty=ab; }
    void setBlendMeshFrames( bool ab ) { bBlendMeshFrames=ab; }
    
    shared_ptr<ofxFBXSource::Mesh> getSourceMesh() { _checkSrcMesh(); return mSrcMesh; }
    // returns a copy of the mesh with the vertices transformed into Global / World space //
    ofMesh getGlobalMesh();
    // returns a copy of the mesh with the vertices transformed into Global / World space around the position //
    ofMesh getGlobalMeshAroundPosition();
    // returns a
    ofMesh getMeshAroundPositionScaleApplied();

private:
    void _checkSrcMesh();
    
    static ofVbo dummyVbo;
    ofMesh mesh, normalsMesh;
    shared_ptr<ofxFBXSource::Mesh> mSrcMesh;
    
    vector< shared_ptr<ofxFBXMeshMaterial> > mMaterials;
    
    bool bHasTexture = false;
    
    signed long mLastFbxTimeMillis = -99999;// = (unsigned int)fbxFrameTime.GetMilliSeconds();
    bool bMeshDirty = false;
    int mLastAnimIndex = 0;
    bool bDrawMeshKeyframe = false;
    bool bBlendMeshFrames = false;
};

