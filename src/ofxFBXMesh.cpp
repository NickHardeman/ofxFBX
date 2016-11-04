//
//  ofxFBXMesh.h
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 10/31/13.
//
//

#include "ofxFBXMesh.h"
#include "ofxFBX.h"

//--------------------------------------------------------------
ofxFBXMesh::ofxFBXMesh() {
    fbxMesh = NULL;
    bAllMappedByControlPoint    = true;
    mNormalsArray = NULL;
}

//--------------------------------------------------------------
ofxFBXMesh::~ofxFBXMesh() {
    if( mNormalsArray != NULL ) {
        delete [] mNormalsArray;
        mNormalsArray = NULL;
    }
}

//--------------------------------------------------------------
void ofxFBXMesh::setup( FbxNode *pNode ) {
    ofxFBXNode::setup( pNode );
    // Bake material and hook as user data.
//    const int lMaterialCount = pNode->GetMaterialCount();
//    subMeshes.resize( lMaterialCount );
//    for (int lMaterialIndex = 0; lMaterialIndex < lMaterialCount; ++lMaterialIndex) {
//        FbxSurfaceMaterial * pMaterial = pNode->GetMaterial(lMaterialIndex);
//        
//        GetElementMaterial
//        
//        ofMaterial& mat = meshMaterials[lMaterialIndex].material;
//        meshMaterials[lMaterialIndex].setup( pMaterial );
//    }
//    
//    cout << "ofxFBXMesh :: setup : number of materials = " << lMaterialCount << " for " << getName() << endl;
    
    setFBXMesh( pNode->GetMesh() );
}

//--------------------------------------------------------------
void ofxFBXMesh::setFBXMesh( FbxMesh* lMesh ) {
	fbxMesh = lMesh;
//	name = lMesh->GetName();
	mesh.clear();
    
    // from ViewScene Example included with the FBX SDK //
    if (!lMesh->GetNode()) {
        ofLogError("ofxFBXMesh") << " error setFBXMesh, lMesh->GetNode failed" << endl;
        return;
    }
    
    
    const int lPolygonCount = lMesh->GetPolygonCount();
    
    
    // Count the polygon count of each material
    FbxLayerElementArrayTemplate<int>* lMaterialIndice = NULL;
    FbxGeometryElement::EMappingMode lMaterialMappingMode = FbxGeometryElement::eNone;
    if (lMesh->GetElementMaterial()) {
        lMaterialIndice = &lMesh->GetElementMaterial()->GetIndexArray();
        lMaterialMappingMode = lMesh->GetElementMaterial()->GetMappingMode();
        if (lMaterialIndice && lMaterialMappingMode == FbxGeometryElement::eByPolygon) {
            FBX_ASSERT(lMaterialIndice->GetCount() == lPolygonCount);
            if (lMaterialIndice->GetCount() == lPolygonCount) {
                
                // make sure the vector is setup and we have the proper amount of materials ready //
                for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex) {
                    const int lMaterialIndex = lMaterialIndice->GetAt(lPolygonIndex);
                    if(lMaterialIndex >= 0) {
                        if (subMeshes.size() < lMaterialIndex + 1) {
                            subMeshes.resize(lMaterialIndex + 1);
                        }
                    }
                }
                
                // Count the faces of each material
                for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex) {
                    const int lMaterialIndex = lMaterialIndice->GetAt(lPolygonIndex);
//                    cout << "lMaterialIndex = " << lMaterialIndex << " submesh.size = " << subMeshes.size() << endl;
                    if(lMaterialIndex >= 0 ) {
                        subMeshes[lMaterialIndex].triangleCount += 1;
                    }
                }
                
                // Record the offset (how many vertex)
                const int lMaterialCount = subMeshes.size();
                int lOffset = 0;
                for (int lIndex = 0; lIndex < lMaterialCount; ++lIndex) {
                    subMeshes[lIndex].indexOffset = lOffset;
                    lOffset += subMeshes[lIndex].triangleCount * 3;
//                    subMeshes[lIndex].totalIndices = subMeshes[lIndex].triangleCount * 3;
//                    cout << "polygon: " << lIndex << " totalIndices = " << subMeshes[lIndex].totalIndices << endl;
                    // This will be used as counter in the following procedures, reset to zero
                    subMeshes[lIndex].triangleCount = 0;
                }
                FBX_ASSERT(lOffset == lPolygonCount * 3); // what?
            }
        }
    }
    
    if(subMeshes.size() == 0) {
        subMeshes.resize(1);
    }
    
    // TODO: Account for if all mapping modes are not by control point //
    
    bool bHasNormals = lMesh->GetElementNormalCount() > 0;
    bool bMapNormalsByControlPoint = true;
    bool bHasUvs = lMesh->GetElementUVCount() > 0;
    bool bMapUvsByControlPoint = true;
    
    // are the normals mapped by control point or by polygon?
    FbxGeometryElement::EMappingMode lNormalMappingMode = FbxGeometryElement::eNone;
    if(bHasNormals) {
        lNormalMappingMode = lMesh->GetElementNormal(0)->GetMappingMode();
        if (lNormalMappingMode == FbxGeometryElement::eNone) {
            bHasNormals = false;
        }
        if (bHasNormals && lNormalMappingMode != FbxGeometryElement::eByControlPoint) {
            bMapNormalsByControlPoint = false;
        }
    }
    
    FbxGeometryElement::EMappingMode lUVMappingMode = FbxGeometryElement::eNone;
    if (bHasUvs) {
        lUVMappingMode = lMesh->GetElementUV(0)->GetMappingMode();
        if (lUVMappingMode == FbxGeometryElement::eNone) {
            bHasUvs = false;
        }
        if (bHasUvs && lUVMappingMode != FbxGeometryElement::eByControlPoint) {
            bMapUvsByControlPoint = false;
        }
    }
    
    bAllMappedByControlPoint = (bMapNormalsByControlPoint && bMapUvsByControlPoint);
    
//    if(lMaterialMappingMode == FbxGeometryElement::eAllSame) {
//        
//    }
    
    int lPolygonVertexCount = lMesh->GetControlPointsCount();
    
    if( bAllMappedByControlPoint ) {
        const FbxVector4 * lControlPoints = lMesh->GetControlPoints();
        for(int i = 0; i < lMesh->GetControlPointsCount(); i++ ) {
            mesh.addVertex( ofVec3f( lControlPoints[i][0], lControlPoints[i][1], lControlPoints[i][2] ) );
        }
        
        for(int i = 0; i < lMesh->GetPolygonCount(); i++ ) {
            for (int lVerticeIndex = 0; lVerticeIndex < 3; ++lVerticeIndex) {
                const int lControlPointIndex = lMesh->GetPolygonVertex(i, lVerticeIndex);
                mesh.addIndex( lControlPointIndex );
            }
        }
        
        if(subMeshes.size() == 1) {
            subMeshes[0].totalIndices = mesh.getNumIndices();
        }
        
        // normals
        if(bHasNormals) {
            ofMesh normalsMesh;
            cout << "ofxFBXMesh :: we have normals for " << getName() << endl;
            if(bMapNormalsByControlPoint) {
                cout << "ofxFBXMesh :: normals by control point for " << getName() << endl;
                const FbxGeometryElementNormal * lNormalElement = lMesh->GetElementNormal(0);
                for(int i = 0; i < lMesh->GetControlPointsCount(); i++ ) {
                    int lNormalIndex = i;
                    if (lNormalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect) {
                        lNormalIndex = lNormalElement->GetIndexArray().GetAt(i);
                    }
                    FbxVector4 lCurrentNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
                    normalsMesh.addNormal( ofVec3f(lCurrentNormal[0], lCurrentNormal[1], lCurrentNormal[2]) );
                }
                mesh.addNormals( normalsMesh.getNormals() );
            }
        }
        
        // textures //
        if(bHasUvs) {
            cout << "ofxFBXMesh :: we have tex coords for " << getName() << endl;
            if(bMapUvsByControlPoint) {
                cout << "ofxFBXMesh :: tex coords by control point " << getName() << endl;
                const FbxGeometryElementUV * lUVElement = lMesh->GetElementUV(0);
                for(int i = 0; i < lMesh->GetControlPointsCount(); i++ ) {
                    int lUVIndex = i;
                    if (lUVElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect) {
                        lUVIndex = lUVElement->GetIndexArray().GetAt(i);
                    }
                    FbxVector2 lCurrentUV = lUVElement->GetDirectArray().GetAt(lUVIndex);
                    mesh.addTexCoord( ofVec2f(lCurrentUV[0], lCurrentUV[1]) );
                }
            }
        }
    } else {
        
//        cout << "--- " << getName() << " mapped by polygons ---------------" << endl;
//        cout << "numSubMeshes = " << subMeshes.size() << " polygon count = " << lPolygonCount << " control points = " << fbxMesh->GetControlPointsCount() << endl;
        
//        if(lMaterialMappingMode == FbxGeometryElement::eNone) {
//            cout << "Material mapping mode = none" << endl;
//        } else if (lMaterialMappingMode == FbxGeometryElement::eByControlPoint) {
//            cout << "Material mapping mode = eByControlPoint" << endl;
//        } else if(lMaterialMappingMode == FbxGeometryElement::eByPolygonVertex) {
//            cout << "Material mapping mode = eByPolygonVertex" << endl;
//        } else if(lMaterialMappingMode == FbxGeometryElement::eByPolygon) {
//            cout << "Material mapping mode = eByPolygon" << endl;
//        } else if(lMaterialMappingMode == FbxGeometryElement::eByEdge) {
//            cout << "Material mapping mode = eByEdge" << endl;
//        } else if(lMaterialMappingMode == FbxGeometryElement::eAllSame) {
//            cout << "Material mapping mode = eAllSame" << endl;
//        }
//        
//        if(lMaterialIndice) {
//            cout << "lMaterialIndice size = " << lMaterialIndice->GetCount() << endl;
//        } else {
//            cout << "did not get the lMaterialIndice" << endl;
//        }
        
        const FbxVector4 * lControlPoints = fbxMesh->GetControlPoints();
        mesh.getVertices().resize( lPolygonCount * 3 );
        mesh.getIndices().resize( lPolygonCount * 3 );
        if(bHasNormals) {
            mesh.getNormals().resize( lPolygonCount * 3 );
        }
        
//        cout << "Polygon vertex count = " << fbxMesh->GetPolygonVertexCount() << endl;
        
        const char * lUVName = NULL;
        FbxStringList lUVNames;
        fbxMesh->GetUVSetNames(lUVNames);
        if(bHasUvs && lUVNames.GetCount() ) {
            mesh.getTexCoords().resize( lPolygonCount * 3 );
            lUVName = lUVNames[0];
        }
        for(int i = 0; i < lUVNames.GetCount(); i++ ) {
//            cout << "lUVName = " << lUVNames[0] << endl;
        }
        
        
        
        FbxVector4 lCurrentVertex;
        FbxVector4 lCurrentNormal;
        FbxVector2 lCurrentUV;
        
        const FbxGeometryElementUV * lUVElement = lMesh->GetElementUV(0);
        
        int lVertexCount = 0;
        for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex) {
            // The material for current face.
            int lMaterialIndex = 0;
            if (lMaterialIndice && lMaterialMappingMode == FbxGeometryElement::eByPolygon) {
                lMaterialIndex = lMaterialIndice->GetAt(lPolygonIndex);
                if(lMaterialIndex < 0) {
                    lMaterialIndex = 0;
                }
            }
            
            // Where should I save the vertex attribute index, according to the material
            const int lIndexOffset = subMeshes[lMaterialIndex].indexOffset + subMeshes[lMaterialIndex].triangleCount * 3;
//            const int lIndexOffset = fbxMesh->GetPolygonVertexIndex(lPolygonIndex);
            for (int lVerticeIndex = 0; lVerticeIndex < 3; ++lVerticeIndex) {
                const int lControlPointIndex = fbxMesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex);
                
                // Populate the array with vertex attribute, if by polygon vertex.
                mesh.getIndices()[lIndexOffset + lVerticeIndex] = static_cast<unsigned int>(lVertexCount);
                
                lCurrentVertex = lControlPoints[ lControlPointIndex ];
                mesh.getVertices()[lVertexCount].set( lCurrentVertex[0], lCurrentVertex[1], lCurrentVertex[2] );
                
                if (bHasNormals) {
                    if(fbxMesh->GetPolygonVertexNormal( lPolygonIndex, lVerticeIndex, lCurrentNormal )) {
                        int normalIndex = lVertexCount;
                        mesh.getNormals()[normalIndex].set( lCurrentNormal[0], lCurrentNormal[1], lCurrentNormal[2] );
                    }
                }
                
                if (bHasUvs) {
                    bool lUnmappedUV;
                    fbxMesh->GetPolygonVertexUV(lPolygonIndex, lVerticeIndex, lUVName, lCurrentUV, lUnmappedUV);
                    int tcoordIndex = lVertexCount;
                    mesh.getTexCoords()[ tcoordIndex ].set( lCurrentUV[0], lCurrentUV[1] );
                }
                ++lVertexCount;
            }
            
            subMeshes[lMaterialIndex].triangleCount += 1;
            subMeshes[lMaterialIndex].totalIndices += 3;
            
//            cout << "materialIndex = " << lMaterialIndex << endl;
        }
        
    }
    
    // All faces will use the same material.
//    if ( subMeshes.size() == 1) {
//        //        meshMaterials.resize(1);
////        subMeshes[0].totalIndices = mesh.getNumIndices();
//    }
//    for(int i = 0; i < subMeshes.size(); i++ ) {
//        cout << i << " submesh totalindicies = " << subMeshes[i].totalIndices << " total verts = " << mesh.getNumVertices() << " polygonos = " << lPolygonCount << endl;
//    }
    
//    cout << "--------------------------------------- " << endl << endl;
    veebs.setMesh( mesh, GL_STREAM_DRAW );
    original = mesh;
    
    if( bHasNormals ) {
        if(fbxMesh->GetControlPointsCount()) {
            mNormalsArray = new FbxVector4[ fbxMesh->GetControlPointsCount() ];
            populateNormals( mNormalsArray );
        }
    }
    
    // associate the materials with the sub meshes //
    int lSubMeshCount = subMeshes.size();
    for (int lIndex = 0; lIndex < lSubMeshCount; ++lIndex) {
        const FbxSurfaceMaterial * lMaterial = fbxMesh->GetNode()->GetMaterial(lIndex);
        if(lMaterial) {
            subMeshes[ lIndex ].materialPtr = static_cast<ofxFBXMeshMaterial *>(lMaterial->GetUserDataPtr());
        }
    }
    
}

//--------------------------------------------------------------
void ofxFBXMesh::configureMesh( ofMesh& aMesh ) {
    aMesh = mesh;
}

//--------------------------------------------------------------
void ofxFBXMesh::updateMesh( ofMesh* aMesh, FbxTime& pTime, FbxAnimLayer * pAnimLayer, FbxPose* pPose ) {
    const bool lHasShape    = fbxMesh->GetShapeCount() > 0;
    const bool lHasSkin     = fbxMesh->GetDeformerCount(FbxDeformer::eSkin) > 0;
    const bool lHasDeformation = lHasShape || lHasSkin;
    
    const int lVertexCount = fbxMesh->GetControlPointsCount();
    if(!lHasDeformation || lVertexCount < 3) return;
    
    FbxAMatrix lGlobalPosition = GetGlobalPosition( fbxMesh->GetNode(), pTime, pPose );
    // Geometry offset.
    // it is not inherited by the children.
    FbxAMatrix lGeometryOffset = GetGeometry( fbxMesh->GetNode() );
    FbxAMatrix lGlobalOffPosition = lGlobalPosition * lGeometryOffset;
    
    FbxVector4* lVertexArray = NULL;
    lVertexArray = new FbxVector4[ lVertexCount ];
    memcpy( lVertexArray, fbxMesh->GetControlPoints(), lVertexCount * sizeof(FbxVector4) );
    
    FbxVector4* lNormalArray = NULL;
    if( aMesh->hasNormals() && aMesh->usingNormals() ) {
        if( mNormalsArray != NULL ) {
//            cout << "updateMesh :: aMesh " << aMesh->hasNormals() << " using: " << aMesh->usingNormals() << endl;
            lNormalArray = new FbxVector4[ lVertexCount ];
            memcpy( lNormalArray, mNormalsArray, lVertexCount * sizeof(FbxVector4) );
        }
    }
    
    if(lHasShape) {
        computeBlendShapes( aMesh, pTime, pAnimLayer );
    }
    if(lHasSkin) {
        //we need to get the number of clusters. which are controlled by bones //
        const int lSkinCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
        int lClusterCount = 0;
        for (int lSkinIndex = 0; lSkinIndex < lSkinCount; ++lSkinIndex) {
            lClusterCount += ((FbxSkin *)(fbxMesh->GetDeformer(lSkinIndex, FbxDeformer::eSkin)))->GetClusterCount();
        }
        if (lClusterCount) {
            computeSkinDeformation( lGlobalOffPosition, pTime, pAnimLayer, lVertexArray, lNormalArray, pPose );
        }
    }
//    cout << "Calling update mesh lHasShape = " << lHasShape << " skin = " << lHasSkin << " lHasDeformation = " << lHasDeformation << endl;
    
    
    vector< ofVec3f >& amverts      = aMesh->getVertices();
    vector< ofVec3f >& amnormals    = aMesh->getNormals();
    
    bool bUpdateNormals = lNormalArray != NULL;
    if(bAllMappedByControlPoint) {
//        cout << "updateMesh :: bAllMappedByControlPoint : " << endl;
        for(int i = 0; i < fbxMesh->GetControlPointsCount(); i++ ) {
            amverts[i].set( lVertexArray[i][0], lVertexArray[i][1], lVertexArray[i][2] );
            if( bUpdateNormals ) {
                amnormals[i].set( lNormalArray[i][0], lNormalArray[i][1], lNormalArray[i][2] );
                amnormals[i].normalize();
            }
        }
    } else {
//        cout << "updateMesh :: !bAllMappedByControlPoint : normals: " << bUpdateNormals << endl;
        const int lPolygonCount = fbxMesh->GetPolygonCount();
        int tvertcount = 0;
        for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex) {
            for (int lVerticeIndex = 0; lVerticeIndex < 3; ++lVerticeIndex) {
                const int lControlPointIndex = fbxMesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex);
                amverts[tvertcount].set(lVertexArray[lControlPointIndex][0],
                                        lVertexArray[lControlPointIndex][1],
                                        lVertexArray[lControlPointIndex][2]);
                if( bUpdateNormals ) {
                    amnormals[tvertcount].set(lNormalArray[ lControlPointIndex ][0],
                                              lNormalArray[ lControlPointIndex ][1],
                                              lNormalArray[ lControlPointIndex ][2] );
                    amnormals[tvertcount].normalize();
                }
                ++tvertcount;
            }
        }
    }
    
    delete [] lVertexArray;
    if( lNormalArray != NULL ) {
        delete [] lNormalArray;
    }
    
}


//--------------------------------------------------------------
void ofxFBXMesh::draw( ofMesh* aMesh ) {
    
    veebs.updateMesh( *aMesh );
    
    const int lSubMeshCount = subMeshes.size();
    glEnable( GL_NORMALIZE );
    
    if(veebs.getIsAllocated()) {
        if(lSubMeshCount > 0) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );
            veebs.bind();
        }
        for (int lIndex = 0; lIndex < lSubMeshCount; ++lIndex) {
            const FbxSurfaceMaterial * lMaterial = fbxMesh->GetNode()->GetMaterial(lIndex);
            ofxFBXMeshMaterial* lMaterialCache = NULL;
            if(lMaterial) {
                 lMaterialCache = static_cast<ofxFBXMeshMaterial *>(lMaterial->GetUserDataPtr());
            }
            bool bWasUsingTextures = false;
            if (lMaterialCache ) {
//                cout << getName() << " has a material cache : with texture = " << lMaterialCache->hasTexture() << endl;
                if( ofGetFrameNum() % 60 == 0 ) {
//                    cout << "******* submesh: " << lIndex << endl;
//                    cout << lMaterialCache->getInfoAsString();
//                    cout << "**************************** " << endl;
                }
                bWasUsingTextures = lMaterialCache->areTexturesEnabled();
//                lMaterialCache->enableTextures();
                if( !aMesh->usingTextures() || !aMesh->hasTexCoords() ) lMaterialCache->disableTextures();
                lMaterialCache->begin();
            }
            
            GLsizei lOffset = subMeshes[lIndex].indexOffset * sizeof(unsigned int);
//            const GLsizei lElementCount = meshMaterials[lIndex].triangleCount * 3;
            const GLsizei lElementCount = subMeshes[lIndex].totalIndices;
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, veebs.getIndexId() );
//            cout << "lElementCount = " << lElementCount << " mesh is using indices = " << mesh.hasIndices() << endl;
            glDrawElements( GL_TRIANGLES, lElementCount, GL_UNSIGNED_INT, reinterpret_cast<const GLvoid *>(lOffset));
            if (lMaterialCache ) {
                lMaterialCache->end();
                if(bWasUsingTextures) lMaterialCache->enableTextures();
                else lMaterialCache->disableTextures();
            }
        }
        if(lSubMeshCount > 0) veebs.unbind();
    }
}

//--------------------------------------------------------------
void ofxFBXMesh::drawNormals( ofMesh* aMesh, float length, bool bFaceNormals ) {
//    transformGL();
    
    if( aMesh->usingNormals()) {
        vector<ofVec3f>& normals    = aMesh->getNormals();
        vector<ofVec3f>& vertices   = aMesh->getVertices();
        ofVec3f normal;
        ofVec3f vert;
        
        // super inefficient, for debug only //
        ofMesh normalsMesh;
        normalsMesh.setMode( OF_PRIMITIVE_LINES );
        normalsMesh.getVertices().resize( normals.size() * 2);
        
        if(bFaceNormals) {
            for(int i = 0; i < (int)normals.size(); i++ ) {
                if(i % 3 == 0) {
                    vert = (vertices[i]+vertices[i+1]+vertices[i+2]) / 3;
                } else if(i % 3 == 1) {
                    vert = (vertices[i-1]+vertices[i]+vertices[i+1]) / 3;
                } else if ( i % 3 == 2) {
                    vert = (vertices[i-2]+vertices[i-1]+vertices[i]) / 3;
                }
                normalsMesh.setVertex(i*2, vert);
                normal = normals[i].getNormalized();
                normal *= length;
                normalsMesh.setVertex(i*2+1, normal+vert);
            }
        } else {
            for(int i = 0; i < (int)normals.size(); i++) {
                vert = vertices[i];
                normal = normals[i].getNormalized();
                normalsMesh.setVertex( i*2, vert);
                normal *= length;
                normalsMesh.setVertex(i*2+1, normal+vert);
            }
        }
        normalsMesh.draw();
    } else {
        ofLogWarning("ofxFBXMesh") << "drawNormals(): mesh normals are disabled for " << getName();
    }
    
    
//    restoreTransformGL();
}

//--------------------------------------------------------------
int ofxFBXMesh::getNumSubMeshes() {
    return (int)subMeshes.size();
}

//--------------------------------------------------------------
int ofxFBXMesh::getNumMaterials() {
    return getNumSubMeshes();
}

//--------------------------------------------------------------
bool ofxFBXMesh::hasClusterDeformation() {
    if(fbxMesh == NULL) {
        ofLogWarning("ofxFBSMesh :: Set the Node before calling hasClusterDeformation");
        return false;
    }
    return fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
}

//--------------------------------------------------------------
vector< ofxFBXMeshMaterial* > ofxFBXMesh::getMaterials() {
    vector< ofxFBXMeshMaterial* > rMaterials;
    int lSubMeshCount = subMeshes.size();
    for (int lIndex = 0; lIndex < lSubMeshCount; ++lIndex) {
        const FbxSurfaceMaterial * lMaterial = fbxMesh->GetNode()->GetMaterial(lIndex);
        ofxFBXMeshMaterial* lMaterialCache = NULL;
        if(lMaterial) {
            lMaterialCache = static_cast<ofxFBXMeshMaterial *>(lMaterial->GetUserDataPtr());
            rMaterials.push_back( lMaterialCache );
        }
        
    }
    return rMaterials;
}

//--------------------------------------------------------------
ofVbo& ofxFBXMesh::getVbo() {
    return veebs;
}

//--------------------------------------------------------------
ofMesh& ofxFBXMesh::getOFMesh() {
    return mesh;
}

//--------------------------------------------------------------
void ofxFBXMesh::computeBlendShapes( ofMesh* aMesh, FbxTime& pTime, FbxAnimLayer * pAnimLayer ) {
    int lBlendShapeDeformerCount = fbxMesh->GetDeformerCount(FbxDeformer::eBlendShape);
//    cout << "Computing blendshapes for " << getName() << endl;
    for(int lBlendShapeIndex = 0; lBlendShapeIndex<lBlendShapeDeformerCount; ++lBlendShapeIndex) {
        FbxBlendShape* lBlendShape = (FbxBlendShape*)fbxMesh->GetDeformer(lBlendShapeIndex, FbxDeformer::eBlendShape);
        
        int lBlendShapeChannelCount = lBlendShape->GetBlendShapeChannelCount();
		for(int lChannelIndex = 0; lChannelIndex<lBlendShapeChannelCount; ++lChannelIndex) {
			FbxBlendShapeChannel* lChannel = lBlendShape->GetBlendShapeChannel(lChannelIndex);
			if(lChannel) {
				// Get the percentage of influence on this channel.
				FbxAnimCurve* lFCurve = fbxMesh->GetShapeChannel(lBlendShapeIndex, lChannelIndex, pAnimLayer);
				if (!lFCurve) continue;
                
				double lWeight          = lFCurve->Evaluate(pTime);
//                cout << "updateMesh lWeight = " << lWeight << " time = " << pTime.GetMilliSeconds() << endl;
                
                int lShapeCount         = lChannel->GetTargetShapeCount();
				double* lFullWeights    = lChannel->GetTargetShapeFullWeights();
                
                // Find out which scope the lWeight falls in.
				int lStartIndex = -1;
				int lEndIndex = -1;
				for(int lShapeIndex = 0; lShapeIndex<lShapeCount; ++lShapeIndex) {
					if(lWeight > 0 && lWeight <= lFullWeights[0]) {
						lEndIndex = 0;
						break;
					}
					if(lWeight > lFullWeights[lShapeIndex] && lWeight < lFullWeights[lShapeIndex+1]) {
						lStartIndex = lShapeIndex;
						lEndIndex = lShapeIndex + 1;
						break;
					}
				}
                
                FbxShape* lStartShape = NULL;
				FbxShape* lEndShape = NULL;
				if(lStartIndex > -1) {
					lStartShape = lChannel->GetTargetShape(lStartIndex);
				}
				if(lEndIndex > -1) {
					lEndShape = lChannel->GetTargetShape(lEndIndex);
				}
                
                //The weight percentage falls between base geometry and the first target shape.
				if(lStartIndex == -1 && lEndShape) {
                    float lEndWeight    = lFullWeights[0];
                    lWeight = (lWeight/lEndWeight);
                    
//                    cout << "updateMesh : weight = " << lWeight << endl;
                    for (int j = 0; j < aMesh->getNumVertices(); j++) {
                        // Add the influence of the shape vertex to the mesh vertex.
                        ofVec3f influence = (toOf(lEndShape->GetControlPoints()[j]) - original.getVertices()[j]) * lWeight;
                        aMesh->getVertices()[j] += influence;
                    }
                    
                } else if(lStartShape && lEndShape) {
                    float lStartWeight  = lFullWeights[lStartIndex];
					float lEndWeight    = lFullWeights[lEndIndex];
                    // Calculate the real weight.
                    lWeight = ofMap(lWeight, lStartWeight, lEndWeight, 0, 1, true);
//                    cout << "updateMesh : weight = " << lWeight << " lStartWeight " << lStartWeight << " lEndWeight " << lEndWeight << endl;
                    //					lWeight = ((lWeight-lStartWeight)/(lEndWeight-lStartWeight)) * 100;
                    for (int j = 0; j < aMesh->getNumVertices(); j++) {
                        // Add the influence of the shape vertex to the mesh vertex.
                        ofVec3f influence = (toOf(lEndShape->GetControlPoints()[j] - lStartShape->GetControlPoints()[j] )) * lWeight;
                        aMesh->getVertices()[j] += influence;
                    }
                }
                
            }
        }
    }
}

void ofxFBXMesh::computeSkinDeformation( FbxAMatrix& pGlobalPosition, FbxTime& pTime, FbxAnimLayer* pAnimLayer, FbxVector4* pVertexArray, FbxVector4* pNormalsArray, FbxPose* pPose ) {
    FbxSkin * lSkinDeformer = (FbxSkin *)fbxMesh->GetDeformer(0, FbxDeformer::eSkin);
	FbxSkin::EType lSkinningType = lSkinDeformer->GetSkinningType();
    
	if(lSkinningType == FbxSkin::eLinear || lSkinningType == FbxSkin::eRigid) {
//        cout << "ofxFBXMesh :: computeSkinDeformation :: eRigid " << endl;
        computeLinearDeformation(pGlobalPosition, fbxMesh, pTime, pVertexArray, pPose, false );
        if( pNormalsArray != NULL ) {
//            cout << "ofxFBXMesh :: computeSkinDeformation :: calculate normals array " << endl;
            computeLinearDeformation( pGlobalPosition, fbxMesh, pTime, pNormalsArray, pPose, true );
        }
    } else if(lSkinningType == FbxSkin::eDualQuaternion) {
//        cout << "ofxFBXMesh :: computeSkinDeformation :: eDualQuaternion " << endl;
		computeDualQuaternionDeformation(pGlobalPosition, fbxMesh, pTime, pVertexArray, pPose, false );
        if( pNormalsArray != NULL ) {
            computeLinearDeformation( pGlobalPosition, fbxMesh, pTime, pNormalsArray, pPose, true );
        }
	} else if(lSkinningType == FbxSkin::eBlend) {
//        cout << "ofxFBXMesh :: computeSkinDeformation :: eBlend " << endl;
		int lVertexCount = fbxMesh->GetControlPointsCount();
		FbxVector4* lVertexArrayLinear = new FbxVector4[lVertexCount];
		memcpy(lVertexArrayLinear, fbxMesh->GetControlPoints(), lVertexCount * sizeof(FbxVector4));
        
		FbxVector4* lVertexArrayDQ = new FbxVector4[lVertexCount];
		memcpy(lVertexArrayDQ, fbxMesh->GetControlPoints(), lVertexCount * sizeof(FbxVector4));
        
		computeLinearDeformation(pGlobalPosition, fbxMesh, pTime, lVertexArrayLinear, pPose, false );
		computeDualQuaternionDeformation(pGlobalPosition, fbxMesh, pTime, lVertexArrayDQ, pPose, false );
        
		// To blend the skinning according to the blend weights
		// Final vertex = DQSVertex * blend weight + LinearVertex * (1- blend weight)
		// DQSVertex: vertex that is deformed by dual quaternion skinning method;
		// LinearVertex: vertex that is deformed by classic linear skinning method;
		int lBlendWeightsCount = lSkinDeformer->GetControlPointIndicesCount();
		for(int lBWIndex = 0; lBWIndex<lBlendWeightsCount; ++lBWIndex) {
			double lBlendWeight = lSkinDeformer->GetControlPointBlendWeights()[lBWIndex];
			pVertexArray[lBWIndex] = lVertexArrayDQ[lBWIndex] * lBlendWeight + lVertexArrayLinear[lBWIndex] * (1 - lBlendWeight);
		}
        
        delete [] lVertexArrayLinear;
        delete [] lVertexArrayDQ;
        
        if( pNormalsArray != NULL ) {
            FbxVector4* lNormalArrayLinear = new FbxVector4[lVertexCount];
            memcpy( lNormalArrayLinear, mNormalsArray, lVertexCount * sizeof(FbxVector4));
            
            FbxVector4* lNormalArrayDQ = new FbxVector4[lVertexCount];
            memcpy(lNormalArrayDQ, mNormalsArray, lVertexCount * sizeof(FbxVector4));
            
            computeLinearDeformation(pGlobalPosition, fbxMesh, pTime, lNormalArrayLinear, pPose, true );
            computeDualQuaternionDeformation(pGlobalPosition, fbxMesh, pTime, lNormalArrayDQ, pPose, true );
            
            // To blend the skinning according to the blend weights
            // Final vertex = DQSVertex * blend weight + LinearVertex * (1- blend weight)
            // DQSVertex: vertex that is deformed by dual quaternion skinning method;
            // LinearVertex: vertex that is deformed by classic linear skinning method;
            int lBlendWeightsCount = lSkinDeformer->GetControlPointIndicesCount();
            for(int lBWIndex = 0; lBWIndex<lBlendWeightsCount; ++lBWIndex) {
                double lBlendWeight = lSkinDeformer->GetControlPointBlendWeights()[lBWIndex];
                pVertexArray[lBWIndex] = lNormalArrayDQ[lBWIndex] * lBlendWeight + lNormalArrayLinear[lBWIndex] * (1 - lBlendWeight);
            }
            
            delete [] lNormalArrayLinear;
            delete [] lNormalArrayDQ;
        }
        
	}
}

// Deform the vertex array in classic linear way.
void ofxFBXMesh::computeLinearDeformation(FbxAMatrix& pGlobalPosition,
                                          FbxMesh* pMesh,
                                          FbxTime& pTime,
                                          FbxVector4* pVertexArray,
                                          FbxPose* pPose,
                                          bool bNormals) {
	// All the links must have the same link mode.
	FbxCluster::ELinkMode lClusterMode = ((FbxSkin*)fbxMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();
    
	int lVertexCount = pMesh->GetControlPointsCount();
    
//    cout << "control points count = " << lVertexCount << " mesh verts = " << mesh.getNumVertices() << endl;
    
	FbxAMatrix* lClusterDeformation = new FbxAMatrix[lVertexCount];
	memset(lClusterDeformation, 0, lVertexCount * sizeof(FbxAMatrix));
    
	double* lClusterWeight = new double[lVertexCount];
	memset(lClusterWeight, 0, lVertexCount * sizeof(double));
    
	if (lClusterMode == FbxCluster::eAdditive) {
		for (int i = 0; i < lVertexCount; ++i) {
			lClusterDeformation[i].SetIdentity();
		}
	}
    
	// For all skins and all clusters, accumulate their deformation and weight
	// on each vertices and store them in lClusterDeformation and lClusterWeight.
	int lSkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);
    
//    cout << "computeLinearDeformation :: number of skins = " << lSkinCount << endl;
    
	for ( int lSkinIndex=0; lSkinIndex<lSkinCount; ++lSkinIndex) {
		FbxSkin * lSkinDeformer = (FbxSkin *)pMesh->GetDeformer(lSkinIndex, FbxDeformer::eSkin);
		
		int lClusterCount = lSkinDeformer->GetClusterCount();
		for ( int lClusterIndex=0; lClusterIndex<lClusterCount; ++lClusterIndex) {
			FbxCluster* lCluster = lSkinDeformer->GetCluster(lClusterIndex);
			if (!lCluster->GetLink())
				continue;
            
//            cout << "lClusterIndex: " << lClusterIndex << endl;
            
			FbxAMatrix lVertexTransformMatrix;
			computeClusterDeformation(pGlobalPosition, pMesh, lCluster, lVertexTransformMatrix, pTime, pPose, bNormals );
            
			int lVertexIndexCount = lCluster->GetControlPointIndicesCount();
			for (int k = 0; k < lVertexIndexCount; ++k) {
				int lIndex = lCluster->GetControlPointIndices()[k];
                
				// Sometimes, the mesh can have less points than at the time of the skinning
				// because a smooth operator was active when skinning but has been deactivated during export.
				if (lIndex >= lVertexCount)
					continue;
                
				double lWeight = lCluster->GetControlPointWeights()[k];
                
				if (lWeight == 0.0) {
					continue;
				}
                
				// Compute the influence of the link on the vertex.
				FbxAMatrix lInfluence = lVertexTransformMatrix;
				MatrixScale(lInfluence, lWeight);
                
				if (lClusterMode == FbxCluster::eAdditive) {
//                    cout << "computeLinearDeformation :: clustermode = eAdditive" << endl;
					// Multiply with the product of the deformations on the vertex.
					MatrixAddToDiagonal(lInfluence, 1.0 - lWeight);
					lClusterDeformation[lIndex] = lInfluence * lClusterDeformation[lIndex];
                    
					// Set the link to 1.0 just to know this vertex is influenced by a link.
					lClusterWeight[lIndex] = 1.0;
				} else // lLinkMode == FbxCluster::eNormalize || lLinkMode == FbxCluster::eTotalOne
				{
//                    cout << "computeLinearDeformation :: clustermode = !!eAdditive" << endl;
//                    if(k == 0) cout << "computeLinearDeformation :: clustermode != eAdditive " << lInfluence << endl;
					// Add to the sum of the deformations on the vertex.
					MatrixAdd(lClusterDeformation[lIndex], lInfluence);
                    
					// Add to the sum of weights to either normalize or complete the vertex.
					lClusterWeight[lIndex] += lWeight;
				}
			}//For each vertex
		}//lClusterCount
	}
    
	//Actually deform each vertices here by information stored in lClusterDeformation and lClusterWeight
//    cout << "going to deform the vertices now " << endl;
	for (int i = 0; i < lVertexCount; i++) {
		FbxVector4 lSrcVertex   = pVertexArray[i];
		FbxVector4& lDstVertex  = pVertexArray[i];
        
		double lWeight = lClusterWeight[i];
        
		// Deform the vertex if there was at least a link with an influence on the vertex,
		if (lWeight != 0.0) {
			lDstVertex = lClusterDeformation[i].MultT(lSrcVertex);
			if (lClusterMode == FbxCluster::eNormalize) {
				// In the normalized link mode, a vertex is always totally influenced by the links.
//                cout << i << " weight: " << lWeight << " enormalize " << endl;
				lDstVertex /= lWeight;
			} else if (lClusterMode == FbxCluster::eTotalOne) {
				// In the total 1 link mode, a vertex can be partially influenced by the links.
//                cout << i << " weight: " << lWeight << " eTotalOne " << endl;
				lSrcVertex *= (1.0 - lWeight);
				lDstVertex += lSrcVertex;
			}
		} else {
//            cout << i << " weight is zero!! " << endl;
        }
	}
    
	delete [] lClusterDeformation;
	delete [] lClusterWeight;
}


// Deform the vertex array in Dual Quaternion Skinning way.
void ofxFBXMesh::computeDualQuaternionDeformation(FbxAMatrix& pGlobalPosition,
                                                  FbxMesh* pMesh,
                                                  FbxTime& pTime,
                                                  FbxVector4* pVertexArray,
                                                  FbxPose* pPose,
                                                  bool bNormals) {
	// All the links must have the same link mode.
	FbxCluster::ELinkMode lClusterMode = ((FbxSkin*)pMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();
    
	int lVertexCount    = pMesh->GetControlPointsCount();
	int lSkinCount      = pMesh->GetDeformerCount(FbxDeformer::eSkin);
    
	FbxDualQuaternion* lDQClusterDeformation = new FbxDualQuaternion[lVertexCount];
	memset(lDQClusterDeformation, 0, lVertexCount * sizeof(FbxDualQuaternion));
    
	double* lClusterWeight = new double[lVertexCount];
	memset(lClusterWeight, 0, lVertexCount * sizeof(double));
    
	// For all skins and all clusters, accumulate their deformation and weight
	// on each vertices and store them in lClusterDeformation and lClusterWeight.
	for ( int lSkinIndex=0; lSkinIndex<lSkinCount; ++lSkinIndex)
	{
		FbxSkin * lSkinDeformer = (FbxSkin *)pMesh->GetDeformer(lSkinIndex, FbxDeformer::eSkin);
		int lClusterCount = lSkinDeformer->GetClusterCount();
		for ( int lClusterIndex=0; lClusterIndex<lClusterCount; ++lClusterIndex)
		{
			FbxCluster* lCluster = lSkinDeformer->GetCluster(lClusterIndex);
			if (!lCluster->GetLink())
				continue;
            
			FbxAMatrix lVertexTransformMatrix;
			computeClusterDeformation(pGlobalPosition, pMesh, lCluster, lVertexTransformMatrix, pTime, pPose, bNormals );
            
			FbxQuaternion lQ = lVertexTransformMatrix.GetQ();
			FbxVector4 lT = lVertexTransformMatrix.GetT();
			FbxDualQuaternion lDualQuaternion(lQ, lT);
            
			int lVertexIndexCount = lCluster->GetControlPointIndicesCount();
			for (int k = 0; k < lVertexIndexCount; ++k)
			{
				int lIndex = lCluster->GetControlPointIndices()[k];
                
				// Sometimes, the mesh can have less points than at the time of the skinning
				// because a smooth operator was active when skinning but has been deactivated during export.
				if (lIndex >= lVertexCount)
					continue;
                
				double lWeight = lCluster->GetControlPointWeights()[k];
                
				if (lWeight == 0.0)
					continue;
                
				// Compute the influence of the link on the vertex.
				FbxDualQuaternion lInfluence = lDualQuaternion * lWeight;
				if (lClusterMode == FbxCluster::eAdditive)
				{
					// Simply influenced by the dual quaternion.
					lDQClusterDeformation[lIndex] = lInfluence;
                    
					// Set the link to 1.0 just to know this vertex is influenced by a link.
					lClusterWeight[lIndex] = 1.0;
				}
				else // lLinkMode == FbxCluster::eNormalize || lLinkMode == FbxCluster::eTotalOne
				{
					if(lClusterIndex == 0)
					{
						lDQClusterDeformation[lIndex] = lInfluence;
					}
					else
					{
						// Add to the sum of the deformations on the vertex.
						// Make sure the deformation is accumulated in the same rotation direction.
						// Use dot product to judge the sign.
						double lSign = lDQClusterDeformation[lIndex].GetFirstQuaternion().DotProduct(lDualQuaternion.GetFirstQuaternion());
						if( lSign >= 0.0 )
						{
							lDQClusterDeformation[lIndex] += lInfluence;
						}
						else
						{
							lDQClusterDeformation[lIndex] -= lInfluence;
						}
					}
					// Add to the sum of weights to either normalize or complete the vertex.
					lClusterWeight[lIndex] += lWeight;
				}
			}//For each vertex
		}//lClusterCount
	}
    
	//Actually deform each vertices here by information stored in lClusterDeformation and lClusterWeight
	for (int i = 0; i < lVertexCount; i++) {
		FbxVector4 lSrcVertex = pVertexArray[i];
		FbxVector4& lDstVertex = pVertexArray[i];
        
		double lWeightSum = lClusterWeight[i];
        
		// Deform the vertex if there was at least a link with an influence on the vertex,
		if (lWeightSum != 0.0)
		{
			lDQClusterDeformation[i].Normalize();
			lDstVertex = lDQClusterDeformation[i].Deform(lDstVertex);
            
			if (lClusterMode == FbxCluster::eNormalize)
			{
				// In the normalized link mode, a vertex is always totally influenced by the links.
				lDstVertex /= lWeightSum;
			}
			else if (lClusterMode == FbxCluster::eTotalOne)
			{
				// In the total 1 link mode, a vertex can be partially influenced by the links. 
				lSrcVertex *= (1.0 - lWeightSum);
				lDstVertex += lSrcVertex;
			}
		}
	}
    
	delete [] lDQClusterDeformation;
	delete [] lClusterWeight;
}


//Compute the transform matrix that the cluster will transform the vertex.
void ofxFBXMesh::computeClusterDeformation(FbxAMatrix& pGlobalPosition,
                                           FbxMesh* pMesh,
                                           FbxCluster* pCluster,
                                           FbxAMatrix& pVertexTransformMatrix,
                                           FbxTime pTime,
                                           FbxPose* pPose, bool bNormal ) {
    
    FbxCluster::ELinkMode lClusterMode = pCluster->GetLinkMode();
    
    ofxFBXBone* bone        = NULL;
    ofxFBXCluster* cluster  = NULL; 
    
    if(pCluster->GetLink()) {
        FbxNode* boneNode = pCluster->GetLink();
        if(boneNode->GetUserDataPtr()) {
            bone = static_cast<ofxFBXBone *>(boneNode->GetUserDataPtr());
        }
    }
    
    if(bone != NULL) {
        if(pCluster->GetUserDataPtr()) {
            cluster = static_cast<ofxFBXCluster *>(pCluster->GetUserDataPtr());
//            cluster->update( pTime, pPose );
        }
    }
    
    if( bone != NULL && cluster != NULL ) {
//        cout << "computeClusterDeformation: bone != NULL && cluster != NULL " << bone->getName() << endl;
//        cout << "We have cached cluster and bone! " << bone->getName() << endl;
        pVertexTransformMatrix = cluster->preTrans * bone->fbxTransform * cluster->postTrans;
        if( bNormal ) {
            pVertexTransformMatrix = pVertexTransformMatrix.Inverse();
            pVertexTransformMatrix = pVertexTransformMatrix.Transpose();
        }
    } else {
        
        FbxAMatrix lReferenceGlobalInitPosition;
        FbxAMatrix lReferenceGlobalCurrentPosition;
        FbxAMatrix lAssociateGlobalInitPosition;
        FbxAMatrix lAssociateGlobalCurrentPosition;
        FbxAMatrix lClusterGlobalInitPosition;
        FbxAMatrix lClusterGlobalCurrentPosition;
        
        FbxAMatrix lReferenceGeometry;
        FbxAMatrix lAssociateGeometry;
        FbxAMatrix lClusterGeometry;
        
        FbxAMatrix lClusterRelativeInitPosition;
        FbxAMatrix lClusterRelativeCurrentPositionInverse;
        
        // nothing is setup for the control of the bones, so we are just doing animation
        // right now, can't do animation and control the bones at the same time.
        if (lClusterMode == FbxCluster::eAdditive && pCluster->GetAssociateModel()) {
            
//            cout << "computeClusterDeformation: FbxCluster::eAdditive " << endl;
            
            pCluster->GetTransformAssociateModelMatrix(lAssociateGlobalInitPosition);
            // Geometric transform of the model
            lAssociateGeometry = GetGeometry(pCluster->GetAssociateModel());
            lAssociateGlobalInitPosition *= lAssociateGeometry;
            lAssociateGlobalCurrentPosition = GetGlobalPosition(pCluster->GetAssociateModel(), pTime, pPose);
            
            pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
            // Multiply lReferenceGlobalInitPosition by Geometric Transformation
            lReferenceGeometry = GetGeometry(pMesh->GetNode());
            lReferenceGlobalInitPosition *= lReferenceGeometry;
            lReferenceGlobalCurrentPosition = pGlobalPosition;
            
            // Get the link initial global position and the link current global position.
            pCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
            // Multiply lClusterGlobalInitPosition by Geometric Transformation
            lClusterGeometry = GetGeometry(pCluster->GetLink());
            lClusterGlobalInitPosition *= lClusterGeometry;
            lClusterGlobalCurrentPosition = GetGlobalPosition(pCluster->GetLink(), pTime, pPose);
            
            // Compute the shift of the link relative to the reference.
            //ModelM-1 * AssoM * AssoGX-1 * LinkGX * LinkM-1*ModelM
            pVertexTransformMatrix = lReferenceGlobalInitPosition.Inverse() * lAssociateGlobalInitPosition * lAssociateGlobalCurrentPosition.Inverse() *
            lClusterGlobalCurrentPosition * lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;
        } else {
            
//            cout << "computeClusterDeformation: !!FbxCluster::eAdditive " << endl;
            
            pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
            lReferenceGlobalCurrentPosition = pGlobalPosition;
            // Multiply lReferenceGlobalInitPosition by Geometric Transformation
            lReferenceGeometry = GetGeometry(pMesh->GetNode());
            lReferenceGlobalInitPosition *= lReferenceGeometry;
            
            // Get the link initial global position and the link current global position.
            pCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
            lClusterGlobalCurrentPosition = GetGlobalPosition(pCluster->GetLink(), pTime, pPose);
            
            // Compute the initial position of the link relative to the reference.
            lClusterRelativeInitPosition = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;
            
            // Compute the current position of the link relative to the reference.
            lClusterRelativeCurrentPositionInverse = lReferenceGlobalCurrentPosition.Inverse() * lClusterGlobalCurrentPosition;
            
            // Compute the shift of the link relative to the reference.
            pVertexTransformMatrix = lClusterRelativeCurrentPositionInverse * lClusterRelativeInitPosition;
        }
    }
}

//--------------------------------------------------------------
void ofxFBXMesh::populateNormals( FbxVector4* pNormalsArray ) {
    
    const int lVertexCount = fbxMesh->GetControlPointsCount();
    
    //get the normal element
    FbxGeometryElementNormal* lNormalElement = fbxMesh->GetElementNormal();
    if(lNormalElement)
    {
        //mapping mode is by control points. The mesh should be smooth and soft.
        //we can get normals by retrieving each control point
        if( lNormalElement->GetMappingMode() == FbxGeometryElement::eByControlPoint )
        {
            //Let's get normals of each vertex, since the mapping mode of normal element is by control point
            for(int lVertexIndex = 0; lVertexIndex < fbxMesh->GetControlPointsCount(); lVertexIndex++)
            {
                int lNormalIndex = 0;
                //reference mode is direct, the normal index is same as vertex index.
                //get normals by the index of control vertex
                if( lNormalElement->GetReferenceMode() == FbxGeometryElement::eDirect )
                    lNormalIndex = lVertexIndex;
                
                //reference mode is index-to-direct, get normals by the index-to-direct
                if(lNormalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
                    lNormalIndex = lNormalElement->GetIndexArray().GetAt(lVertexIndex);
                
                //Got normals of each vertex.
                FbxVector4 lNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
                pNormalsArray[ lVertexIndex ] = lNormal;
//                if( gVerbose ) FBXSDK_printf("normals for vertex[%d]: %f %f %f %f \n", lVertexIndex, lNormal[0], lNormal[1], lNormal[2], lNormal[3]);
                
                //add your custom code here, to output normals or get them into a list, such as KArrayTemplate<FbxVector4>
                //. . .
            }//end for lVertexIndex
        }//end eByControlPoint
        //mapping mode is by polygon-vertex.
        //we can get normals by retrieving polygon-vertex.
        else if(lNormalElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
        {
            int lIndexByPolygonVertex = 0;
            //Let's get normals of each polygon, since the mapping mode of normal element is by polygon-vertex.
            for(int lPolygonIndex = 0; lPolygonIndex < fbxMesh->GetPolygonCount(); lPolygonIndex++)
            {
                //get polygon size, you know how many vertices in current polygon.
                int lPolygonSize = fbxMesh->GetPolygonSize(lPolygonIndex);
                //retrieve each vertex of current polygon.
                for(int i = 0; i < lPolygonSize; i++)
                {
                    int lNormalIndex = 0;
                    //reference mode is direct, the normal index is same as lIndexByPolygonVertex.
                    if( lNormalElement->GetReferenceMode() == FbxGeometryElement::eDirect )
                        lNormalIndex = lIndexByPolygonVertex;
                    
                    //reference mode is index-to-direct, get normals by the index-to-direct
                    if(lNormalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
                        lNormalIndex = lNormalElement->GetIndexArray().GetAt(lIndexByPolygonVertex);
                    
                    //Got normals of each polygon-vertex.
                    FbxVector4 lNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
                    const int lControlPointIndex = fbxMesh->GetPolygonVertex( lPolygonIndex, i );
                    pNormalsArray[ lControlPointIndex ] = lNormal;
                    
//                    if( gVerbose ) FBXSDK_printf("normals for polygon[%d]vertex[%d]: %f %f %f %f \n",
//                                                 lPolygonIndex, i, lNormal[0], lNormal[1], lNormal[2], lNormal[3]);
                    
                    
                    
                    //add your custom code here, to output normals or get them into a list, such as KArrayTemplate<FbxVector4>
                    //. . .
                    
                    lIndexByPolygonVertex++;
                }//end for i //lPolygonSize
            }//end for lPolygonIndex //PolygonCount
            
        }//end eByPolygonVertex
    }//end if lNormalElement
}




