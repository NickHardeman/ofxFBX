//
//  ofxFBXSrcMesh.h
//  ConnectionsWall-Nick
//
//  Created by Nick Hardeman on 7/11/19.
//

#pragma once
#define FBXSDK_NEW_API

#include "ofMain.h"
#include <fbxsdk.h>

#include "ofxFBXSrcNode.h"
#include "ofxFBXSrcBone.h"
#include "ofxFBXSrcCluster.h"
#include "ofxFBXSrcMeshMaterial.h"
#include "ofxFBXMeshMaterial.h"

namespace ofxFBXSource {
class SubMesh {
public:
    
    SubMesh() {
        triangleCount   = 0;
        indexOffset     = 0;
        totalIndices    = 0;
        materialPtr     = NULL;
    }
    
    ~SubMesh() {}
    
    ofxFBXSource::MeshMaterial* materialPtr;
    int triangleCount;
    int indexOffset;
    int totalIndices;
};
    
class MeshAnimKey {
public:
    // hold all of the data for that key //
    ofMesh mesh;
    signed long millis;
};

class MeshAnimKeyCollection {
public:
    string name;
    vector<MeshAnimKey> meshKeys;
};

class Mesh : public ofxFBXSource::Node {
public:
    Mesh();
    virtual ~Mesh();
    
    ofxFBXSource::Node::NodeType getType() override { return OFX_FBX_MESH; }
    
    void setup( FbxNode * pNode ) override;
    void setFBXMesh( FbxMesh* lMesh );
    void configureMesh( ofMesh& aMesh );
    
    void update( FbxTime& pTime, FbxPose* pPose ) override;
    virtual void update( int aAnimIndex, signed long aMillis ) override;
    virtual void update( int aAnimIndex1, signed long aAnim1Millis, int aAnimIndex2, signed long aAnim2Millis, float aMixPct ) override;
    
    void updateMeshFromKeyframes( ofMesh* amesh, int aAnimIndex, signed long aMillis );
    void updateMeshFromKeyframesBlended( ofMesh* amesh, int aAnimIndex, signed long aMillis );
    void updateMeshFromKeyframes( ofMesh* amesh, int aAnimIndex1, signed long aAnim1Millis, int aAnimIndex2, signed long aAnim2Millis, float aMixPct );
    void updateMesh( ofMesh* aMesh, FbxTime& pTime, FbxAnimLayer * pAnimLayer, FbxPose* pPose  );
    
    void draw( ofMesh* aMesh );
    void draw( ofMesh* aMesh, vector< shared_ptr<ofxFBXMeshMaterial> >& aMats, bool bUpdateTheVeeeeebs );
    void drawMeshKeyframe( int aAnimIndex, signed long aMillis, vector< shared_ptr<ofxFBXMeshMaterial> >& aMats, bool bUpdateTheVeeeeebs );
    
    FbxMesh* getFbxMesh() { return fbxMesh; }
    int getNumSubMeshes();
    
    const vector<ofxFBXSource::SubMesh>& getSubMeshes() const { return subMeshes; }
    
    int getNumMaterials();
    vector< ofxFBXSource::MeshMaterial* > getMaterials();
    
    bool hasTexture();
    vector< shared_ptr<ofxFBXSource::MeshTexture> > getTextures();
    
    bool hasClusterDeformation();
    
    ofVbo& getVbo();
    ofMesh& getOFMesh();
    
    // mesh keys //
    MeshAnimKeyCollection& getMeshKeyCollection( int aAnimIndex );
    bool hasMeshKeyCollection( int aAnimIndex );
    virtual void clearKeyFrames() override;
    void setUsingCachedMeshes( bool ab );
    bool isUsingCachedMeshes();
    MeshAnimKey& getMeshAnimKey( int aAnimIndex, signed long aMillis );
    MeshAnimKey& getMeshAnimKeyBlended( int aAnimIndex, signed long aMillis );
    
private:
    void _configureMeshFromSrcMesh( ofMesh* aSrcMesh, ofMesh* amesh );
    void _updateMeshFromKeyMesh( ofMesh* aSrcMesh, ofMesh* amesh );
    void computeBlendShapes( ofMesh* aMesh, FbxTime& pTime, FbxAnimLayer * pAnimLayer );
    void computeSkinDeformation( FbxAMatrix& pGlobalPosition, FbxTime& pTime, FbxAnimLayer * pAnimLayer, FbxVector4* pVertexArray, FbxVector4* pNormalsArray, FbxPose* pPose );
    void computeLinearDeformation(FbxAMatrix& pGlobalPosition,
                                  FbxMesh* pMesh,
                                  FbxTime& pTime,
                                  FbxVector4* pVertexArray,
                                  FbxPose* pPose,
                                  bool bNormals);
    void computeDualQuaternionDeformation(FbxAMatrix& pGlobalPosition,
                                          FbxMesh* pMesh,
                                          FbxTime& pTime,
                                          FbxVector4* pVertexArray,
                                          FbxPose* pPose,
                                          bool bNormals);
    void computeClusterDeformation(FbxAMatrix& pGlobalPosition,
                                   FbxMesh* pMesh,
                                   FbxCluster* pCluster,
                                   FbxAMatrix& pVertexTransformMatrix,
                                   FbxTime pTime,
                                   FbxPose* pPose,
                                   bool bNormals);
    
    void populateNormals( FbxVector4* pNormalsArray );
    
    vector<SubMesh> subMeshes;
    ofVbo veebs;
    ofMesh mesh;
    ofMesh original;
    FbxMesh* fbxMesh = NULL;
    FbxVector4* mNormalsArray = NULL;
    
    FbxGeometryElement::EMappingMode mNormalMappingMode = FbxGeometryElement::eNone;
    map<int, MeshAnimKeyCollection> mMeshKeyCollections;
    bool bUsingCachedMeshes = false;
    MeshAnimKey dummyMeshAnimKey, mBlendedAnimKey;
};
    
}
