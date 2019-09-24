//
//  ofxFBXMesh.h
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 10/31/13.
//
//


#pragma once
#define FBXSDK_NEW_API
#include "ofMain.h"
#include <fbxsdk.h>

#include "ofxFBXUtils.h"
#include "ofxFBXAnimation.h"
#include "ofxFBXSrcMesh.h"
#include "ofxFBXSrcBone.h"
#include "ofxFBXSrcMeshMaterial.h"
#include "ofxFBXSrcCluster.h"
#include "ofxFBXSrcSkeleton.h"
#include "ofxFBXSrcPose.h"
#include "ofxFBXSrcNurbsCurve.h"

namespace ofxFBXSource {

class Scene {
public:
    
    class Settings {
    public:
        Settings() {}
        ~Settings() {}
        
        bool importBones = true;
        bool importMaterials = true;
        bool importTextures = true;
        bool importShapes = true;
        bool importGobos = true;
        bool importAnimations = true;
        bool importNurbsCurves = true;
        bool useKeyFrames = true;
        bool printInfo = false;
        string filePath = "";
        // used if load called directly from ofxFBX. ofxFBX will create an internal instance
        // setting to true will remove that internal instance when nothing is using it.
        bool unloadIfUnused = false;
        // creates a cache of meshes for each keyframe, reduces the number of bone transform calls for speed.
        // will not work if you want to dynamically manipulate the bones //
        bool cacheMeshKeyframes = false;
        // if the meshes are cached, should they be blended between frames, heavier cpu use, smoother appearance //
        bool blendCachedMeshKeyframes = false;
    };
    
    Scene();
    ~Scene();
    
    static FbxSystemUnit FbxUnits;
    
    bool load( Settings aSettings );
	bool load( string path, Settings aSettings = Settings() );
    Settings& getSettings() { return _settings; }
    
    FbxScene* getFBXScene();
    
    int getNumAnimations();
    bool areAnimationsEnabled();
    void populateAnimations( vector<ofxFBXAnimation>& aInVector );
    void clearAnimations();
    ofxFBXAnimation& addAnimation( string aname, int aFrameBegin, int aFrameEnd, int aAnimStackIndex=0 );
    
    int getNumPoses();
    void populatePoses( vector< shared_ptr<ofxFBXSource::Pose> >& aInPoses );
    
    string getFbxFilePath();
    string getFbxFolderPath();
    
    ofxFBXSource::Node::NodeType getOFXNodeType( FbxNode* aNode );
    vector< shared_ptr<ofxFBXSource::Node> > getSceneNodes();
    
    string getInfoString();
    void setMeshKeyframesCached( bool ab ) { bMeshKeyframesCached = ab; }
    bool areMeshKeyframesCached() { return bMeshKeyframesCached; }
    
private:
    void _parseNodesRecursive( FbxNode* aNode );
    void _constructSkeletons( FbxNode* aNode );
    void _getSkeletonBases( FbxNode* aNode, list<FbxNode*>& aSkeletonBases );
    shared_ptr<ofxFBXSource::Mesh> _getMeshForNode( FbxNode* aNode );
    shared_ptr<ofxFBXSource::NurbsCurve> _getNurbsCurveForNode( FbxNode* aNode );
    shared_ptr<ofxFBXSource::Node> _getNullForNode( FbxNode* aNode );
    shared_ptr<ofxFBXSource::Bone> _getBoneForNode( FbxNode* aNode );
    
    shared_ptr<ofxFBXSource::Node> _getOfxNodeFromNodeUserData( FbxNode* aNode );
    
    void _parentNodesRecursive( FbxNode* aNode );
    
    void cacheTexturesInScene( FbxScene* pScene );
    void deleteCachedTexturesInScene( FbxScene* pScene );
    bool isValidTexturePath( string aPathToTexture );
    
    void cacheMaterialsRecursive( FbxNode* pNode );
    void deleteCachedMaterialsRecursive( FbxNode* pNode );
    
    void populateAnimationInformation();
    void populateCachedSkeletonAnimationInformation();
    void constructSkeletonsRecursive( ofxFBXSource::Skeleton* aSkeleton, FbxNode* pNode, int aBoneLevel );
    
    void populateKeyFrames( FbxNode* pNode, int aAnimIndex );
    vector< ofxFBXSource::AnimKey<float> > getFloatKeys( FbxAnimCurve* pCurve );
    bool requiresKeyForTime( vector< ofxFBXSource::AnimKey<float> >& tkeys, signed long amillis1, signed long amillis2 );
    
    FbxTime fbxFrameTime;
    string fbxFilePath = "";
    
//    vector< shared_ptr<ofxFBXSource::Bone> > bones;
    // these are not nodes, but just data, so store them separately
    vector< shared_ptr<ofxFBXSource::Cluster> > clusters;
    vector< shared_ptr<ofxFBXSource::MeshTexture> > mTextures;
    vector< shared_ptr<ofxFBXSource::MeshMaterial> > mMaterials;
    
//    vector< shared_ptr<ofxFBXSource::Mesh> > meshes;
//    vector< shared_ptr<ofxFBXSource::Skeleton> > skeletons;
//    vector< shared_ptr<ofxFBXSource::NurbsCurve> > mNurbsCurves;
//    vector< shared_ptr<ofxFBXSource::Node> > mNullNodes;
    
    vector< shared_ptr<ofxFBXSource::Node> > mSrcNodes;
    vector< shared_ptr<ofxFBXSource::Node> > mSceneNodes;
	
    FbxManager* lSdkManager = NULL;
	FbxScene* lScene = NULL;
    
    vector<ofxFBXAnimation> animations;
    FbxAnimLayer* currentFbxAnimationLayer = NULL;
    
    Settings _settings;
    bool bMeshKeyframesCached = false;

	ofxFBXAnimation dummyAnimation;
};
    
}















