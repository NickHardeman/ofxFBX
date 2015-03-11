/****************************************************************************************

Copyright (C) 2013 Autodesk, Inc.
All rights reserved.

Use of this software is subject to the terms of the Autodesk license agreement
provided at the time of installation or download, or which otherwise accompanies
this software in either electronic or hard copy form.

****************************************************************************************/
 
#ifndef _GET_POSITION_H
#define _GET_POSITION_H

#include "fbxsdk.h"
#include "ofMain.h"

FbxAMatrix GetGlobalPosition(FbxNode* pNode,
							  const FbxTime& pTime, 
							  FbxPose* pPose = NULL,
							  FbxAMatrix* pParentGlobalPosition = NULL);
FbxAMatrix GetLocalPositionForNode(FbxNode* pNode,
                                   const FbxTime& pTime,
                                   FbxPose* pPose = NULL,
                                   FbxAMatrix* pParentGlobalPosition = NULL);

FbxAMatrix GetPoseMatrix(FbxPose* pPose, 
                          int pNodeIndex);
FbxAMatrix GetGeometry(FbxNode* pNode);

// Scale all the elements of a matrix.
void MatrixScale(FbxAMatrix& pMatrix, double pValue);

// Add a value to all the elements in the diagonal of the matrix.
void MatrixAddToDiagonal(FbxAMatrix& pMatrix, double pValue);

// Sum two matrices element by element.
void MatrixAdd(FbxAMatrix& pDstMatrix, FbxAMatrix& pSrcMatrix);



#endif // #ifndef _GET_POSITION_H



