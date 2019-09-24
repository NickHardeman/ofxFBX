//
//  ofxFBXMeshMaterial.h
//  example-BoneControl
//
//  Created by Nick Hardeman on 7/15/19.
//

#pragma once
#include "ofMain.h"
#include "ofxFBXSrcMeshMaterial.h"

class ofxFBXMeshMaterial : public ofMaterial {
public:
    
    void setup( ofxFBXSource::MeshMaterial* aSrcMat );
    void setTexture( shared_ptr<ofTexture> aUserTex );
    
    void begin();
    void end();
    
    bool hasTexture();
    bool hasSourceTexture();
    bool hasUserTexture();
    shared_ptr<ofTexture> getUserTexture();
    shared_ptr<ofxFBXSource::MeshTexture> getSrcTexture();
    
    void enableTextures();
    void disableTextures();
    bool areTexturesEnabled();
    
    void enableMaterials();
    void disableMaterials();
    bool areMaterialsEnabled();
    
    void enable();
    void disable();
    bool isEnabled();
    
    string getName();
    string getInfoAsString();
    
protected:
    bool _bTexturesEnabled = true;
    bool _bMaterialsEnabled = true;
    bool _bEnabled = true;
    string _name = "default material name";
    
    shared_ptr<ofTexture> mUserTex;
    shared_ptr<ofxFBXSource::MeshTexture> mSrcTexture;

};
