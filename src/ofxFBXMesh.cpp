//
//  ofxFBXMesh.h
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 10/31/13.
//
//

#include "ofxFBXMesh.h"
#include "ofxFBX.h"

ofVbo ofxFBXMesh::dummyVbo;

//----------------------------------------
ofxFBXSource::Node::NodeType ofxFBXMesh::getType() {
    return ofxFBXSource::Node::OFX_FBX_MESH;
}

//--------------------------------------------------------------
void ofxFBXMesh::setup( shared_ptr<ofxFBXSource::Node> anode ) {
    ofxFBXNode::setup( anode );
    _checkSrcMesh();
    bHasTexture = false;
    if(mSrcMesh) {
        mSrcMesh->configureMesh( mesh );
        setTransform( anode );
        // populate the materials //
        auto srcMats = mSrcMesh->getMaterials();
        for( auto sm : srcMats ) {
            auto mat = make_shared<ofxFBXMeshMaterial>();
            mat->setup( sm );
            mMaterials.push_back( mat );
            if( mat->hasTexture() ) {
                bHasTexture = true;
            }
        }
    }
}

//--------------------------------------------------------------
void ofxFBXMesh::update( FbxTime& pTime, FbxPose* pPose ) {
    _checkSrcMesh();
    if(mSrcMesh) {
        mSrcMesh->update( pTime, pPose );
        bDrawMeshKeyframe=false;
    }
}

//--------------------------------------------------------------
void ofxFBXMesh::update( int aAnimIndex, signed long aMillis ) {
    _checkSrcMesh();
    if(mSrcMesh) {
        mSrcMesh->update( aAnimIndex, aMillis );
        
        if( mSrcMesh->isUsingCachedMeshes() && mSrcMesh->hasMeshKeyCollection(aAnimIndex)) {
            // now configure the mesh for the animation index and time //
            // but only if there are no vertices, since the src mesh has all of the meshes already in a vector //
            // the mesh gets blended in transition, so setup here, so there isn't a stutter on transition start //
            if( bBlendMeshFrames ) {
                mSrcMesh->updateMeshFromKeyframesBlended( &mesh, aAnimIndex, aMillis );
            } else {
                if( mesh.getNumVertices() < 1 ) {
                    mSrcMesh->updateMeshFromKeyframes( &mesh, aAnimIndex, aMillis );
                }
            }
            mLastAnimIndex = aAnimIndex;
            mLastFbxTimeMillis = aMillis;
            bDrawMeshKeyframe=true;
        }
    }
}

//--------------------------------------------------------------
void ofxFBXMesh::update( int aAnimIndex1, signed long aAnim1Millis, int aAnimIndex2, signed long aAnim2Millis, float aMixPct ) {
    _checkSrcMesh();
    if(mSrcMesh) {
        mSrcMesh->update( aAnimIndex1, aAnim1Millis, aAnimIndex2, aAnim2Millis, aMixPct );
        bDrawMeshKeyframe=false;
        if( mSrcMesh->isUsingCachedMeshes() && mSrcMesh->hasMeshKeyCollection(aAnimIndex1) && mSrcMesh->hasMeshKeyCollection(aAnimIndex2)) {
            mSrcMesh->updateMeshFromKeyframes( &mesh, aAnimIndex1, aAnim1Millis, aAnimIndex2, aAnim2Millis, aMixPct );
        }
    }
}

//--------------------------------------------------------------
void ofxFBXMesh::update() {
    _checkSrcMesh();
    if(mSrcMesh) {
        setPosition( mSrcMesh->getPosition() );
        setOrientation( mSrcMesh->getOrientationQuat() );
        setScale( mSrcMesh->getScale() );
    }
}

//--------------------------------------------------------------
void ofxFBXMesh::lateUpdate(FbxTime& pTime, FbxAnimLayer * pAnimLayer, FbxPose* pPose) {
    _checkSrcMesh();
    if(mSrcMesh) {
//        signed long ctime = (signed long)pTime.GetMilliSeconds();
//        if(mLastFbxTimeMillis != ctime ) {
        if(!mSrcMesh->isUsingCachedMeshes() ) {
            mSrcMesh->updateMesh( &mesh, pTime, pAnimLayer, pPose );
            bMeshDirty = true;
        } else {
            bMeshDirty = true;
        }
//        mLastFbxTimeMillis = ctime;
    }
}

//--------------------------------------------------------------
void ofxFBXMesh::draw() {
    _checkSrcMesh();
    if(!mSrcMesh) {
        ofLogError("ofxFBXMesh::draw : src mesh is invalid! ") << getName();
        return;
    }
    transformGL(); {
        if(bDrawMeshKeyframe) {
            mSrcMesh->drawMeshKeyframe( mLastAnimIndex, mLastFbxTimeMillis, mMaterials, bMeshDirty );
        } else {
            mSrcMesh->draw( &mesh, mMaterials, bMeshDirty );
        }
    } restoreTransformGL();
    
    bMeshDirty = false;
}

//--------------------------------------------------------------
void ofxFBXMesh::drawWireframe() {
    _checkSrcMesh();
    transformGL(); {
        mesh.drawWireframe();
    } restoreTransformGL();
}

//--------------------------------------------------------------
void ofxFBXMesh::drawNormals(float length, bool bFaceNormals ) {
    
    if( mesh.usingNormals()) {
        vector<glm::vec3>& normals    = mesh.getNormals();
        vector<glm::vec3>& vertices   = mesh.getVertices();
        glm::vec3 normal;
        glm::vec3 vert;
        
        // super inefficient, for debug only //
//        ofMesh normalsMesh;
        if( normalsMesh.getNumVertices() != normals.size() * 2 ) {
            normalsMesh.getVertices().resize( normals.size() * 2 );
            normalsMesh.setMode( OF_PRIMITIVE_LINES );
        }
        
//        normalsMesh.getVertices().resize( normals.size() * 2);
        
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
                normal = glm::normalize(normals[i]);
                normal *= length;
                normalsMesh.setVertex(i*2+1, normal+vert);
            }
        } else {
            for(int i = 0; i < (int)normals.size(); i++) {
                vert = vertices[i];
                normal = glm::normalize(normals[i]);
                normalsMesh.setVertex( i*2, vert);
                normal *= length;
                normalsMesh.setVertex(i*2+1, normal+vert);
            }
        }
        transformGL(); {
            normalsMesh.draw();
        } restoreTransformGL();
    } else {
        ofLogWarning("ofxFBXMesh") << "drawNormals(): mesh normals are disabled for " << getName();
    }
}

//--------------------------------------------------------------
ofVbo& ofxFBXMesh::getVbo() {
    _checkSrcMesh();
    if( mSrcMesh ) {
        return mSrcMesh->getVbo();
    }
    ofLogWarning( "ofxFBXMesh::getVbo src node is not set!!" );
    return dummyVbo;
}

//--------------------------------------------------------------
ofMesh& ofxFBXMesh::getMesh() {
    return mesh;
}

//--------------------------------------------------------------
int ofxFBXMesh::getNumMaterials() {
    return mMaterials.size();
}

//--------------------------------------------------------------
vector< shared_ptr<ofxFBXMeshMaterial> > ofxFBXMesh::getMaterials() {
    return mMaterials;
}

//--------------------------------------------------------------
void ofxFBXMesh::setMaterialsEnabled(bool ab) {
    for( auto mat : mMaterials ) {
        if( ab ) {
            mat->enable();
        } else {
            mat->disable();
        }
    }
}

//--------------------------------------------------------------
vector< shared_ptr<ofxFBXSource::MeshTexture> > ofxFBXMesh::getTextures() {
    vector< shared_ptr<ofxFBXSource::MeshTexture> > ttexs;
    for( auto mat : mMaterials ) {
        if( mat->hasSourceTexture() ) {
            ttexs.push_back( mat->getSrcTexture() );
        }
    }
    return ttexs;
}

//--------------------------------------------------------------
bool ofxFBXMesh::hasTexture() {
    return bHasTexture;
//    for( auto mat : mMaterials ) {
//        if( mat->hasTexture() ) {
//            return true;
//        }
//    }
//    return false;
}

//--------------------------------------------------------------
ofMesh ofxFBXMesh::getGlobalMesh() {
    glm::mat4 gmat = getGlobalTransformMatrix();
    // transform points into global space //
    ofMesh tmesh = mesh;
    auto& tverts = tmesh.getVertices();
    
    for( int i = 0; i < tverts.size(); i++ ) {
        auto& mp = tverts[i];
        mp = gmat * glm::vec4(mp, 1.0);
    }
    
    glm::quat tq = getGlobalOrientation();
    // transform the normals from local to global space //
    auto& tnormals = tmesh.getNormals();
    for( auto& tn : tnormals ) {
        tn = tq * tn;
    }
    
    return tmesh;
}

//--------------------------------------------------------------
ofMesh ofxFBXMesh::getGlobalMeshAroundPosition() {
    glm::vec3 gpos = getGlobalPosition();
    auto tmesh = getGlobalMesh();
    for( auto& tv : tmesh.getVertices() ) {
        tv -= gpos;
    }
    return tmesh;
}

//--------------------------------------------------------------
ofMesh ofxFBXMesh::getMeshAroundPositionScaleApplied() {
    glm::vec3 gscale = getGlobalScale();
    glm::vec3 gpos = getGlobalPosition();
    // transform points into global space //
    ofMesh tmesh = mesh;
    auto& tverts = tmesh.getVertices();
    
    for( int i = 0; i < tverts.size(); i++ ) {
        auto& mp = tverts[i];// - gpos;
        mp -= gpos;
        mp = gscale * mp;//glm::vec4(mp, 1.0);
    }
    return tmesh;
}

#pragma mark private
//--------------------------------------------------------------
void ofxFBXMesh::_checkSrcMesh() {
    if( !mSrcMesh && mSrcNode && mSrcNode->getType() == ofxFBXSource::Node::OFX_FBX_MESH ) {
        mSrcMesh = dynamic_pointer_cast<ofxFBXSource::Mesh>(mSrcNode);
    }
}
