//
//  ofxFBXMesh.h
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 10/31/13.
//
//

#pragma once

#define FBXSDK_NEW_API
/*
#if defined(TARGET_LINUX) && !defined(TARGET_OPENGLES)
    #include <fbxsdk.h>
#endif
*/

#include "ofMain.h"

#include <fbxsdk.h>

/*
#if defined(TARGET_LINUX) 
    #include <fbxsdk.h>
#else
	#include "fbxsdk.h"
#endif
*/


#include "ofVboMesh.h"
#include "ofxFBXNode.h"
#include "ofxFBXBone.h"
#include "ofxFBXCluster.h"

class ofxFBXSubMesh {
public:
    
    ofxFBXSubMesh() {
        triangleCount   = 0;
        indexOffset     = 0;
        totalIndices    = 0;
    }
    
    ~ofxFBXSubMesh() {
        
    }
    
    int triangleCount;
    int indexOffset;
    int totalIndices;
};

class ofxFBXMesh : public ofxFBXNode {
public:
	ofxFBXMesh();
	virtual ~ofxFBXMesh();
    
    void setup( FbxNode * pNode );
	void setFBXMesh( FbxMesh* lMesh );
    void configureMesh( ofMesh& aMesh );
    
    void updateMesh( ofMesh* aMesh, FbxTime& pTime, FbxAnimLayer * pAnimLayer, FbxPose* pPose  );

	void draw( ofMesh* aMesh );
    void drawNormals( ofMesh* aMesh, float length, bool bFaceNormals);
    
    FbxMesh* getFbxMesh() { return fbxMesh;}
    int getNumSubMeshes();
    int getNumMaterials();
    
    bool hasClusterDeformation();
    
	ofVbo& getVbo();
    ofMesh& getOFMesh();

private:
    
    void computeBlendShapes( ofMesh* aMesh, FbxTime& pTime, FbxAnimLayer * pAnimLayer );
    void computeSkinDeformation( FbxAMatrix& pGlobalPosition, FbxTime& pTime, FbxAnimLayer * pAnimLayer, FbxVector4* pVertexArray, FbxPose* pPose );
    void computeLinearDeformation(FbxAMatrix& pGlobalPosition,
                                  FbxMesh* pMesh,
                                  FbxTime& pTime,
                                  FbxVector4* pVertexArray,
                                  FbxPose* pPose );
    void computeDualQuaternionDeformation(FbxAMatrix& pGlobalPosition,
                                          FbxMesh* pMesh,
                                          FbxTime& pTime,
                                          FbxVector4* pVertexArray,
                                          FbxPose* pPose );
    void computeClusterDeformation(FbxAMatrix& pGlobalPosition,
                                   FbxMesh* pMesh,
                                   FbxCluster* pCluster,
                                   FbxAMatrix& pVertexTransformMatrix,
                                   FbxTime pTime,
                                   FbxPose* pPose);
    
    vector<ofxFBXSubMesh> subMeshes;
    ofVbo veebs;
	ofMesh mesh;
	ofMesh original;
	FbxMesh* fbxMesh;
    bool bAllMappedByControlPoint;
};

