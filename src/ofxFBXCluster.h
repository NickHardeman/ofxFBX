//
//  ofxFBXCluster.h
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 11/4/13.
//
//

#pragma once
#include "ofMain.h"

#include <fbxsdk.h>

/*
#if defined(TARGET_LINUX) 
    #include <fbxsdk.h>
#else
	#include "fbxsdk.h"
#endif
*/


#include "ofxFBXNode.h"

class ofxFBXCluster {
public:
    ofxFBXCluster(){}
    ~ofxFBXCluster() {}
    void setup( FbxAMatrix& pGlobalPosition,
               FbxMesh* pMesh,
               FbxCluster* pCluster );
    void update( FbxTime& pTime, FbxPose* pPose );
    
    FbxAMatrix origTransform;
    FbxAMatrix preTrans, postTrans;
    FbxCluster* cluster;
    FbxMesh* mesh;
protected:
    FbxAMatrix lReferenceGlobalInitPosition;
    FbxAMatrix lAssociateGlobalInitPosition;
};
