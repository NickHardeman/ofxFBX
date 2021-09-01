//
//  ofxFBXCluster.cpp
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 11/4/13.
//
//

#include "ofxFBXSrcCluster.h"
#include "ofxFBXUtils.h"

using namespace ofxFBXSource;

// from the ViewScene example //
// cache the matrices if we are not doing animation //
////------------------------------------------------
//void Cluster::setup(FbxAMatrix& pGlobalPosition, FbxMesh* pMesh, FbxCluster* pCluster, bool bLocal ) {
//
//    bLocalRef = bLocal;
////    FbxAMatrix lReferenceGlobalInitPosition;
//    FbxAMatrix lReferenceGlobalCurrentPosition;
////    FbxAMatrix lAssociateGlobalInitPosition;
//    FbxAMatrix lAssociateGlobalCurrentPosition;
//    FbxAMatrix lClusterGlobalInitPosition;
//    FbxAMatrix lClusterGlobalCurrentPosition;
//
//    FbxAMatrix lReferenceGeometry;
//    FbxAMatrix lAssociateGeometry;
//    FbxAMatrix lClusterGeometry;
//
//    FbxAMatrix lClusterRelativeInitPosition;
//    FbxAMatrix lClusterRelativeCurrentPositionInverse;
//
//    cluster = pCluster;
//    mesh    = pMesh;
//
//    lClusterGlobalCurrentPosition = GetGlobalPosition( pCluster->GetLink(), FBXSDK_TIME_INFINITE, NULL );
//    origTransform = lClusterGlobalCurrentPosition;
//
//    FbxCluster::ELinkMode lClusterMode = pCluster->GetLinkMode();
//
//    if (lClusterMode == FbxCluster::eAdditive && pCluster->GetAssociateModel()) {
//        pCluster->GetTransformAssociateModelMatrix(lAssociateGlobalInitPosition);
//        // Geometric transform of the model
//        lAssociateGeometry = GetGeometry(pCluster->GetAssociateModel());
//        lAssociateGlobalInitPosition *= lAssociateGeometry;
//        lAssociateGlobalCurrentPosition = GetGlobalPosition( pCluster->GetAssociateModel(), FBXSDK_TIME_INFINITE, NULL );
//
//        pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
//        // Multiply lReferenceGlobalInitPosition by Geometric Transformation
//        lReferenceGeometry = GetGeometry(pMesh->GetNode());
//        lReferenceGlobalInitPosition *= lReferenceGeometry;
//        lReferenceGlobalCurrentPosition = pGlobalPosition;
//
//        // Get the link initial global position and the link current global position.
//        pCluster->GetTransformLinkMatrix( lClusterGlobalInitPosition );
//        // Multiply lClusterGlobalInitPosition by Geometric Transformation
//        lClusterGeometry = GetGeometry(pCluster->GetLink());
//        lClusterGlobalInitPosition *= lClusterGeometry;
////        lClusterGlobalCurrentPosition = GetGlobalPosition( pCluster->GetLink(), FBXSDK_TIME_INFINITE, NULL );
//
//        // Compute the shift of the link relative to the reference.
//        //ModelM-1 * AssoM * AssoGX-1 * LinkGX * LinkM-1*ModelM
////        vertexTransformMatrix = lReferenceGlobalInitPosition.Inverse() * lAssociateGlobalInitPosition * lAssociateGlobalCurrentPosition.Inverse() *
////        lClusterGlobalCurrentPosition * lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;
//
//        preTrans    = lReferenceGlobalInitPosition.Inverse() * lAssociateGlobalInitPosition * lAssociateGlobalCurrentPosition.Inverse();
//        postTrans   = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;
//
//    } else {
//        pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
//        lReferenceGlobalCurrentPosition = pGlobalPosition;
//        // Multiply lReferenceGlobalInitPosition by Geometric Transformation
//        lReferenceGeometry = GetGeometry(pMesh->GetNode());
////        lReferenceGlobalInitPosition = lReferenceGeometry * lReferenceGlobalInitPosition;
//        lReferenceGlobalInitPosition *= lReferenceGeometry;
//
//        // Get the link initial global position and the link current global position.
//        pCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
////        lClusterGlobalCurrentPosition = GetGlobalPosition(pCluster->GetLink(), FBXSDK_TIME_INFINITE, NULL );
//
//        // Compute the initial position of the link relative to the reference.
////        lClusterRelativeInitPosition = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;
//
//        // Compute the current position of the link relative to the reference.
////        lClusterRelativeCurrentPositionInverse = lReferenceGlobalCurrentPosition.Inverse() * lClusterGlobalCurrentPosition;
//
//
//        preTrans    = lReferenceGlobalCurrentPosition.Inverse();
//        postTrans   = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;
//
////        preTrans    = lReferenceGlobalCurrentPosition;//.Inverse();
////        postTrans   = lReferenceGlobalInitPosition.Inverse() * lClusterGlobalInitPosition;
//
////        postTrans = lReferenceGlobalCurrentPosition.Inverse();
////        preTrans   = lReferenceGlobalInitPosition.Inverse() * lClusterGlobalInitPosition;
//
//        // Compute the shift of the link relative to the reference.
////        vertexTransformMatrix = lClusterRelativeCurrentPositionInverse * lClusterRelativeInitPosition;
//    }
//
//}

//------------------------------------------------
void Cluster::setup( FbxAMatrix& pPosition, FbxMesh* pMesh, FbxCluster* pCluster ) {
    
//    FbxAMatrix lReferenceGlobalInitPosition;
    FbxAMatrix lReferenceCurrentPosition = pPosition;
//	FbxAMatrix lAssociateGlobalInitPosition;
//	FbxAMatrix lAssociateCurrentPosition;
	FbxAMatrix lClusterInitPosition;
//	FbxAMatrix lClusterCurrentPosition;
    
	FbxAMatrix lReferenceGeometry;
//	FbxAMatrix lAssociateGeometry;
	
    
//	FbxAMatrix lClusterRelativeInitPosition;
//	FbxAMatrix lClusterRelativeCurrentPositionInverse;
    
    cluster = pCluster;
    mesh    = pMesh;
    
    FbxCluster::ELinkMode lClusterMode = pCluster->GetLinkMode();
    
    if (lClusterMode == FbxCluster::eAdditive && pCluster->GetAssociateModel()) {
        FbxAMatrix lAssociateCurrentPosition;
        FbxAMatrix lClusterGeometry;
        
        cout << pCluster->GetName() << " ofxFBXSrcCluster :: setup : additive && associateModel " << endl;
        pCluster->GetTransformAssociateModelMatrix(mAssociateInitPosition);
        // Geometric transform of the model
		FbxAMatrix lAssociateGeometry = GetGeometry(pCluster->GetAssociateModel());
		mAssociateInitPosition *= lAssociateGeometry;
        
        lAssociateCurrentPosition = GetGlobalPosition( pCluster->GetAssociateModel(), FBXSDK_TIME_INFINITE, NULL );
        
        pCluster->GetTransformMatrix(mReferenceInitPosition);
		// Multiply lReferenceGlobalInitPosition by Geometric Transformation
		lReferenceGeometry = GetGeometry(pMesh->GetNode());
		mReferenceInitPosition *= lReferenceGeometry;
//		lReferenceCurrentPosition = pGlobalPosition;
        
        // Get the link initial global position and the link current global position.
		pCluster->GetTransformLinkMatrix( lClusterInitPosition );
		// Multiply lClusterGlobalInitPosition by Geometric Transformation
		lClusterGeometry = GetGeometry(pCluster->GetLink());
		lClusterInitPosition *= lClusterGeometry;
//		lClusterGlobalCurrentPosition = GetGlobalPosition( pCluster->GetLink(), FBXSDK_TIME_INFINITE, NULL );
        
        // Compute the shift of the link relative to the reference.
		//ModelM-1 * AssoM * AssoGX-1 * LinkGX * LinkM-1*ModelM
//		vertexTransformMatrix = lReferenceGlobalInitPosition.Inverse() * lAssociateGlobalInitPosition * lAssociateGlobalCurrentPosition.Inverse() *
//        lClusterGlobalCurrentPosition * lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;
        
        mInvReferenceInitPosition = mReferenceInitPosition.Inverse();
        
        preTrans    = mReferenceInitPosition.Inverse() * mAssociateInitPosition * lAssociateCurrentPosition.Inverse();
        postTrans   = lClusterInitPosition.Inverse() * mReferenceInitPosition;
        
    } else {
//        cout << pCluster->GetName() << " ofxFBXSrcCluster :: setup : NOTTT additive && associateModel " << endl;
        
        //This lFbxLinkMatrix is the skeleton's transform when the binding happened.
        //It is the same as the matrix in bindpose if the bindpose is complete.
        // Multiply lClusterGlobalInitPosition by Geometric Transformation
//        FbxAMatrix clusterGlobalInitPosition;
//        cluster->GetTransformLinkMatrix(clusterGlobalInitPosition);

//        FbxAMatrix clusterGeometry = GetGeometry(cluster->GetLink());
//        clusterGlobalInitPosition *= clusterGeometry;

//        skeleton->at(boneListPosition).bindMatrix = clusterGlobalInitPosition;

        // Compute the shift of the link relative to the reference.
        //lVertexTransformMatrix = RGCP.Inverse() * CGCP * CGIP.Inverse() * RGIP * RG;
        // CGCP = position of bone
        // RGCP = mesh position
//        FbxAMatrix offsetMatrix;
//        FbxNode* boneNode = cluster->GetLink();
//
//        FbxAMatrix CGIP, RGIP, vertexTransformMatrix;
//
//        cluster->GetTransformLinkMatrix(CGIP);
//        cluster->GetTransformMatrix(RGIP);
//
//        vertexTransformMatrix = CGIP.Inverse() * RGIP;
        
//        preTrans = clusterGlobalInitPosition;//.Inverse();
//        postTrans = vertexTransformMatrix;
        
        
//        FbxAMatrix transformMatrix, transformLinkMatrix, globalBindposeInverseMatrix;
        
//        cluster->GetTransformMatrix(transformMatrix);   // The transformation of the mesh at binding time
//        cluster->GetTransformLinkMatrix(transformLinkMatrix);   // The transformation of the cluster(joint) at binding time from joint space to world space
//        globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * clusterGeometry;
        
        
        pCluster->GetTransformMatrix(mReferenceInitPosition);
        lReferenceGeometry = GetGeometry(pMesh->GetNode());
        mReferenceInitPosition *= lReferenceGeometry;
        
        // Get the link initial global position and the link current global position.
        pCluster->GetTransformLinkMatrix(lClusterInitPosition);
        
//        FbxAMatrix currentTransformOffset = pCluster->GetLink()->EvaluateGlobalTransform(FBXSDK_TIME_INFINITE);
//        currentTransformOffset *= clusterGeometry;
//        lReferenceGlobalCurrentPosition = GetGlobalPosition( mesh->GetNode(), FBXSDK_TIME_INFINITE, NULL );
//        preTrans    = mReferenceInitPosition;//.Inverse();
//        postTrans    = vertexTransformMatrix;// * clusterGeometry.Inverse();
//        preTrans    = currentTransformOffset.Inverse();
//        postTrans   = lClusterInitPosition.Inverse() * mReferenceInitPosition;
        
        preTrans    = lReferenceCurrentPosition.Inverse();
        postTrans   = lClusterInitPosition.Inverse() * mReferenceInitPosition;
        
        mInvReferenceInitPosition = mReferenceInitPosition.Inverse();
        
//        skeleton->at(boneListPosition).localBoneMatrix = vertexTransformMatrix;
        
//        pCluster->GetTransformMatrix(mReferenceInitPosition);
////        FbxAMatrix lMeshGlobal;
//        pCluster->GetTransformMatrix(mReferenceInitPosition);
////		lReferenceCurrentPosition = pGlobalPosition;
//		// Multiply lReferenceGlobalInitPosition by Geometric Transformation
//		lReferenceGeometry = GetGeometry(pMesh->GetNode());
////        lReferenceGlobalInitPosition = lReferenceGeometry * lReferenceGlobalInitPosition;
////        if( pMesh->GetNode()->GetParent() ) {
////            FbxAMatrix lParentGlobalPosition;// = pCluster->GetTransformParentMatrix();
////            //            pCluster->GetTransformParentMatrix(lParentGlobalPosition);
////            lParentGlobalPosition = GetGlobalPosition( pMesh->GetNode()->GetParent(), FBXSDK_TIME_INFINITE, NULL );
////            mReferenceInitPosition *= ( lParentGlobalPosition * lReferenceGeometry );
////        } else {
////            mReferenceInitPosition *= lReferenceGeometry;
////        }
//
//		// Get the link initial global position and the link current global position.
//		pCluster->GetTransformLinkMatrix(lClusterInitPosition);
////		lClusterGlobalCurrentPosition = GetGlobalPosition(pCluster->GetLink(), FBXSDK_TIME_INFINITE, NULL );
//
//		// Compute the initial position of the link relative to the reference.
////		lClusterRelativeInitPosition = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;
//
//		// Compute the current position of the link relative to the reference.
////		lClusterRelativeCurrentPositionInverse = lReferenceGlobalCurrentPosition.Inverse() * lClusterGlobalCurrentPosition;
//
//        preTrans    = lReferenceCurrentPosition.Inverse();
//        postTrans   = lClusterInitPosition.Inverse() * mReferenceInitPosition;
////        preTrans = lClusterInitPosition.Inverse();
////        postTrans =
////        preTrans    = lMeshGlobal;
////        postTrans   = lClusterInitPosition.Inverse();
//
////        postTrans   = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;
//
////        FbxAMatrix lReferenceGlobalCurrentPosition = GetGlobalPosition( pMesh->GetNode(), FBXSDK_TIME_INFINITE, NULL );
////        preTrans    = lReferenceGlobalCurrentPosition.Inverse();
//
        
//        preTrans    = lReferenceGlobalCurrentPosition;//.Inverse();
//        postTrans   = lReferenceGlobalInitPosition.Inverse() * lClusterGlobalInitPosition;
        
//        postTrans = lReferenceGlobalCurrentPosition.Inverse();
//        preTrans   = lReferenceGlobalInitPosition.Inverse() * lClusterGlobalInitPosition;
        
		// Compute the shift of the link relative to the reference.
//		vertexTransformMatrix = lClusterRelativeCurrentPositionInverse * lClusterRelativeInitPosition;
    }
    
//    if(pCluster->GetLink() && pCluster->GetLink()->GetParent() ) {
//        auto lParentGlobalPosition = GetGlobalPosition( pCluster->GetLink()->GetParent(), FBXSDK_TIME_INFINITE, NULL );
//    }
    
}

//------------------------------------------------
void Cluster::update( FbxTime& pTime , FbxPose* pPose ) {
    FbxCluster::ELinkMode lClusterMode = cluster->GetLinkMode();
    if (lClusterMode == FbxCluster::eAdditive && cluster->GetAssociateModel()) {
        cout << "ofxFBXCluster :: update : cluster mode is eAdditive " << endl;
        FbxAMatrix lAssociateGlobalCurrentPosition = GetGlobalPosition( cluster->GetAssociateModel(), pTime, NULL );
//        mInvReferenceInitPosition
//        preTrans    = mReferenceInitPosition.Inverse() * mAssociateInitPosition * lAssociateGlobalCurrentPosition.Inverse();
        preTrans    = mInvReferenceInitPosition * mAssociateInitPosition * lAssociateGlobalCurrentPosition.Inverse();
    } else {
        cout << "ofxFBXCluster :: update : cluster mode is not eAdditive " << endl;
        FbxAMatrix lReferenceGlobalCurrentPosition = GetGlobalPosition( mesh->GetNode(), pTime, NULL );
        preTrans    = lReferenceGlobalCurrentPosition.Inverse();
    }
}






















