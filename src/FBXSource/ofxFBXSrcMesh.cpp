//
//  ofxFBXSrcMesh.cpp
//  ConnectionsWall-Nick
//
//  Created by Nick Hardeman on 7/11/19.
//

#include "ofxFBXSrcMesh.h"
#include "ofxFBXUtils.h"

using namespace ofxFBXSource;

//--------------------------------------------------------------
Mesh::Mesh() {
    fbxMesh = NULL;
    mNormalsArray = NULL;
}

//--------------------------------------------------------------
Mesh::~Mesh() {
    if( mNormalsArray != NULL ) {
        delete [] mNormalsArray;
        mNormalsArray = NULL;
    }
}

//--------------------------------------------------------------
void Mesh::setup( FbxNode *pNode ) {
    ofxFBXSource::Node::setup( pNode );
    setFBXMesh( pNode->GetMesh() );
}

//--------------------------------------------------------------
void Mesh::setFBXMesh( FbxMesh* lMesh ) {
    fbxMesh = lMesh;
    mesh.clear();
    
    // from ViewScene Example included with the FBX SDK //
    if (!lMesh->GetNode()) {
        ofLogError("ofxFBXMesh") << " error setFBXMesh, lMesh->GetNode failed" << endl;
        return;
    }
    
    ofLogVerbose() << "** ofxFBXMesh :: " << getName() << " attempting to parse ******* ";
    
    vector< string > eMappingModeNames = {"eNone", "eByControlPoint", "eByPolygonVertex", "eByPolygon", "eByEdge", "eAllSame"};
    vector<string> eRefModes = {"eDirect", "eIndex", "eIndexToDirect" };
    
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
                        if(subMeshes.size() < lMaterialIndex + 1) {
                            subMeshes.resize(lMaterialIndex + 1);
                        }
                    }
                }
            }
        }
    }
    
    // split the vertices based on the materials //
    // so that it renders properly //
    //    if( subMeshes.size() > 1 ) {
    fbxMesh->SplitPoints();
    //    }
    
    if(subMeshes.size() == 0) {
        subMeshes.resize(1);
    }
    
    // add in the vertices //
    const FbxVector4 * lControlPoints = lMesh->GetControlPoints();
    int controlPointCount = lMesh->GetControlPointsCount();
    mesh.getVertices().resize( controlPointCount );
    
    ofLogVerbose() << "ofxFBXMesh :: number of vertices: " << mesh.getNumVertices() << " sub meshes: " << subMeshes.size();
    
    // INDICES /////
    mesh.clearIndices();
    // populate the indices //
    for( int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; lPolygonIndex++ ) {
        int faceSize = fbxMesh->GetPolygonSize( lPolygonIndex );
        for( int lVerticeIndex = 0; lVerticeIndex < faceSize; lVerticeIndex++ ) {
            const int lControlPointIndex = fbxMesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex);
            mesh.getVertices()[ lControlPointIndex ] = glm::vec3(lControlPoints[lControlPointIndex][0],
                                                                 lControlPoints[lControlPointIndex][1],
                                                                 lControlPoints[lControlPointIndex][2] );
            mesh.addIndex( lControlPointIndex );
        }
    }
    
    ofLogVerbose() << "ofxFBXMesh :: number of indices: " << mesh.getNumIndices() << " number of polygons: " << lPolygonCount;
    
    
    // TODO: Account for if all mapping modes are not by control point //
    
    // Normals //
    if( lMesh->GetElementNormalCount() > 0 && lMesh->GetElementNormal() ) {
        const FbxGeometryElementNormal * lNormalElement     = lMesh->GetElementNormal(0);
        mNormalMappingMode = lNormalElement->GetMappingMode();
        
        ofLogVerbose() << "ofxFBXMesh :: normals detected. mapping mode: " << eMappingModeNames[ mNormalMappingMode ];
        
        mesh.getNormals().resize( controlPointCount );
        
        FbxVector4 lCurrentNormal;
        if( mNormalMappingMode == FbxGeometryElement::eByControlPoint ) {
            for(int i = 0; i < controlPointCount; i++ ) {
                int lNormalIndex = i;
                if (lNormalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect) {
                    lNormalIndex = lNormalElement->GetIndexArray().GetAt(i);
                }
                lCurrentNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
                mesh.getNormals()[i] = glm::vec3( lCurrentNormal[0], lCurrentNormal[1], lCurrentNormal[2] );
            }
        } else if( mNormalMappingMode == FbxGeometryElement::eByPolygonVertex ) {
            for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex) {
                int faceSize = fbxMesh->GetPolygonSize( lPolygonIndex );
                for( int lVerticeIndex = 0; lVerticeIndex < faceSize; lVerticeIndex++ ) {
                    const int lControlPointIndex = fbxMesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex);
                    if(fbxMesh->GetPolygonVertexNormal( lPolygonIndex, lVerticeIndex, lCurrentNormal )) {
                        mesh.getNormals()[ lControlPointIndex ] = glm::vec3( lCurrentNormal[0], lCurrentNormal[1], lCurrentNormal[2] );
                    }
                }
            }
        } else {
            ofLogVerbose() << "ofxFBXMesh :: clearing normals, only eByControlPoint and eByPolygonVertex supported.";
            mNormalMappingMode = FbxGeometryElement::eNone;
            mesh.clearNormals();
        }
    }
    
    
    if( fbxMesh->GetElementUVCount() ) mesh.getTexCoords().resize( mesh.getNumVertices() );
    
    // TEX COORDS CODE BASED ON CODE FROM FBX SDK 2016.1 example //
    //iterating over all uv sets
    for (int lUVSetIndex = 0; lUVSetIndex < fbxMesh->GetElementUVCount(); lUVSetIndex++) {
        const FbxGeometryElementUV* lUVElement = fbxMesh->GetElementUV( lUVSetIndex );
        
        if(!lUVElement)
            continue;
        
        FbxGeometryElement::EMappingMode lUVMappingMode = lUVElement->GetMappingMode();
        ofLogVerbose() << "ofxFBXMesh :: uv mapping mode: " << eMappingModeNames[ lUVMappingMode ];
        // only support mapping mode eByPolygonVertex and eByControlPoint
        if(lUVElement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex &&
           lUVElement->GetMappingMode() != FbxGeometryElement::eByControlPoint ) {
            continue;
        }
        
        //index array, where holds the index referenced to the uv data
        const bool lUseIndex = lUVElement->GetReferenceMode() != FbxGeometryElement::eDirect;
        const int lIndexCount= (lUseIndex) ? lUVElement->GetIndexArray().GetCount() : 0;
        
        //iterating through the data by polygon
        const int lPolyCount = fbxMesh->GetPolygonCount();
        
        if( lUVElement->GetMappingMode() == FbxGeometryElement::eByControlPoint ) {
            for( int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex ) {
                // build the max index array that we need to pass into MakePoly
                const int lPolySize = fbxMesh->GetPolygonSize(lPolyIndex);
                for( int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex ) {
                    FbxVector2 lUVValue;
                    //get the index of the current vertex in control points array
                    int lPolyVertIndex = fbxMesh->GetPolygonVertex(lPolyIndex,lVertIndex);
                    //the UV index depends on the reference mode
                    int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyVertIndex) : lPolyVertIndex;
                    lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);
                    mesh.getTexCoords()[ lPolyVertIndex ] = glm::vec2( lUVValue[0], lUVValue[1] );
                }
            }
        } else if (lUVElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {
            int lIndexByPolygonVertex = 0;
            FbxVector2 lUVValue;
            //Let's get normals of each polygon, since the mapping mode of normal element is by polygon-vertex.
            for(int lPolygonIndex = 0; lPolygonIndex < fbxMesh->GetPolygonCount(); lPolygonIndex++) {
                //get polygon size, you know how many vertices in current polygon.
                int lPolygonSize = fbxMesh->GetPolygonSize(lPolygonIndex);
                ofLogVerbose() << "ofxFBXMesh :: polygon size: " << lPolygonSize << " ref mode " << eRefModes[lUVElement->GetReferenceMode()];
                //retrieve each vertex of current polygon.
                for(int i = 0; i < lPolygonSize; i++) {
                    int lNormalIndex = 0;
                    //reference mode is direct, the normal index is same as lIndexByPolygonVertex.
                    if( lUVElement->GetReferenceMode() == FbxGeometryElement::eDirect )
                        lNormalIndex = lIndexByPolygonVertex;
                    
                    //reference mode is index-to-direct, get normals by the index-to-direct
                    if( lUVElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
                        lNormalIndex = lUVElement->GetIndexArray().GetAt(lIndexByPolygonVertex);
                    
                    //Got normals of each polygon-vertex.
                    lUVValue = lUVElement->GetDirectArray().GetAt(lNormalIndex);
                    const int lControlPointIndex = fbxMesh->GetPolygonVertex( lPolygonIndex, i );
                    mesh.getTexCoords()[ lControlPointIndex ] = glm::vec2( lUVValue[0], lUVValue[1] );
                    
                    lIndexByPolygonVertex++;
                }//end for i //lPolygonSize
            }//end for lPolygonIndex //PolygonCount
        }
        
        ofLogVerbose( "ofxFBXMesh : num verts: " ) << mesh.getNumVertices() << " tcoords: " << mesh.getNumTexCoords();
    }
    
    
    // VERTEX COLORS ////
    if( lMesh->GetElementVertexColorCount() > 0 && lMesh->GetElementVertexColor() ) {
        const FbxLayerElementVertexColor* pVertexColorElement   = lMesh->GetElementVertexColor();
        FbxGeometryElement::EMappingMode lVertexColorMappingMode = pVertexColorElement->GetMappingMode();
        
        mesh.getColors().resize( controlPointCount );
        
        ofLogVerbose() << "ofxFBXMesh :: vertex colors detected: mapping mode: " << eMappingModeNames[ lVertexColorMappingMode ];
        
        FbxColor lVertexColor;
        if( lVertexColorMappingMode == FbxGeometryElement::eByControlPoint ) {
            int lVertexColorIndex = 0;
            for(int i = 0; i < controlPointCount; i++ ) {
                lVertexColorIndex = i;
                if ( pVertexColorElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect) {
                    lVertexColorIndex = pVertexColorElement->GetIndexArray().GetAt(i);
                }
                lVertexColor = pVertexColorElement->GetDirectArray().GetAt(lVertexColorIndex);
                mesh.getColors()[i].set( ofFloatColor(lVertexColor.mRed, lVertexColor.mGreen, lVertexColor.mBlue, lVertexColor.mAlpha ) );
            }
        } else {
            ofLogVerbose() << "ofxFBXMesh :: clearing vertex colors, only eByControlPoint supported.";
            mesh.clearColors();
            mesh.disableColors();
            lVertexColorMappingMode = FbxGeometryElement::eNone;
        }
    } else {
        mesh.clearColors();
        mesh.disableColors();
    }
    
    
    // update the meshes offsets and indices counts //
    for( int i = 0; i < subMeshes.size(); i++ ) {
        subMeshes[i].triangleCount = 0;
        subMeshes[i].totalIndices = 0;
    }
    
    // update the sub meshes with the proper amount of indices //
    for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex) {
        // The material for current face.
        int lMaterialIndex = 0;
        if (lMaterialIndice && lMaterialMappingMode == FbxGeometryElement::eByPolygon) {
            lMaterialIndex = lMaterialIndice->GetAt(lPolygonIndex);
            if(lMaterialIndex < 0) {
                lMaterialIndex = 0;
            }
        }
        
        subMeshes[lMaterialIndex].triangleCount += 1;
        subMeshes[lMaterialIndex].totalIndices += lMesh->GetPolygonSize( lPolygonIndex );
    }
    
    int toffset = 0;
    for( int i = 0; i < subMeshes.size(); i++ ) {
        subMeshes[i].indexOffset = toffset;
        toffset += subMeshes[i].totalIndices;
    }
    
    
    
    //    cout << "--------------------------------------- " << endl << endl;
    
    //ofLogVerbose("ofxFBXMesh") << "sub meshes: " <<   subMeshes.size();
    for( int i = 0; i < subMeshes.size(); i++ ) {
        ofLogVerbose("ofxFBXMesh") << i << " submesh totalindicies = " << subMeshes[i].totalIndices << " index offset: " << subMeshes[i].indexOffset << " total verts = " << mesh.getNumVertices() << " polygonos = " << (lPolygonCount*3);
    }
    ofLogVerbose("ofxFBXMesh") << getName() << " parse complete." << endl;
    //ofLogVerbose("ofxFBXMesh ") << "verts: " << mesh.getNumVertices() << " indices: " << mesh.getNumIndices() << " normals: " << mesh.getNumNormals() << " tex coords: " << mesh.getNumTexCoords();
    
    veebs.setMesh( mesh, GL_STREAM_DRAW );
    original = mesh;
    
    if( mesh.hasNormals() ) {
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
            subMeshes[ lIndex ].materialPtr = static_cast<ofxFBXSource::MeshMaterial *>(lMaterial->GetUserDataPtr());
        }
    }
    
}

//--------------------------------------------------------------
void Mesh::configureMesh( ofMesh& aMesh ) {
    aMesh = mesh;
}

#pragma mark - Update
//--------------------------------------------------------------
void Mesh::update( FbxTime& pTime, FbxPose* pPose ) {
    
//    cout << "ofxFBXSource :: Mesh : update : " << getName() << " has parent: " << ( mFbxNode->GetParent() ? "yes":"no") << endl;
    if( fbxMesh->GetNode() && mFbxNode ) {
        if( !mFbxNode->GetParent() ) {
            FbxAMatrix lGlobalPosition = GetGlobalPosition(fbxMesh->GetNode(), pTime, NULL );
            setLocalTransformMatrix(lGlobalPosition);
        } else {
            FbxAMatrix lLocalPosition = GetLocalPositionForNode(fbxMesh->GetNode(), pTime, NULL );
            setLocalTransformMatrix(lLocalPosition);
        }
    }
    
//    FbxAMatrix lGlobalPosition = GetGlobalPosition( fbxMesh->GetNode(), pTime, NULL );
//    //    FbxAMatrix lGlobalPosition = GetLocalPosition( fbxMesh->GetNode(), pTime, NULL );
//    setLocalTransformMatrix(lGlobalPosition);
//    //    setTransformMatrix(lGlobalPosition);
//    //    glm::mat4 ofgpos = fbxToOf(lGlobalPosition);
//    //    setTransformMatrix( ofgpos );
}

//--------------------------------------------------------------
void Mesh::update( int aAnimIndex, signed long aMillis ) {
    ofxFBXSource::Node::update( aAnimIndex, aMillis );
}

//--------------------------------------------------------------
void Mesh::update( int aAnimIndex1, signed long aAnim1Millis, int aAnimIndex2, signed long aAnim2Millis, float aMixPct ) {
    ofxFBXSource::Node::update( aAnimIndex1, aAnim1Millis, aAnimIndex2, aAnim2Millis, aMixPct );
}

//--------------------------------------------------------------
void Mesh::_configureMeshFromSrcMesh( ofMesh* aSrcMesh, ofMesh* amesh ) {
    if(aSrcMesh == nullptr || amesh == nullptr) return;
    if( aSrcMesh->getNumVertices() < 3 ) return;
//    ofMesh* srcMesh = &akey.mesh;
    
    if( aSrcMesh != nullptr ) {
        if( aSrcMesh->getNumColors() != amesh->getNumColors() ) {
            std::copy( aSrcMesh->getColors().begin(), aSrcMesh->getColors().end(), amesh->getColors().begin());
        }
        if( aSrcMesh->getNumTexCoords() != amesh->getNumTexCoords() ) {
            std::copy( aSrcMesh->getTexCoords().begin(), aSrcMesh->getTexCoords().end(), amesh->getTexCoords().begin());
        }
        if( aSrcMesh->getNumIndices() != amesh->getNumIndices() ) {
            std::copy( aSrcMesh->getIndices().begin(), aSrcMesh->getIndices().end(), amesh->getIndices().begin());
        }
        
        if( aSrcMesh->getNumNormals() != amesh->getNumNormals() && amesh->usingNormals() ) {
            std::copy( aSrcMesh->getNormals().begin(), aSrcMesh->getNormals().end(), amesh->getNormals().begin());
        }
        
        if( aSrcMesh->getNumVertices() != amesh->getNumVertices() ) {
            std::copy( aSrcMesh->getVertices().begin(), aSrcMesh->getVertices().end(), amesh->getVertices().begin());
        }
    }
}

//--------------------------------------------------------------
void Mesh::_updateMeshFromKeyMesh( ofMesh* aSrcMesh, ofMesh* amesh ) {
    if(aSrcMesh == nullptr || amesh == nullptr) return;
    _configureMeshFromSrcMesh( aSrcMesh, amesh );
    
    if( aSrcMesh->getNumNormals() > 0 && amesh->usingNormals() ) {
        std::copy( aSrcMesh->getNormals().begin(), aSrcMesh->getNormals().end(), amesh->getNormals().begin());
    }
    
    std::copy( aSrcMesh->getVertices().begin(), aSrcMesh->getVertices().end(), amesh->getVertices().begin());
}

//--------------------------------------------------------------
void Mesh::updateMeshFromKeyframes( ofMesh* amesh, int aAnimIndex, signed long aMillis ) {
    if(amesh == nullptr) return;
    
    auto& meshKey = getMeshAnimKey( aAnimIndex,aMillis );
    if( meshKey.mesh.getNumVertices() < 3 ) return;
    
    _updateMeshFromKeyMesh( &meshKey.mesh, amesh );
}

//--------------------------------------------------------------
void Mesh::updateMeshFromKeyframesBlended( ofMesh* amesh, int aAnimIndex, signed long aMillis ) {
    if(amesh == nullptr) return;
    
    bool bUpdatedMesh = false;
    MeshAnimKeyCollection& tcollection = mMeshKeyCollections[aAnimIndex];
    auto& keys = tcollection.meshKeys;
    size_t numkeys = keys.size();
    for( int i = 0; i < numkeys; i++) {
        if(keys[i].millis == aMillis) {
            _updateMeshFromKeyMesh( &keys[i].mesh, amesh );
            bUpdatedMesh = true;
        } else if(keys[i].millis > aMillis) {
            if(i > 0) {
                _configureMeshFromSrcMesh( &keys[i].mesh, amesh );
                bUpdatedMesh = true;
                
                signed long delta = aMillis - keys[i-1].millis;
                float pct = double(delta) / double(keys[i].millis - keys[i-1].millis);
                // should we lerp all of the mesh vertices //
                //                return keys[i];
                //                srcMesh = &keys[i].mesh;
                //return ofLerp(keys[i-1].value,keys[i].value,pct);
                ofMesh& mesh1 = keys[i-1].mesh;
                ofMesh& mesh2 = keys[i].mesh;
                
                // mix the vertices //
                auto& tverts = amesh->getVertices();
                auto& srcVerts1 = mesh1.getVertices();
                auto& srcVerts2 = mesh2.getVertices();
                
                bool bUpdateNormals = amesh->usingNormals() && amesh->getNumNormals() > 0 && (amesh->getNumVertices() == amesh->getNumNormals());
                size_t tnumvs = tverts.size();
                pct = ofClamp(pct, 0.0, 1.0);
                float invpct = 1.0 - pct;
                
                for( int i = 0; i < tnumvs; i++ ) {
                    tverts[i] = srcVerts1[i] * invpct + srcVerts2[i] * pct;
                }
                
                // calling getNormals flags them as dirty, so lets run through a separate loop //
                if(bUpdateNormals) {
                    auto& tnormals = amesh->getNormals();
                    auto& srcNormals1 = mesh1.getNormals();
                    auto& srcNormals2 = mesh2.getNormals();
                    for( int i = 0; i < tnumvs; i++ ) {
                        tnormals[i] = srcNormals1[i] * invpct + srcNormals2[i] * pct;
                    }
                }
                
            } else {
                _updateMeshFromKeyMesh( &keys[0].mesh, amesh );
                bUpdatedMesh = true;
            }
        }
    }
    
    if(!keys.empty() && !bUpdatedMesh) {
        _updateMeshFromKeyMesh( &keys.back().mesh, amesh );
    }
    
    
}

//--------------------------------------------------------------
void Mesh::updateMeshFromKeyframes( ofMesh* amesh, int aAnimIndex1, signed long aAnim1Millis, int aAnimIndex2, signed long aAnim2Millis, float aMixPct ) {
    if(amesh == nullptr) return;
    auto& key1 = getMeshAnimKey( aAnimIndex1, aAnim1Millis );
    auto& key2 = getMeshAnimKey( aAnimIndex2, aAnim2Millis );
    if( key1.mesh.getNumVertices() < 3 || key2.mesh.getNumVertices() < 3 ) {
        ofLogError("ofxFBXSource::Mesh::updateMeshFromKeyframes: key1.mesh.getNumVertices(): ") << key1.mesh.getNumVertices() << " key2: " << key2.mesh.getNumVertices();
        return;
    };
    
    // set the verts based on the first animation //
    updateMeshFromKeyframes( amesh, aAnimIndex1, aAnim1Millis );
    
    // mix the vertices //
    auto& tverts = amesh->getVertices();
    auto& srcVerts = key2.mesh.getVertices();
    bool bUpdateNormals = amesh->usingNormals() && amesh->getNumNormals() > 0 && (amesh->getNumVertices() == amesh->getNumNormals());
    size_t tnumvs = tverts.size();
    aMixPct = ofClamp(aMixPct, 0.0, 1.0);
    float invpct = 1.0 - aMixPct;
    
    for( int i = 0; i < tnumvs; i++ ) {
        tverts[i] = tverts[i] * invpct + srcVerts[i] * aMixPct;
    }
    // calling getNormals flags them as dirty, so lets run through a separate loop //
    if(bUpdateNormals) {
        auto& tnormals = amesh->getNormals();
        auto& srcNormals = key2.mesh.getNormals();
        for( int i = 0; i < tnumvs; i++ ) {
            tnormals[i] = tnormals[i] * invpct + srcNormals[i] * aMixPct;
        }
    }
    
}

//--------------------------------------------------------------
void Mesh::updateMesh( ofMesh* aMesh, FbxTime& pTime, FbxAnimLayer * pAnimLayer, FbxPose* pPose ) {
    const bool lHasShape    = fbxMesh->GetShapeCount() > 0;
    const bool lHasSkin     = fbxMesh->GetDeformerCount(FbxDeformer::eSkin) > 0;
    const bool lHasVertexCache = fbxMesh->GetDeformerCount(FbxDeformer::eVertexCache);
    const bool lHasDeformation = lHasShape || lHasSkin;
    
    const int lVertexCount = fbxMesh->GetControlPointsCount();
    
    //    cout << "ofxFBXMesh :: " << getName() << " vertices: " << lVertexCount << " has blend shape: " << lHasShape << " has skin: " << lHasSkin << " has def: " << lHasDeformation << " has vertex cache: " << lHasVertexCache << " | " << ofGetFrameNum() << endl;
    
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
    
    
    vector< glm::vec3 >& amverts      = aMesh->getVertices();
    vector< glm::vec3 >& amnormals    = aMesh->getNormals();
    
    bool bUpdateNormals = lNormalArray != NULL && amnormals.size() > 1;
    
    const int controlPointCount = fbxMesh->GetControlPointsCount();
    // update the vertices //
    for(int i = 0; i < controlPointCount; i++ ) {
        amverts[i] = glm::vec3( lVertexArray[i][0], lVertexArray[i][1], lVertexArray[i][2] );
    }
    
    if( bUpdateNormals ) {
        if( mNormalMappingMode == FbxGeometryElement::eByControlPoint ) {
            for(int i = 0; i < controlPointCount; i++ ) {
                amnormals[i] = glm::vec3( lNormalArray[i][0], lNormalArray[i][1], lNormalArray[i][2] );
                //                amnormals[i].normalize();
            }
        } else if( mNormalMappingMode == FbxGeometryElement::eByPolygonVertex ) {
            const int lPolygonCount = fbxMesh->GetPolygonCount();
            int polysize = 3;
            
            for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex) {
                polysize = fbxMesh->GetPolygonSize( lPolygonIndex );
                for (int lVerticeIndex = 0; lVerticeIndex < polysize; ++lVerticeIndex) {
                    
                    const int lControlPointIndex = fbxMesh->GetPolygonVertex( lPolygonIndex, lVerticeIndex );
                    //                    amverts[lControlPointIndex].set(lVertexArray[lControlPointIndex][0],
                    //                                                    lVertexArray[lControlPointIndex][1],
                    //                                                    lVertexArray[lControlPointIndex][2]);
                    //                    if( bUpdateNormals ) {
                    amnormals[lControlPointIndex] = glm::vec3(lNormalArray[ lControlPointIndex ][0],
                                                              lNormalArray[ lControlPointIndex ][1],
                                                              lNormalArray[ lControlPointIndex ][2] );
                    //                        amnormals[lControlPointIndex].normalize();
                    //                    }
                }
            }
        }
    }
    
    //    if(bAllMappedByControlPoint) {
    ////        cout << "updateMesh :: bAllMappedByControlPoint : " << endl;
    //        for(int i = 0; i < fbxMesh->GetControlPointsCount(); i++ ) {
    //            amverts[i].set( lVertexArray[i][0], lVertexArray[i][1], lVertexArray[i][2] );
    //            if( bUpdateNormals ) {
    //                amnormals[i].set( lNormalArray[i][0], lNormalArray[i][1], lNormalArray[i][2] );
    //                amnormals[i].normalize();
    //            }
    //        }
    //    } else {
    ////        cout << "updateMesh :: !bAllMappedByControlPoint : normals: " << bUpdateNormals << endl;
    //        const int lPolygonCount = fbxMesh->GetPolygonCount();
    //        int tvertcount = 0;
    //        for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex) {
    //            for (int lVerticeIndex = 0; lVerticeIndex < 3; ++lVerticeIndex) {
    //
    //                const int lControlPointIndex = fbxMesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex);
    //                amverts[lControlPointIndex].set(lVertexArray[lControlPointIndex][0],
    //                                        lVertexArray[lControlPointIndex][1],
    //                                        lVertexArray[lControlPointIndex][2]);
    //                if( bUpdateNormals ) {
    //                    amnormals[lControlPointIndex].set(lNormalArray[ lControlPointIndex ][0],
    //                                              lNormalArray[ lControlPointIndex ][1],
    //                                              lNormalArray[ lControlPointIndex ][2] );
    //                    amnormals[lControlPointIndex].normalize();
    //                }
    ////                ++tvertcount;
    //
    //                //  WORKING ////////////
    ////                const int lControlPointIndex = fbxMesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex);
    ////                amverts[tvertcount].set(lVertexArray[lControlPointIndex][0],
    ////                                        lVertexArray[lControlPointIndex][1],
    ////                                        lVertexArray[lControlPointIndex][2]);
    ////                if( bUpdateNormals ) {
    ////                    amnormals[tvertcount].set(lNormalArray[ lControlPointIndex ][0],
    ////                                              lNormalArray[ lControlPointIndex ][1],
    ////                                              lNormalArray[ lControlPointIndex ][2] );
    ////                    amnormals[tvertcount].normalize();
    ////                }
    ////                ++tvertcount;
    //                // !-- WORKING ///////////////
    //            }
    //        }
    //    }
    
    delete [] lVertexArray;
    if( lNormalArray != NULL ) {
        delete [] lNormalArray;
    }
    
}

#pragma mark - Draw
//--------------------------------------------------------------
void Mesh::draw( ofMesh* aMesh ) {
    //    cout << "ofxFBXMesh :: " << getName() << " verts: " << aMesh->haveVertsChanged() << " indices: " << aMesh->haveIndicesChanged() << " normals: " << aMesh->haveNormalsChanged() << " tex coords: " << aMesh->haveTexCoordsChanged() << " colors: " << aMesh->haveColorsChanged() << " | " << ofGetFrameNum() << endl;
    veebs.updateMesh( *aMesh );
    if( aMesh->getNumColors() < 1 || !aMesh->usingColors() ) {
        veebs.disableColors();
    } else {
        veebs.enableColors();
    }
    
    const int lSubMeshCount = subMeshes.size();
    glEnable( GL_NORMALIZE );
    
    if(veebs.getIsAllocated()) {
        if(lSubMeshCount > 0) {
#ifndef TARGET_OPENGLES
            // GLES does not support glPolygonMode
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );
#endif
            
            veebs.bind();
        }
        for (int lIndex = 0; lIndex < lSubMeshCount; ++lIndex) {
            GLsizei lOffset = subMeshes[lIndex].indexOffset * sizeof(unsigned int);
            //            const GLsizei lElementCount = meshMaterials[lIndex].triangleCount * 3;
            const GLsizei lElementCount = subMeshes[lIndex].totalIndices;
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, veebs.getIndexId() );
            //            cout << "lElementCount = " << lElementCount << " mesh is using indices = " << mesh.hasIndices() << endl;
            
#ifdef TARGET_OPENGLES
            glDrawElements(GL_TRIANGLES, lElementCount, GL_UNSIGNED_SHORT, reinterpret_cast<const GLvoid *>(lOffset));
#else
            glDrawElements( GL_TRIANGLES, lElementCount, GL_UNSIGNED_INT, reinterpret_cast<const GLvoid *>(lOffset));
#endif
            
        }
        if(lSubMeshCount > 0) veebs.unbind();
    }
}

//--------------------------------------------------------------
void Mesh::draw( ofMesh* aMesh, vector< shared_ptr<ofxFBXMeshMaterial> >& aMats, bool bUpdateTheVeeeeebs ) {
    if( aMats.size() == 0 ) {
        draw( aMesh );
        return;
    }
    
//    cout << "ofxFBXSrcMesh :: aMesh num verts: " << aMesh->getNumVertices() << " num tcoords: " << aMesh->getNumTexCoords() << " update veebs: " << bUpdateTheVeeeeebs << " veebs num verts: " << veebs.getNumVertices() << " | " << ofGetFrameNum() << endl;
//        cout << "ofxFBXMesh :: " << getName() << " verts: " << aMesh->haveVertsChanged() << " indices: " << aMesh->haveIndicesChanged() << " normals: " << aMesh->haveNormalsChanged() << " tex coords: " << aMesh->haveTexCoordsChanged() << " colors: " << aMesh->haveColorsChanged() << " | " << ofGetFrameNum() << endl;
//    veebs.updateMesh( *aMesh );
    
    
    if(bUpdateTheVeeeeebs || veebs.getNumVertices() < 1) {
        if( aMesh->hasVertices() ) {
            veebs.updateVertexData( aMesh->getVerticesPointer(), aMesh->getNumVertices() );
        }
        
        if( aMesh->hasColors() && aMesh->usingColors() ) {
            veebs.updateColorData( aMesh->getColorsPointer(), aMesh->getNumColors() );
        }
        
        if( aMesh->hasNormals() && aMesh->usingNormals() ) {
            veebs.updateNormalData(aMesh->getNormalsPointer(),aMesh->getNumNormals());
        }
        
        if( aMesh->hasTexCoords() && aMesh->usingTextures() ) {
            veebs.updateTexCoordData( aMesh->getTexCoordsPointer(), aMesh->getNumTexCoords() );
        }
    }
    
//    if(haveIndicesChanged()){
//        if(getNumIndices()==0){
//            vbo.clearIndices();
//            vboNumIndices = getNumIndices();
//        }else if(vboNumIndices<getNumIndices()){
//            vbo.setIndexData(getIndexPointer(),getNumIndices(),usage);
//            vboNumIndices = getNumIndices();
//        }else{
//            vbo.updateIndexData(getIndexPointer(),getNumIndices());
//        }
//    }
    
    
    if( aMesh->getNumColors() < 1 || !aMesh->usingColors() ) {
        veebs.disableColors();
    } else {
        veebs.enableColors();
    }
    
    const int lSubMeshCount = subMeshes.size();
    if( veebs.getIsAllocated() && lSubMeshCount > 0 ) {
//        glEnable( GL_NORMALIZE ); // <-- dont think we need this anymore //
        
//        #ifndef TARGET_OPENGLES
////          // GLES does not support glPolygonMode
//            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );
//        #endif
        
        // check to see if all have materials and are enabled so that OF can set the attributes //
        // the setAttributes in Programmable renderer is a private method, so the vbo has to bind and unbind
        // per sub mesh so we can call that method via ofVbo.drawElements :(
        bool bAllMaterialsEnabled = true;
        int tnumMatsEnabled = 0;
        for( int i = 0; i < aMats.size(); i++ ) {
            if( !aMats[i]->isEnabled() ) {
                bAllMaterialsEnabled = false;
            } else {
                tnumMatsEnabled++;
            }
        }
        if( tnumMatsEnabled < 1 ) {
            // just draw the whole thing //
            veebs.drawElements( GL_TRIANGLES, veebs.getNumIndices() );
        } else {
        
            if(bAllMaterialsEnabled) {
                veebs.bind();
            }
            
            for( int lIndex = 0; lIndex < lSubMeshCount; ++lIndex ) {
                if( lIndex < aMats.size() ) {
                    aMats[lIndex]->begin();
                    
                    const GLsizei lElementCount = subMeshes[lIndex].totalIndices;
                    
                    if( bAllMaterialsEnabled ) {
//                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, veebs.getIndexId() );
        //            cout << "lElementCount = " << lElementCount << " mesh is using indices = " << mesh.hasIndices() << endl;

                        #ifdef TARGET_OPENGLES
                            glDrawElements( GL_TRIANGLES, lElementCount, GL_UNSIGNED_SHORT, (void*)(sizeof(ofIndexType) * subMeshes[lIndex].indexOffset));
                        #else
                            GLsizei lOffset = subMeshes[lIndex].indexOffset * sizeof(unsigned int);
                            glDrawElements( GL_TRIANGLES, lElementCount, GL_UNSIGNED_INT, reinterpret_cast<const GLvoid *>(lOffset));
                        #endif
                    } else {
                        veebs.drawElements( GL_TRIANGLES, lElementCount, subMeshes[lIndex].indexOffset );
                    }

                    aMats[lIndex]->end();
                }
            }
            
            if(bAllMaterialsEnabled) veebs.unbind();
        }
    }
}

//--------------------------------------------------------------
void Mesh::drawMeshKeyframe( int aAnimIndex, signed long aMillis, vector< shared_ptr<ofxFBXMeshMaterial> >& aMats, bool bUpdateTheVeeeeebs ) {
    auto& meshKey = getMeshAnimKey( aAnimIndex,aMillis );
    if( meshKey.mesh.getNumVertices() < 3 ) return;
    draw( &meshKey.mesh, aMats, bUpdateTheVeeeeebs );
}

//--------------------------------------------------------------
int Mesh::getNumSubMeshes() {
    return (int)subMeshes.size();
}

//--------------------------------------------------------------
bool Mesh::hasClusterDeformation() {
    if(fbxMesh == NULL) {
        ofLogWarning("ofxFBSMesh :: Set the Node before calling hasClusterDeformation");
        return false;
    }
    return fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
}

#pragma mark - Materials + Textures
//--------------------------------------------------------------
int Mesh::getNumMaterials() {
    return getMaterials().size();
}

//--------------------------------------------------------------
vector< ofxFBXSource::MeshMaterial* > Mesh::getMaterials() {
    vector< ofxFBXSource::MeshMaterial* > rMaterials;
    int lSubMeshCount = subMeshes.size();
    for (int lIndex = 0; lIndex < lSubMeshCount; ++lIndex) {
        const FbxSurfaceMaterial * lMaterial = fbxMesh->GetNode()->GetMaterial(lIndex);
        ofxFBXSource::MeshMaterial* lMaterialCache = NULL;
        if(lMaterial && lMaterial->GetUserDataPtr() ) {
            lMaterialCache = static_cast<ofxFBXSource::MeshMaterial *>(lMaterial->GetUserDataPtr());
            rMaterials.push_back( lMaterialCache );
        }
        
    }
    return rMaterials;
}

//--------------------------------------------------------------
bool Mesh::hasTexture() {
    return ( getTextures().size() > 0 );
}

//--------------------------------------------------------------
vector< shared_ptr<ofxFBXSource::MeshTexture> > Mesh::getTextures() {
    auto tmats = getMaterials();
    vector< shared_ptr<ofxFBXSource::MeshTexture> > ttexs;
    if( tmats.size() == 0 ) return ttexs;
    for( auto& tmat : tmats ) {
        if( tmat && tmat->hasTexture() ) {
            ttexs.push_back( tmat->getTexturePtr() );
        }
    }
    return ttexs;
}

//--------------------------------------------------------------
ofVbo& Mesh::getVbo() {
    return veebs;
}

//--------------------------------------------------------------
ofMesh& Mesh::getOFMesh() {
    return mesh;
}

#pragma mark - Keyframes
//----------------------------------------
ofxFBXSource::MeshAnimKeyCollection& Mesh::getMeshKeyCollection( int aAnimIndex ) {
    if( mMeshKeyCollections.count(aAnimIndex) < 1 ) {
        MeshAnimKeyCollection temp;
        mMeshKeyCollections[ aAnimIndex ] = temp;
    }
    return mMeshKeyCollections[aAnimIndex];
}

//----------------------------------------
bool Mesh::hasMeshKeyCollection( int aAnimIndex ) {
    if( mMeshKeyCollections.count(aAnimIndex) < 1 ) return false;
    return true;
}

//----------------------------------------
void Mesh::clearKeyFrames() {
    mMeshKeyCollections.clear();
    ofxFBXSource::Node::clearKeyFrames();
    setUsingCachedMeshes( false );
}

//----------------------------------------
void Mesh::setUsingCachedMeshes( bool ab ) {
    bUsingCachedMeshes = ab;
}

//----------------------------------------
bool Mesh::isUsingCachedMeshes() {
    return bUsingCachedMeshes;
}

//----------------------------------------
MeshAnimKey& Mesh::getMeshAnimKey( int aAnimIndex, signed long aMillis ) {
    MeshAnimKeyCollection& tcollection = mMeshKeyCollections[aAnimIndex];
    auto& keys = tcollection.meshKeys;
    size_t numkeys = keys.size();
    for(int i=0; i < numkeys; i++) {
        if(keys[i].millis == aMillis) {
            return keys[i];
        } else if(keys[i].millis > aMillis) {
            if(i > 0){
//                signed long delta = aMillis - keys[i-1].millis;
//                float pct = double(delta) / double(keys[i].millis - keys[i-1].millis);
                // should we lerp all of the mesh vertices //
                return keys[i];
//                srcMesh = &keys[i].mesh;
                //return ofLerp(keys[i-1].value,keys[i].value,pct);
            } else {
                return keys[0];
            }
        }
    }
    
    if(!keys.empty()){
        return keys.back();
    }
    return dummyMeshAnimKey;
}

#pragma mark - Deformations
//--------------------------------------------------------------
void Mesh::computeBlendShapes( ofMesh* aMesh, FbxTime& pTime, FbxAnimLayer * pAnimLayer ) {
    int lBlendShapeDeformerCount = fbxMesh->GetDeformerCount(FbxDeformer::eBlendShape);
    //    cout << "Computing blendshapes for " << getName() << endl;
    for(int lBlendShapeIndex = 0; lBlendShapeIndex<lBlendShapeDeformerCount; ++lBlendShapeIndex) {
        FbxBlendShape* lBlendShape = (FbxBlendShape*)fbxMesh->GetDeformer(lBlendShapeIndex, FbxDeformer::eBlendShape);
        
        int lBlendShapeChannelCount = lBlendShape->GetBlendShapeChannelCount();
        for(int lChannelIndex = 0; lChannelIndex < lBlendShapeChannelCount; ++lChannelIndex) {
            FbxBlendShapeChannel* lChannel = lBlendShape->GetBlendShapeChannel(lChannelIndex);
            if(lChannel && pAnimLayer) {
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
                        glm::vec3 ov = original.getVertices()[j];
                        glm::vec3 influence = ((fbxToOf(lEndShape->GetControlPoints()[j]) - glm::vec4(ov.x, ov.y, ov.z, 0.0)) * lWeight);
                        aMesh->getVertices()[j] += influence;
                    }
                    
                } else if(lStartShape && lEndShape) {
                    float lStartWeight  = lFullWeights[lStartIndex];
                    float lEndWeight    = lFullWeights[lEndIndex];
                    // Calculate the real weight.
                    lWeight = ofMap(lWeight, lStartWeight, lEndWeight, 0, 1, true);
                    //                    cout << "updateMesh : weight = " << lWeight << " lStartWeight " << lStartWeight << " lEndWeight " << lEndWeight << endl;
                    //                    lWeight = ((lWeight-lStartWeight)/(lEndWeight-lStartWeight)) * 100;
                    for (int j = 0; j < aMesh->getNumVertices(); j++) {
                        // Add the influence of the shape vertex to the mesh vertex.
                        glm::vec3 influence = (fbxToOf(lEndShape->GetControlPoints()[j] - lStartShape->GetControlPoints()[j] )) * lWeight;
                        aMesh->getVertices()[j] += influence;
                    }
                }
                
            }
        }
    }
}

void Mesh::computeSkinDeformation( FbxAMatrix& pGlobalPosition, FbxTime& pTime, FbxAnimLayer* pAnimLayer, FbxVector4* pVertexArray, FbxVector4* pNormalsArray, FbxPose* pPose ) {
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
void Mesh::computeLinearDeformation(FbxAMatrix& pGlobalPosition,
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
    FbxAMatrix lInfluence;// = lVertexTransformMatrix;
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
                //                FbxAMatrix lInfluence = lVertexTransformMatrix;
                lInfluence = lVertexTransformMatrix;
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
    FbxVector4 lSrcVertex;
    for (int i = 0; i < lVertexCount; i++) {
        lSrcVertex   = pVertexArray[i];
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
void Mesh::computeDualQuaternionDeformation(FbxAMatrix& pGlobalPosition,
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
void Mesh::computeClusterDeformation(FbxAMatrix& pGlobalPosition,
                                           FbxMesh* pMesh,
                                           FbxCluster* pCluster,
                                           FbxAMatrix& pVertexTransformMatrix,
                                           FbxTime pTime,
                                           FbxPose* pPose, bool bNormal ) {
    
    FbxCluster::ELinkMode lClusterMode = pCluster->GetLinkMode();
    
    ofxFBXSource::Bone* bone        = NULL;
    ofxFBXSource::Cluster* cluster  = NULL;
    FbxNode* boneNode       = pCluster->GetLink();
    
    if(boneNode) {
        if(boneNode->GetUserDataPtr()) {
            bone = static_cast<ofxFBXSource::Bone *>(boneNode->GetUserDataPtr());
        }
    }
    
    if(bone != NULL) {
        if(pCluster->GetUserDataPtr()) {
            cluster = static_cast<ofxFBXSource::Cluster *>(pCluster->GetUserDataPtr());
            //            cluster->update( pTime, pPose );
        }
    }
    
    if( bone != NULL && cluster != NULL ) {
        //        cout << "computeClusterDeformation: bone != NULL && cluster != NULL " << bone->getName() << endl;
        //        cout << "We have cached cluster and bone! " << bone->getName() << endl;
        //        pVertexTransformMatrix = cluster->preTrans * bone->fbxTransform * cluster->postTrans;
        //        pVertexTransformMatrix = cluster->preTrans * bone->fbxTransform * cluster->postTrans;
        pVertexTransformMatrix = cluster->preTrans * bone->fbxTransform * cluster->postTrans;
        if( bNormal ) {
            //            cout << "We have cached cluster andNORMAL bone! " << bone->getName() << endl;
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
void Mesh::populateNormals( FbxVector4* pNormalsArray ) {
    
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
                //void KFbxMesh::GetPolygonVertexNormal&#40; int pPolyIndex, int pVertexIndex, KFbxVector4 &pNormal&#41; const;
                //void KFbxMesh::GetPolygonVertexNormal&#40; int pPolyIndex, int pVertexIndex, KFbxVector4 &pNormal&#41; const;
                FbxVector4 lNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
                pNormalsArray[ lVertexIndex ] = lNormal;
                // lNormals->GetDirectArray().Release&#40;&#40;void**&#41;&lFbxNormals&#41;
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
