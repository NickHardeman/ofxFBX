//
//  ofxFBXMeshMaterial.cpp
//  example-BoneControl
//
//  Created by Nick Hardeman on 7/15/19.
//

#include "ofxFBXMeshMaterial.h"

//--------------------------------------------------------------
void ofxFBXMeshMaterial::setup( ofxFBXSource::MeshMaterial* aSrcMat ) {
    setEmissiveColor( aSrcMat->getEmissiveColor() );
    setAmbientColor( aSrcMat->getAmbientColor() );
    setDiffuseColor( aSrcMat->getDiffuseColor() );
    setSpecularColor( aSrcMat->getSpecularColor() );
    setShininess( aSrcMat->getShininess() );
    _name = aSrcMat->getName();
    
    if( aSrcMat->hasTexture() ) {
        mSrcTexture = aSrcMat->getTexturePtr();
    }
}

//--------------------------------------------------------------
void ofxFBXMeshMaterial::setTexture( shared_ptr<ofTexture> aUserTex ) {
    mUserTex = aUserTex;
}

//--------------------------------------------------------------
void ofxFBXMeshMaterial::begin() {
    if( !isEnabled() ) return;
    if( areMaterialsEnabled() ) ofMaterial::begin();
    if( hasTexture() && areTexturesEnabled() ) {
//        cout << "ofxFBXMeshMaterial :: " << getName() << " binding the texture " << areMaterialsEnabled() << endl;
//        glEnable( GL_TEXTURE_2D );
//        glBindTexture(GL_TEXTURE_2D, texture->getTextureData().textureID);
//        mSrcTexture->bind();
        if( mUserTex && mUserTex->isAllocated() ) mUserTex->bind();
        else if( mSrcTexture && mSrcTexture->isAllocated() ) mSrcTexture->bind();
    }
}

//--------------------------------------------------------------
void ofxFBXMeshMaterial::end() {
    if( !isEnabled() ) return;
    if( areMaterialsEnabled() ) ofMaterial::end();
    if( hasTexture() && areTexturesEnabled() ) {
        if( mUserTex && mUserTex->isAllocated() ) mUserTex->unbind();
        else if( mSrcTexture && mSrcTexture->isAllocated() ) mSrcTexture->unbind();
//        mSrcTexture->unbind();
//        glBindTexture( GL_TEXTURE_2D, 0);
//        glDisable( GL_TEXTURE_2D );
    }
}

//--------------------------------------------------------------
bool ofxFBXMeshMaterial::hasTexture() {
//    return mSrcTexture != NULL;
    if( mUserTex ) return true;
    if( mSrcTexture ) return true;
    return false;//texture;// != NULL;
}

//--------------------------------------------------------------
bool ofxFBXMeshMaterial::hasSourceTexture() {
    if( mSrcTexture ) return true;
    return false;
}

//--------------------------------------------------------------
bool ofxFBXMeshMaterial::hasUserTexture() {
    if( mUserTex ) return true;
    return false;
}

//--------------------------------------------------------------
shared_ptr<ofTexture> ofxFBXMeshMaterial::getUserTexture() {
    return mUserTex;
}

//--------------------------------------------------------------
shared_ptr<ofxFBXSource::MeshTexture> ofxFBXMeshMaterial::getSrcTexture() {
    return mSrcTexture;
}

//--------------------------------------------------------------
void ofxFBXMeshMaterial::enableTextures() {
    _bTexturesEnabled = true;
}

//--------------------------------------------------------------
void ofxFBXMeshMaterial::disableTextures() {
    _bTexturesEnabled = false;
}

//--------------------------------------------------------------
bool ofxFBXMeshMaterial::areTexturesEnabled() {
    return _bTexturesEnabled;
}

//--------------------------------------------------------------
void ofxFBXMeshMaterial::enableMaterials() {
    _bMaterialsEnabled = true;
}

//--------------------------------------------------------------
void ofxFBXMeshMaterial::disableMaterials() {
    _bMaterialsEnabled = false;
}

//--------------------------------------------------------------
bool ofxFBXMeshMaterial::areMaterialsEnabled() {
    return _bMaterialsEnabled;
}

//--------------------------------------------------------------
void ofxFBXMeshMaterial::enable() {
    _bEnabled = true;
}

//--------------------------------------------------------------
void ofxFBXMeshMaterial::disable() {
    _bEnabled = false;
}

//--------------------------------------------------------------
bool ofxFBXMeshMaterial::isEnabled() {
    return _bEnabled;
}

//--------------------------------------------------------------
string ofxFBXMeshMaterial::getName() {
    return _name;
}

//--------------------------------------------------------------
string ofxFBXMeshMaterial::getInfoAsString() {
    stringstream ss;
    ss << "-- " << getName() << " Material enabled: " << isEnabled() << " material enabled: " << areMaterialsEnabled() << " ----------------------" << endl;
    ss << "emissive = " << getEmissiveColor() << endl;
    ss << "ambient = " << getAmbientColor() << endl;
    ss << "diffuse = " << getDiffuseColor() << endl;
    ss << "specular = " << getSpecularColor() << endl;
    ss << "shininess = " << getShininess() << endl;
    // lets determine if there is a pointer to the src texture //
//    ss << "texture = " << _textureName << " enabled: " << areTexturesEnabled() << " has texture: " << hasTexture() << endl;
    
    return ss.str();
}
