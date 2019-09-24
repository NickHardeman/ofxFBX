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

namespace ofxFBXSource {

class Cluster {
public:
    Cluster(){}
    ~Cluster() {}
    void setup( FbxAMatrix& pGlobalPosition,
               FbxMesh* pMesh,
               FbxCluster* pCluster );
    void update( FbxTime& pTime, FbxPose* pPose );
    
    FbxAMatrix origTransform;
    FbxAMatrix preTrans, postTrans;
    FbxCluster* cluster = NULL;
    FbxMesh* mesh = NULL;
protected:
    FbxAMatrix lReferenceGlobalInitPosition;
    FbxAMatrix lAssociateGlobalInitPosition;
};
    
}
