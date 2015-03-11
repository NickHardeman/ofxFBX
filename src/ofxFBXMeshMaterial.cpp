//
//  ofxFBXMeshMaterial.cpp
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 10/31/13.
//
//

#include "ofxFBXMeshMaterial.h"

//--------------------------------------------------------------
ofxFBXMeshMaterial::ofxFBXMeshMaterial() {
    texture = NULL;
    _name   = "default material";
    enableTextures();
    enableMaterials();
    enable();
}

//--------------------------------------------------------------
void ofxFBXMeshMaterial::setup( const FbxSurfaceMaterial * pMaterial ) {
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
//    cout << pMaterial->GetName() << " texture count = " << lTextureCount << endl;
    
    // pMaterial->GetClassId().Is(KFbxSurfacePhong::ClassId)
    
    // search for a texture based on the material properties :)
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
//        findTextureForProperty( pMaterial, FbxSurfaceMaterial::sMultiLayer );
    }
    
    
//    cout << "Do we have a texture for this material? " << hasTexture() << endl;
    
    if( hasTexture() ) enableTextures();
}

//--------------------------------------------------------------
void ofxFBXMeshMaterial::begin() {
    if( !isEnabled() ) return;
    if( areMaterialsEnabled() ) ofMaterial::begin();
    if( hasTexture() && areTexturesEnabled() ) {
        glEnable( GL_TEXTURE_2D );
        glBindTexture(GL_TEXTURE_2D, texture->getTextureData().textureID);
    }
}

//--------------------------------------------------------------
void ofxFBXMeshMaterial::end() {
    if( !isEnabled() ) return;
    if( areMaterialsEnabled() ) ofMaterial::end();
    if( hasTexture() && areTexturesEnabled() ) {
        glBindTexture( GL_TEXTURE_2D, 0);
        glDisable( GL_TEXTURE_2D );
    }
}

//--------------------------------------------------------------
bool ofxFBXMeshMaterial::hasTexture() {
    return texture != NULL;
}

//--------------------------------------------------------------
ofTexture* ofxFBXMeshMaterial::getTexturePtr() {
    return texture;
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
    ss << "-- " << getName() << " Material ----------------------" << endl;
    ss << "emissive = " << getEmissiveColor() << endl;
    ss << "ambient = " << getAmbientColor() << endl;
    ss << "diffuse = " << getDiffuseColor() << endl;
    ss << "specular = " << getSpecularColor() << endl;
    ss << "shininess = " << getShininess() << endl;
    
    return ss.str();
}

//--------------------------------------------------------------
ofFloatColor ofxFBXMeshMaterial::getMaterialProperty(const FbxSurfaceMaterial * pMaterial,
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
bool ofxFBXMeshMaterial::findTextureForProperty(const FbxSurfaceMaterial * pMaterial,
                                                const char * pPropertyName ) {
    
    const FbxProperty lProperty = pMaterial->FindProperty(pPropertyName);
    
    if (lProperty.IsValid()) {
        const int lTextureCount     = lProperty.GetSrcObjectCount<FbxFileTexture>();
        const int lLTextureCount    = lProperty.GetSrcObjectCount<FbxLayeredTexture>();
//        cout << pPropertyName << " is a valid property with texture count of " << lTextureCount << " layered texture = " << lLTextureCount << endl;
        if (lTextureCount) {
            const FbxFileTexture* lTexture = lProperty.GetSrcObject<FbxFileTexture>();
            if (lTexture && lTexture->GetUserDataPtr()) {
                texture = static_cast<ofTexture *>( lTexture->GetUserDataPtr() );
                return true;
            }
        }
        
        if (lLTextureCount) {
            const FbxLayeredTexture* lLTexture = lProperty.GetSrcObject<FbxLayeredTexture>();
            if( lLTexture ) {
                const FbxFileTexture* lTexture = lLTexture->GetSrcObject<FbxFileTexture>();
//                cout << "-------> Layered " << pPropertyName << " layered texture = " << lLTextureCount << endl;
                if (lTexture && lTexture->GetUserDataPtr()) {
                    texture = static_cast<ofTexture *>( lTexture->GetUserDataPtr() );
                    return true;
                }
            }
        }
        
    } else {
//        cout << pPropertyName << " is NOT a valid property " << endl;
    }
    
    return false;
}





