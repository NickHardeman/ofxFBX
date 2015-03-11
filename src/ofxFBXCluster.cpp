//
//  ofxFBXCluster.cpp
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 11/4/13.
//
//

#include "ofxFBXCluster.h"

// from the ViewScene example //
// cache the matrices if we are not doing animation //
void ofxFBXCluster::setup(FbxAMatrix& pGlobalPosition,
                          FbxMesh* pMesh,
                          FbxCluster* pCluster ) {
//    FbxAMatrix lReferenceGlobalInitPosition;
	FbxAMatrix lReferenceGlobalCurrentPosition;
//	FbxAMatrix lAssociateGlobalInitPosition;
	FbxAMatrix lAssociateGlobalCurrentPosition;
	FbxAMatrix lClusterGlobalInitPosition;
	FbxAMatrix lClusterGlobalCurrentPosition;
    
	FbxAMatrix lReferenceGeometry;
	FbxAMatrix lAssociateGeometry;
	FbxAMatrix lClusterGeometry;
    
	FbxAMatrix lClusterRelativeInitPosition;
	FbxAMatrix lClusterRelativeCurrentPositionInverse;
    
    cluster = pCluster;
    mesh    = pMesh;
    
    lClusterGlobalCurrentPosition = GetGlobalPosition( pCluster->GetLink(), FBXSDK_TIME_INFINITE, NULL );
    origTransform = lClusterGlobalCurrentPosition;
    
    FbxCluster::ELinkMode lClusterMode = pCluster->GetLinkMode();
    if (lClusterMode == FbxCluster::eAdditive && pCluster->GetAssociateModel()) {
        pCluster->GetTransformAssociateModelMatrix(lAssociateGlobalInitPosition);
        // Geometric transform of the model
		lAssociateGeometry = GetGeometry(pCluster->GetAssociateModel());
		lAssociateGlobalInitPosition *= lAssociateGeometry;
		lAssociateGlobalCurrentPosition = GetGlobalPosition( pCluster->GetAssociateModel(), FBXSDK_TIME_INFINITE, NULL );
        
        pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
		// Multiply lReferenceGlobalInitPosition by Geometric Transformation
		lReferenceGeometry = GetGeometry(pMesh->GetNode());
		lReferenceGlobalInitPosition *= lReferenceGeometry;
		lReferenceGlobalCurrentPosition = pGlobalPosition;
        
        // Get the link initial global position and the link current global position.
		pCluster->GetTransformLinkMatrix( lClusterGlobalInitPosition );
		// Multiply lClusterGlobalInitPosition by Geometric Transformation
		lClusterGeometry = GetGeometry(pCluster->GetLink());
		lClusterGlobalInitPosition *= lClusterGeometry;
//		lClusterGlobalCurrentPosition = GetGlobalPosition( pCluster->GetLink(), FBXSDK_TIME_INFINITE, NULL );
        
        // Compute the shift of the link relative to the reference.
		//ModelM-1 * AssoM * AssoGX-1 * LinkGX * LinkM-1*ModelM
//		vertexTransformMatrix = lReferenceGlobalInitPosition.Inverse() * lAssociateGlobalInitPosition * lAssociateGlobalCurrentPosition.Inverse() *
//        lClusterGlobalCurrentPosition * lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;
        
        preTrans    = lReferenceGlobalInitPosition.Inverse() * lAssociateGlobalInitPosition * lAssociateGlobalCurrentPosition.Inverse();
        postTrans   = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;
        
    } else {
        pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
		lReferenceGlobalCurrentPosition = pGlobalPosition;
		// Multiply lReferenceGlobalInitPosition by Geometric Transformation
		lReferenceGeometry = GetGeometry(pMesh->GetNode());
		lReferenceGlobalInitPosition *= lReferenceGeometry;
        
		// Get the link initial global position and the link current global position.
		pCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
//		lClusterGlobalCurrentPosition = GetGlobalPosition(pCluster->GetLink(), FBXSDK_TIME_INFINITE, NULL );
        
		// Compute the initial position of the link relative to the reference.
//		lClusterRelativeInitPosition = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;
        
		// Compute the current position of the link relative to the reference.
//		lClusterRelativeCurrentPositionInverse = lReferenceGlobalCurrentPosition.Inverse() * lClusterGlobalCurrentPosition;
        
        
        preTrans    = lReferenceGlobalCurrentPosition.Inverse();
        postTrans   = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;
        
		// Compute the shift of the link relative to the reference.
//		vertexTransformMatrix = lClusterRelativeCurrentPositionInverse * lClusterRelativeInitPosition;
    }
    
}


void ofxFBXCluster::update( FbxTime& pTime , FbxPose* pPose ) {
    FbxCluster::ELinkMode lClusterMode = cluster->GetLinkMode();
    if (lClusterMode == FbxCluster::eAdditive && cluster->GetAssociateModel()) {
        cout << "ofxFBXCluster :: update : cluster mode is eAdditive " << endl;
        FbxAMatrix lAssociateGlobalCurrentPosition = GetGlobalPosition( cluster->GetAssociateModel(), pTime, NULL );
        preTrans    = lReferenceGlobalInitPosition.Inverse() * lAssociateGlobalInitPosition * lAssociateGlobalCurrentPosition.Inverse();
    } else {
        cout << "ofxFBXCluster :: update : cluster mode is not eAdditive " << endl;
        FbxAMatrix lReferenceGlobalCurrentPosition = GetGlobalPosition( mesh->GetNode(), pTime, NULL );
        preTrans    = lReferenceGlobalCurrentPosition.Inverse();
    }
}






















