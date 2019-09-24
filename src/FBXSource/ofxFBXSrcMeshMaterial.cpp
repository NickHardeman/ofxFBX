//
//  ofxFBXMeshMaterial.cpp
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 10/31/13.
//
//

#include "ofxFBXSrcMeshMaterial.h"

using namespace ofxFBXSource;

//--------------------------------------------------------------
MeshMaterial::MeshMaterial() {
//    texture = NULL;
    _name   = "default material";
    enableTextures();
    enableMaterials();
    enable();
}

//--------------------------------------------------------------
void MeshMaterial::setup( const FbxSurfaceMaterial * pMaterial ) {
    ofFloatColor tEmissive = getMaterialProperty( pMaterial, FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor );
    ofFloatColor tAmbient = getMaterialProperty( pMaterial, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor );
    ofFloatColor tDiffuse = getMaterialProperty( pMaterial, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor );
    ofFloatColor tSpecular = getMaterialProperty( pMaterial, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor );
    float shininess = 0;
    FbxProperty lShininessProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sShininess);
    if (lShininessProperty.IsValid()) {
        double lShininess = lShininessProperty.Get<FbxFloat>();
        shininess = lShininess;
    }
    
    setEmissiveColor( tEmissive );
    setAmbientColor( tAmbient );
    setDiffuseColor( tDiffuse );
    setSpecularColor( tSpecular );
    setShininess( shininess );
    
    _name = pMaterial->GetName();
    
//    cout << "-- " << pMaterial->GetName() << " -- setup -------------------------" << endl;
//    cout << "emissive = " << tEmissive << endl;
//    cout << "ambient = " << tAmbient << endl;
//    cout << "diffuse = " << tDiffuse << endl;
//    cout << "specular = " << tSpecular << endl;
//    cout << "shininess = " << shininess << endl;
    
    const int lTextureCount = pMaterial->GetSrcObjectCount<FbxFileTexture>();
    ofLogVerbose("ofxFBXMeshMaterial::setup") << pMaterial->GetName() << " texture count = " << lTextureCount;
    
    // pMaterial->GetClassId().Is(KFbxSurfacePhong::ClassId)
    
    // search for a texture based on the material properties :)
	if (!hasTexture()) {
		findTextureForProperty(pMaterial, FbxSurfaceMaterial::sShadingModel);
	}
    if(!hasTexture()) {
        findTextureForProperty( pMaterial, FbxSurfaceMaterial::sEmissive );
    }
    if(!hasTexture()) {
        findTextureForProperty( pMaterial, FbxSurfaceMaterial::sAmbient );
    }
    if(!hasTexture()) {
        findTextureForProperty( pMaterial, FbxSurfaceMaterial::sDiffuse );
    }
    if(!hasTexture()) {
        findTextureForProperty( pMaterial, FbxSurfaceMaterial::sSpecular );
    }
    if(!hasTexture()) {
        findTextureForProperty( pMaterial, FbxSurfaceMaterial::sNormalMap );
    }
    if(!hasTexture()) {
        findTextureForProperty( pMaterial, FbxSurfaceMaterial::sBump );
    }
    if(!hasTexture()) {
        findTextureForProperty( pMaterial, FbxSurfaceMaterial::sDisplacementColor );
    }
    if(!hasTexture()) {
        findTextureForProperty( pMaterial, FbxSurfaceMaterial::sMultiLayer );
    }
    if(!hasTexture()) {
        findTextureForProperty( pMaterial, FbxSurfaceMaterial::sReflection );
    }
    if(!hasTexture()) {
        findTextureForProperty( pMaterial, FbxSurfaceMaterial::sShadingModel );
    }
    if(!hasTexture()) {
        findTextureForProperty( pMaterial, FbxSurfaceMaterial::sShininess );
    }
    if(!hasTexture()) {
        findTextureForProperty( pMaterial, FbxSurfaceMaterial::sTransparentColor );
    }
    if(!hasTexture()) {
        findTextureForProperty( pMaterial, FbxSurfaceMaterial::sVectorDisplacementColor );
    }
    if(!hasTexture()) {
        findTextureForProperty( pMaterial, FbxSurfaceMaterial::sMultiLayer );
    }
    
    
//    cout << "Do we have a texture for this material? " << hasTexture() << endl;
    
    if( hasTexture() ) enableTextures();
}

//--------------------------------------------------------------
void MeshMaterial::begin() {
    if( !isEnabled() ) return;
    if( areMaterialsEnabled() ) ofMaterial::begin();
    if( hasTexture() && areTexturesEnabled() ) {
//        cout << "ofxFBXMeshMaterial :: " << getName() << " binding the texture " << areMaterialsEnabled() << endl;
//        glEnable( GL_TEXTURE_2D );
//        glBindTexture(GL_TEXTURE_2D, texture->getTextureData().textureID);
        texture->bind();
    }
}

//--------------------------------------------------------------
void MeshMaterial::end() {
    if( !isEnabled() ) return;
    if( areMaterialsEnabled() ) ofMaterial::end();
    if( hasTexture() && areTexturesEnabled() ) {
        texture->unbind();
//        glBindTexture( GL_TEXTURE_2D, 0);
//        glDisable( GL_TEXTURE_2D );
    }
}

//--------------------------------------------------------------
bool MeshMaterial::hasTexture() {
//    return texture != NULL;
    if( texture ) return true;
    return false;//texture;// != NULL;
}

//--------------------------------------------------------------
//const MeshTexture* MeshMaterial::getTexturePtr() {
shared_ptr<MeshTexture> MeshMaterial::getTexturePtr() {
    return texture;
}

//--------------------------------------------------------------
void MeshMaterial::enableTextures() {
    _bTexturesEnabled = true;
}

//--------------------------------------------------------------
void MeshMaterial::disableTextures() {
    _bTexturesEnabled = false;
}

//--------------------------------------------------------------
bool MeshMaterial::areTexturesEnabled() {
    return _bTexturesEnabled;
}

//--------------------------------------------------------------
void MeshMaterial::enableMaterials() {
    _bMaterialsEnabled = true;
}

//--------------------------------------------------------------
void MeshMaterial::disableMaterials() {
    _bMaterialsEnabled = false;
}

//--------------------------------------------------------------
bool MeshMaterial::areMaterialsEnabled() {
    return _bMaterialsEnabled;
}

//--------------------------------------------------------------
void MeshMaterial::enable() {
    _bEnabled = true;
}

//--------------------------------------------------------------
void MeshMaterial::disable() {
    _bEnabled = false;
}

//--------------------------------------------------------------
bool MeshMaterial::isEnabled() {
    return _bEnabled;
}

//--------------------------------------------------------------
string MeshMaterial::getName() {
    return _name;
}

//--------------------------------------------------------------
string MeshMaterial::getInfoAsString() {
    stringstream ss;
    ss << "-- " << getName() << " Material enabled: " << isEnabled() << " material enabled: " << areMaterialsEnabled() << " ----------------------" << endl;
    ss << "emissive = " << getEmissiveColor() << endl;
    ss << "ambient = " << getAmbientColor() << endl;
    ss << "diffuse = " << getDiffuseColor() << endl;
    ss << "specular = " << getSpecularColor() << endl;
    ss << "shininess = " << getShininess() << endl;
    ss << "texture = " << _textureName << " enabled: " << areTexturesEnabled() << " has texture: " << hasTexture() << endl;
    
    return ss.str();
}

//--------------------------------------------------------------
ofFloatColor MeshMaterial::getMaterialProperty(const FbxSurfaceMaterial * pMaterial,
                                 const char * pPropertyName,
                                 const char * pFactorPropertyName ) {
    FbxDouble3 lResult(0, 0, 0);
    const FbxProperty lProperty = pMaterial->FindProperty(pPropertyName);
    const FbxProperty lFactorProperty = pMaterial->FindProperty(pFactorPropertyName);
    if (lProperty.IsValid() && lFactorProperty.IsValid()) {
        lResult = lProperty.Get<FbxDouble3>();
        double lFactor = lFactorProperty.Get<FbxDouble>();
        if (lFactor != 1) {
            lResult[0] *= lFactor;
            lResult[1] *= lFactor;
            lResult[2] *= lFactor;
        }
    }
    ofFloatColor tcolor;
    tcolor.set( lResult[0], lResult[1], lResult[2] );
    return tcolor;
}

//--------------------------------------------------------------
bool MeshMaterial::findTextureForProperty(const FbxSurfaceMaterial * pMaterial,
                                                const char * pPropertyName ) {
    
    const FbxProperty lProperty = pMaterial->FindProperty(pPropertyName);
    
    if (lProperty.IsValid()) {
        const int lTextureCount     = lProperty.GetSrcObjectCount<FbxFileTexture>();
        const int lLTextureCount    = lProperty.GetSrcObjectCount<FbxLayeredTexture>();
        ofLogVerbose("ofxFBXMeshMaterial::findTextureForProperty") << pPropertyName << " is a valid property with texture count of " << lTextureCount << " layered texture = " << lLTextureCount;
        if (lTextureCount) {
            const FbxFileTexture* lTexture = lProperty.GetSrcObject<FbxFileTexture>();
            if (lTexture && lTexture->GetUserDataPtr()) {
                _textureName = lTexture->GetName();
//                texture = static_cast<MeshTexture *>( lTexture->GetUserDataPtr() );
//                texture = *static_cast< shared_ptr<MeshTexture>* >(lTexture->GetUserDataPtr());
//                auto rtex = static_cast< shared_ptr<MeshTexture>* >(lTexture->GetUserDataPtr());
                auto ptex = static_cast<MeshTexture *>( lTexture->GetUserDataPtr() );
                texture = make_shared<MeshTexture>(*ptex);
                return true;
            }
        }
        
        if (lLTextureCount) {
            const FbxLayeredTexture* lLTexture = lProperty.GetSrcObject<FbxLayeredTexture>();
            if( lLTexture ) {
                const FbxFileTexture* lTexture = lLTexture->GetSrcObject<FbxFileTexture>();
                ofLogVerbose("ofxFBXMeshMaterial::findTextureForProperty") << "-------> Layered " << pPropertyName << " layered texture = " << lLTextureCount;
                if (lTexture && lTexture->GetUserDataPtr()) {
                    _textureName = lTexture->GetName();
                    // std::shared_ptr<disk_node> u_poi = * static_cast< std::shared_ptr<disk_node>* >( RayCallback.m_collisionObject->getUserPointer() );
//                    texture = static_cast<MeshTexture *>( lTexture->GetUserDataPtr() );
//                    texture = *static_cast< shared_ptr<MeshTexture>* >(lTexture->GetUserDataPtr());
//                    auto rtex = static_cast< shared_ptr<MeshTexture>* >(lTexture->GetUserDataPtr());
//                    texture = *rtex;
//                    texture = make_shared<MeshTexture>(ptex);
                    auto ptex = static_cast<MeshTexture *>( lTexture->GetUserDataPtr() );
                    texture = make_shared<MeshTexture>(*ptex);
                    return true;
                }
            }
        }
        
    } else {
//        cout << pPropertyName << " is NOT a valid property " << endl;
    }
    
    return false;
}





