//
//  ofxFBXScene.h
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 10/31/13.
//
//

#include "ofxFBXSrcScene.h"
#include "Common/Common.h"

FbxSystemUnit ofxFBXSource::Scene::FbxUnits = FbxSystemUnit::cm;

using namespace ofxFBXSource;

//--------------------------------------------------------------
Scene::Scene() {
    lSdkManager = NULL;
    lScene      = NULL;
    currentFbxAnimationLayer = NULL;
    fbxFilePath = "";
}

//--------------------------------------------------------------
Scene::~Scene() {
    
	if (currentFbxAnimationLayer != NULL) {
		currentFbxAnimationLayer->Destroy();
	}

    if(lScene != NULL ) {
        deleteCachedTexturesInScene( lScene );
        deleteCachedMaterialsRecursive( lScene->GetRootNode() );
		lScene->Destroy(true);
    }

	
    
    if(lSdkManager != NULL) {
		bool lResult;
		// Destroy all objects created by the FBX SDK.
		//DestroySdkObjects(lSdkManager, lResult);
		//lSdkManager->Destroy();
		//delete lSdkManager;
       // lSdkManager = NULL;
    }
}

//--------------------------------------------------------------
bool Scene::load( Settings aSettings ) {
    if( aSettings.filePath == "" ) {
        ofLogError("ofxFBXSource::Scene::load()") << " file path not set in settings!! " << " :(";
        return false;
    }
    return load( aSettings.filePath, aSettings );
}

//--------------------------------------------------------------
bool Scene::load( string path, Scene::Settings aSettings ) {
    
    if( !ofFile::doesFileExist(path)) {
        ofLogError("ofxFBXSource::Scene::load()") << " File does not exist!!: " << path << " :(";
        return false;
    }
    
    bool lResult = false;
    
	// Prepare the FBX SDK.
	InitializeSdkObjects(lSdkManager, lScene);
    
    // make sure to import bones if we have animations. Since some animations are bone based.
    if(aSettings.importAnimations) {
        // make sure to import the bones / known as links in the FBX SDK
        aSettings.importBones   = true;
        aSettings.importShapes  = true; // i think this is referring to blend shapes?
    }
    
    _settings = aSettings;
    
    // load only what is requested //
    (*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_MATERIAL,        _settings.importMaterials );
    // load in the texture file locations information, _settings.importTextures determines if pixel data is loaded //
    (*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_TEXTURE,         true );//_settings.importTextures );
    (*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_LINK,            _settings.importBones );
    (*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_SHAPE,           _settings.importShapes );
    (*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_GOBO,            _settings.importGobos );
    (*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_ANIMATION,       _settings.importAnimations );
    
	lResult = LoadScene(lSdkManager, lScene, ofToDataPath(path).c_str());
    
	if(!lResult || !lScene ) {
        ofLogError("ofxFBXSource::Scene") << "An error occurred while loading the scene... :(";
        return false;
	}
    
    fbxFilePath = ofToDataPath(path);
    
    if(aSettings.printInfo) cout << "ofxFBXSrcScene :: loading fbx file from " << fbxFilePath << endl;
    
    // Convert Axis System to what is used in this example, if needed
    FbxAxisSystem SceneAxisSystem = lScene->GetGlobalSettings().GetAxisSystem();
//    FbxAxisSystem OurAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
    FbxAxisSystem OurAxisSystem( FbxAxisSystem::OpenGL );
    if( SceneAxisSystem != OurAxisSystem ) {
        OurAxisSystem.ConvertScene(lScene);
    }
    
    // Convert Unit System to what is used in this example, if needed
    FbxSystemUnit SceneSystemUnit = lScene->GetGlobalSettings().GetSystemUnit();
    ofLogVerbose("ofxFBXSource::Scene") << "SceneSystemUnit = " << SceneSystemUnit.GetScaleFactor() << " " << SceneSystemUnit.GetScaleFactorAsString_Plurial();
    
    if( ofxFBXSource::Scene::FbxUnits.GetScaleFactor() == 0 ) {
        // setting to the default cm //
        ofxFBXSource::Scene::FbxUnits = FbxSystemUnit::cm;
    }
    
    if( SceneSystemUnit.GetScaleFactor() != ofxFBXSource::Scene::FbxUnits.GetScaleFactor() ) {
        ofLogVerbose("ofxFBXSource::Scene") << "coverting scene from " << SceneSystemUnit.GetScaleFactorAsString_Plurial() << " to " << ofxFBXSource::Scene::FbxUnits.GetScaleFactorAsString_Plurial();
        ofxFBXSource::Scene::FbxUnits.ConvertScene( lScene );
    }
    
//    FbxGeometryConverter lGeomConverter( lSdkManager );
//    lGeomConverter.Triangulate(lScene, /*replace*/true);
    
    // Split meshes per material, so that we only have one material per mesh (for VBO support)
//    if(!lGeomConverter.SplitMeshesPerMaterial(lScene, /*replace*/true)) {
//        cout << "There was an error splitting the meshes per material" << endl;
//    }
    
    // cache all of the textures by loading them from disk //
    cacheTexturesInScene( lScene );
    // cache all of the materials in the scene as ofxFBXMeshMaterial //
    if( _settings.importMaterials ) {
        cacheMaterialsRecursive( lScene->GetRootNode() );
    }
    
    // Initialize the frame period.
    fbxFrameTime.SetTime(0, 0, 0, 1, 0, lScene->GetGlobalSettings().GetTimeMode());
//    fbxFrameTime.Set( lScene->GetGlobalSettings().GetFrameRate() );
//    fbxFrameTime.SetTime(0, 0, 0, 1, 0, FbxTime::eFrames24 );
//    fbxFrameTime.SetSecondDouble( 1.f / 24.f );
    ofLogVerbose() << "ofxFBXSource::Scene :: time mode: " << lScene->GetGlobalSettings().GetTimeMode() << endl;
    
    // Get the list of all the animation stack.
    if( areAnimationsEnabled() ) {
        populateAnimationInformation();
    }
    
    if(animations.size() > 0) {
        FbxAnimStack * lCurrentAnimationStack = lScene->FindMember<FbxAnimStack>( (&animations[0].fbxname)->Buffer());
        if (lCurrentAnimationStack == NULL) {
            ofLogError("ofxFBXSource::Scene :: ") << "this is a problem. The anim stack should be found in the scene!" << endl;
            // this is a problem. The anim stack should be found in the scene!
            return false;
        }
        
        // we assume that the first animation layer connected to the animation stack is the base layer
        // (this is the assumption made in the FBXSDK)
        currentFbxAnimationLayer = lCurrentAnimationStack->GetMember<FbxAnimLayer>();
    }
    
    _parseNodesRecursive( lScene->GetRootNode() );
    
    // populate bones //
    if( _settings.importBones ) {
        _constructSkeletons( lScene->GetRootNode() );
    }
    
    // add in the keyframe information //
    if( areAnimationsEnabled() && _settings.useKeyFrames ) {
        // politely inform the skeletons that we will be using keyframes //
//        for( auto& skel : skeletons ) {
//            skel->setUseKeyFrames( true );
//        }
//
//        for( auto& mesh : meshes ) {
//            mesh->setUseKeyFrames( true );
//        }
        for( auto& node : mSrcNodes ) {
            node->setUseKeyFrames(true);
        }
        
        for( int i = 0; i < getNumAnimations(); i++ ) {
            
            FbxAnimStack* currentAnimationStack = getFBXScene()->FindMember<FbxAnimStack>( (&animations[i].fbxname)->Buffer() );
            if (currentAnimationStack == NULL) {
                // this is a problem. The anim stack should be found in the scene!
                ofLogWarning("ofxFBXSource::Scene :: setAnimation : the anim stack was not found in the scene!");
                continue;
            }
            currentFbxAnimationLayer = currentAnimationStack->GetMember<FbxAnimLayer>();
            getFBXScene()->SetCurrentAnimationStack( currentAnimationStack );
            
            populateKeyFrames( lScene->GetRootNode(), i );
        }
    }
    
    if(animations.size() > 0) {
        FbxAnimStack * lCurrentAnimationStack = lScene->FindMember<FbxAnimStack>( (&animations[0].fbxname)->Buffer());
        if (lCurrentAnimationStack == NULL) {
            ofLogError("ofxFBXSource::Scene :: ") << "this is a problem. The anim stack should be found in the scene!" << endl;
            // this is a problem. The anim stack should be found in the scene!
            return false;
        }
        
        // we assume that the first animation layer connected to the animation stack is the base layer
        // (this is the assumption made in the FBXSDK)
        currentFbxAnimationLayer = lCurrentAnimationStack->GetMember<FbxAnimLayer>();
    }
    
    _parentNodesRecursive(lScene->GetRootNode());
    
    if( _settings.printInfo ) {
        cout << getInfoString() << endl;
    }
    
    return true;
}


#pragma mark - Getters
// ---------------------------------------------- getters
FbxScene* Scene::getFBXScene() {
    return lScene;
}

// ---------------------------------------------- utils
string Scene::getFbxFilePath() {
    return fbxFilePath;
}

//--------------------------------------------------------------
string Scene::getFbxFolderPath() {
    if(fbxFilePath == "") return "";
    const FbxString lAbsFbxFileName = FbxPathUtils::Resolve( getFbxFilePath().c_str() );
    return  (string)FbxPathUtils::GetFolderName(lAbsFbxFileName);
}

//--------------------------------------------------------------
ofxFBXSource::Node::NodeType Scene::getOFXNodeType( FbxNode* aNode ) {
    FbxNodeAttribute* lNodeAttribute = aNode->GetNodeAttribute();
    if( lNodeAttribute ) {
        FbxNodeAttribute::EType et = lNodeAttribute->GetAttributeType();
        if (et == FbxNodeAttribute::eMesh ||
            et == FbxNodeAttribute::eNurbs ||
            et == FbxNodeAttribute::ePatch ||
            et == FbxNodeAttribute::eNurbsSurface) {
            return ofxFBXSource::Node::OFX_FBX_MESH;
        } else if( et == FbxNodeAttribute::eNurbsCurve ) {
            return ofxFBXSource::Node::OFX_FBX_NURBS_CURVE;
        } else if( et == FbxNodeAttribute::eNull ) {
            return ofxFBXSource::Node::OFX_FBX_NULL;
        } else if( et == FbxNodeAttribute::eSkeleton ) {
            if( aNode->GetSkeleton() && aNode->GetSkeleton()->IsSkeletonRoot() ) {
                return ofxFBXSource::Node::OFX_FBX_SKELETON;
            }
            return ofxFBXSource::Node::OFX_FBX_BONE;
        }
    }
    
    return ofxFBXSource::Node::OFX_FBX_NULL;
}

//--------------------------------------------------------------
vector< shared_ptr<ofxFBXSource::Node> > Scene::getSceneNodes() {
    return mSceneNodes;
}

//--------------------------------------------------------------
string Scene::getInfoString() {
    stringstream ss;
    ss << "ofxFBXSource::Scene : " << ofFilePath::getBaseName(fbxFilePath)<<"."<<ofFilePath::getFileExt(fbxFilePath) << endl;
    for( auto& sn : mSceneNodes ) {
        ss << sn->getAsString();
    }
    return ss.str();
}

#pragma mark - Textures
// ---------------------------------------------- animation
void Scene::cacheTexturesInScene( FbxScene* pScene ) {
    // Load the textures into GPU, only for file texture now
    const int lTextureCount = pScene->GetTextureCount();
    for (int lTextureIndex = 0; lTextureIndex < lTextureCount; ++lTextureIndex) {
        FbxTexture * lTexture = pScene->GetTexture(lTextureIndex);
        FbxFileTexture * lFileTexture = FbxCast<FbxFileTexture>(lTexture);
        if (lFileTexture && !lFileTexture->GetUserDataPtr()) {
            // Try to load the texture from absolute path
            const FbxString lFileName = lFileTexture->GetFileName();
            
            GLuint lTextureObject = 0;
            bool bFoundTexture = false;
            string filepath  = (string)lFileName;
            bFoundTexture = isValidTexturePath( filepath );
            
            const FbxString lAbsFbxFileName = FbxPathUtils::Resolve( fbxFilePath.c_str() );
            const FbxString lAbsFolderName = FbxPathUtils::GetFolderName(lAbsFbxFileName);
            
            if( !bFoundTexture ) {
                // Load texture from relative file name (relative to FBX file)
                const FbxString lResolvedFileName = FbxPathUtils::Bind(lAbsFolderName, lFileTexture->GetRelativeFileName());
                filepath = (string)lResolvedFileName;
                bFoundTexture = isValidTexturePath( filepath );
            }
            
            if(!bFoundTexture) {
                // Load texture from file name only (relative to FBX file)
                const FbxString lTextureFileName = FbxPathUtils::GetFileName(lFileName);
                const FbxString lResolvedFileName = FbxPathUtils::Bind(lAbsFolderName, lTextureFileName);
                filepath = (string)lResolvedFileName;
                bFoundTexture = isValidTexturePath( filepath );
            }
            
            if(!bFoundTexture) {
                ofLogWarning("ofxFBXSource::Scene :: Could not find texture ") << filepath;
                continue;
            }
            
            if (bFoundTexture) {
//                ofxFBXSource::MeshTexture* texture = new ofxFBXSource::MeshTexture();
                auto texture = make_shared<ofxFBXSource::MeshTexture>();
                
                if( _settings.importTextures ) {
                    ofPixels pixels;
                    bool loaded = ofLoadImage(pixels, filepath);
                    if(loaded){
                        texture->allocate(pixels.getWidth(), pixels.getHeight(), ofGetGLInternalFormat(pixels), false);
                        pixels.mirror(true, false);
                        texture->loadData(pixels);
                    }
                    if(loaded) {
                        ofLogVerbose("ofxFBXSource::Scene :: Loaded the texture from ") << filepath << endl;
                        
                        texture->getTextureData().bFlipTexture = true;
                        texture->disableTextureMatrix();
                        texture->filePath = filepath;
                        
                        lFileTexture->SetUserDataPtr( texture.get() );
                    } else {
                        ofLogError("ofxFBXSource::Scene :: Failed to load texture for ") << lFileName;
                        //delete texture;
                        //texture = NULL;
                        texture.reset();
                    }
                } else {
                    ofLogVerbose("ofxFBXSource::Scene :: found texture, but not loading pixel data ") << filepath << endl;
                    texture->filePath = filepath;
                    lFileTexture->SetUserDataPtr( texture.get() );
                }
                if( texture ) {
                    mTextures.push_back(texture);
                }
            }
        }
    }
}

void Scene::deleteCachedTexturesInScene( FbxScene* pScene ) {
    const int lTextureCount = pScene->GetTextureCount();
    for (int lTextureIndex = 0; lTextureIndex < lTextureCount; ++lTextureIndex) {
        FbxTexture* lTexture = pScene->GetTexture(lTextureIndex);
        FbxFileTexture* lFileTexture = FbxCast<FbxFileTexture>(lTexture);
        if (lFileTexture && lFileTexture->GetUserDataPtr()) {
//            ofxFBXSource::MeshTexture* texture = static_cast<ofxFBXSource::MeshTexture *>( lFileTexture->GetUserDataPtr() );
            //auto texture = static_cast< shared_ptr<ofxFBXSource::MeshTexture>* >(lFileTexture->GetUserDataPtr());
            lFileTexture->SetUserDataPtr(NULL);
            //delete texture;
            //texture = NULL;
        }
    }
    mTextures.clear();
}

bool Scene::isValidTexturePath( string aPathToTexture ) {
    ofFile tfile( (string)aPathToTexture );
    if(!tfile.exists()) return false;
    bool bGoodToGo = false;
    if(tfile.getExtension() == "tga") bGoodToGo = true;
    if(tfile.getExtension() == "jpg") bGoodToGo = true;
    if(tfile.getExtension() == "png") bGoodToGo = true;
    if(tfile.getExtension() == "bmp") bGoodToGo = true;
    if(tfile.getExtension() == "tif") bGoodToGo = true;
    if(tfile.getExtension() == "tiff") bGoodToGo = true;
    return bGoodToGo;
}

#pragma mark - Materials
// ---------------------------------------------- materials
void Scene::cacheMaterialsRecursive( FbxNode* pNode ) {
    // Bake material and hook as user data.
    const int lMaterialCount = pNode->GetMaterialCount();
    for (int lMaterialIndex = 0; lMaterialIndex < lMaterialCount; ++lMaterialIndex) {
        FbxSurfaceMaterial * lMaterial = pNode->GetMaterial(lMaterialIndex);
        if (lMaterial && !lMaterial->GetUserDataPtr()) {
            ofxFBXSource::MeshMaterial* materialCache = new ofxFBXSource::MeshMaterial();
            ofLogVerbose("ofxFBXSource::Scene") << "found a material :: " << lMaterial->GetName();
//            ofLogNotice("ofxFBXScene") << "found a material :: " << lMaterial->GetName();
            materialCache->setup( lMaterial );
            lMaterial->SetUserDataPtr( materialCache );
        }
    }
    
    const int lChildCount = pNode->GetChildCount();
    for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex) {
        cacheMaterialsRecursive( pNode->GetChild(lChildIndex) );
    }
}

void Scene::deleteCachedMaterialsRecursive( FbxNode* pNode ) {
    const int lMaterialCount = pNode->GetMaterialCount();
    for (int lMaterialIndex = 0; lMaterialIndex < lMaterialCount; ++lMaterialIndex) {
        FbxSurfaceMaterial * lMaterial = pNode->GetMaterial(lMaterialIndex);
        if (lMaterial && lMaterial->GetUserDataPtr()) {
            ofxFBXSource::MeshMaterial* materialCache = static_cast<ofxFBXSource::MeshMaterial *>(lMaterial->GetUserDataPtr());
            lMaterial->SetUserDataPtr(NULL);
            delete materialCache;
			materialCache = NULL;
        }
    }
    
    const int lChildCount = pNode->GetChildCount();
    for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex) {
        deleteCachedMaterialsRecursive( pNode->GetChild(lChildIndex) );
    }
}

#pragma mark - Animation
// ---------------------------------------------- animation
int Scene::getNumAnimations() {
    return animations.size();
}

//--------------------------------------------------------------
bool Scene::areAnimationsEnabled() {
    return _settings.importAnimations;
}

//--------------------------------------------------------------
void Scene::populateAnimationInformation() {
    FbxArray<FbxString*> mAnimStackNameArray;
    lScene->FillAnimStackNameArray(mAnimStackNameArray);
//    animations.resize( mAnimStackNameArray.GetCount() );
    for(int i = 0; i < mAnimStackNameArray.GetCount(); i++ ) {
        
        FbxAnimStack* lCurrentAnimationStack    = lScene->FindMember<FbxAnimStack>(mAnimStackNameArray[i]->Buffer());
        FbxTakeInfo* lCurrentTakeInfo           = lScene->GetTakeInfo(*(mAnimStackNameArray[i]));
        
        FbxTime startTime, endTime;
        
        if (lCurrentTakeInfo) {
            startTime   = lCurrentTakeInfo->mLocalTimeSpan.GetStart();
            endTime     = lCurrentTakeInfo->mLocalTimeSpan.GetStop();
        } else {
            // Take the time line value
            FbxTimeSpan lTimeLineTimeSpan;
            lScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(lTimeLineTimeSpan);
            startTime   = lTimeLineTimeSpan.GetStart();
            endTime     = lTimeLineTimeSpan.GetStop();
        }
        ofxFBXAnimation animation;
        animation.setup( startTime, endTime, fbxFrameTime );
        animation.fbxCurrentTime = animation.fbxStartTime;
        FbxString tfbxName      = FbxString(*mAnimStackNameArray[i]);
        animation.fbxname   = tfbxName;
        animation.index     = i;
        animation.name      = FbxString(*mAnimStackNameArray[i]);
        animations.push_back( animation );
        //FbxString ostr = startTime;
        //startTime.
        ofLogVerbose("ofxFBXSource::Scene") << i << " - " << " ofxFBXSource::Scene :: animations[" << i << "].name: " << animations[i].name << " starttime: " << startTime.GetTimeString() << " end time: " << endTime.GetTimeString() << endl;
        
//        int l;
//        int nbAnimLayers = lCurrentAnimationStack->GetMemberCount<FbxAnimLayer>();
        
//        cout << "ofxFBXScene :: animation: " << animations[i].name << " num layers: " << nbAnimLayers << " | " << ofGetFrameNum() << endl;
        
//        for (l = 0; l < nbAnimLayers; l++) {
//            FbxAnimLayer* lAnimLayer = lCurrentAnimationStack->GetMember<FbxAnimLayer>(l);
//            populateKeyFrames( FbxNode* pNode, FbxAnimLayer* pAnimLayer, int aAnimIndex ) {
            
//        }
//        cout << i << " - ofxFBXScene :: name " << animations[i].name << "| fbxname: " << FbxString(*mAnimStackNameArray[i]) << endl;
    }
    
    FbxArrayDelete(mAnimStackNameArray);
    
    // check if there is an xml file //
    if( fbxFilePath != "" && _settings.useKeyFrames ) {
        string tencDir = ofFilePath::addTrailingSlash( ofFilePath::getEnclosingDirectory( fbxFilePath ) );
        string txmlPath = tencDir + ofFilePath::getBaseName(fbxFilePath)+".xml";
        if( ofFile::doesFileExist(txmlPath)) {
            ofXml txml;
            if( txml.load(txmlPath)) {
                ofLogVerbose("ofxFBXSource::Source : populateAnimationInformation : found animation file: ") << txmlPath;
                auto animationClipsXml = txml.find("//AnimationClip");
                if( animationClipsXml.size() > 0 ) {
                    clearAnimations();
                    for( int i = 0; i < animationClipsXml.size(); i++ ) {
                        string tname = animationClipsXml[i].getAttribute("name").getValue();
                        int sframe = animationClipsXml[i].getAttribute("startFrame").getIntValue();
                        int eframe = animationClipsXml[i].getAttribute("endFrame" ).getIntValue();
                        string loopStr = animationClipsXml[i].getAttribute("loops").getValue();
                        bool bLoops = false;
                        if( ofToLower(loopStr) == "true" ) {
                            bLoops = true;
                        }
                        auto& tanim = addAnimation( tname, sframe, eframe );
                        tanim.setLoops( bLoops );
                        
                        ofLogVerbose("ofxFBXSource::Source : adding animation: ") << i << " name: " << tanim.name << " num frames: " << tanim.getTotalNumFrames() << " loops: " << tanim.getLoops();
                    }
                }
            }
        }
    }
}

//--------------------------------------------------------------
void Scene::clearAnimations() {
    animations.clear();
    
    if( areAnimationsEnabled() && _settings.useKeyFrames ) {
        // politely inform the skeletons that we will be using keyframes //
//        for( auto& skel : skeletons ) {
//            skel->clearKeyFrames();
//        }
//
//        for( auto& mesh : meshes ) {
//            mesh->clearKeyFrames();
//        }
        
        for( auto& node : mSrcNodes ) {
            node->clearKeyFrames();
        }
    }
    bMeshKeyframesCached = false;
    
}

//--------------------------------------------------------------
ofxFBXAnimation& Scene::addAnimation( string aname, int aFrameBegin, int aFrameEnd, int aAnimStackIndex ) {
    FbxArray<FbxString*> mAnimStackNameArray;
    lScene->FillAnimStackNameArray(mAnimStackNameArray);
    //animations.resize( mAnimStackNameArray.GetCount() );
    
    if( mAnimStackNameArray.GetCount() < 1 ) {
        ofLogError("ofxFBXSource::Scene :: addAnimation : no animations in file found" );
        return dummyAnimation;
    }
    if( aAnimStackIndex >= mAnimStackNameArray.GetCount()) {
        ofLogError("ofxFBXSource::Scene :: addAnimation : anim stack index" ) << " ("<<aAnimStackIndex<<")  is out of bounds";
        return dummyAnimation;
    }
    
    FbxAnimStack* lCurrentAnimationStack    = lScene->FindMember<FbxAnimStack>(mAnimStackNameArray[aAnimStackIndex]->Buffer());
    FbxTakeInfo* lCurrentTakeInfo           = lScene->GetTakeInfo(*(mAnimStackNameArray[aAnimStackIndex]));
    
    FbxTime startTime, endTime;
    
    if (lCurrentTakeInfo) {
        startTime   = lCurrentTakeInfo->mLocalTimeSpan.GetStart();
        endTime     = lCurrentTakeInfo->mLocalTimeSpan.GetStop();
    } else {
        // Take the time line value
        FbxTimeSpan lTimeLineTimeSpan;
        lScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(lTimeLineTimeSpan);
        startTime   = lTimeLineTimeSpan.GetStart();
        endTime     = lTimeLineTimeSpan.GetStop();
    }
    
    int totalFrames = ceil( (endTime.GetMilliSeconds() - startTime.GetMilliSeconds()) / fbxFrameTime.GetMilliSeconds() );
    
//    int ttime = ofMap(aPct, 0.f, 1.f, startTimeMillis, stopTimeMillis, true );
//    fbxCurrentTime.SetMilliSeconds(ttime);
    
    FbxTime startFrameTime = startTime + fbxFrameTime * aFrameBegin;
    FbxTime endFrameTime = startTime + fbxFrameTime * aFrameEnd;
    
    startFrameTime.SetMilliSeconds( (int)ofMap((float)aFrameBegin / (float)totalFrames, 0.f, 1.f, startTime.GetMilliSeconds(), endTime.GetMilliSeconds(), true ));
    endFrameTime.SetMilliSeconds( (int)ofMap((float)aFrameEnd / (float)totalFrames, 0.f, 1.f, startTime.GetMilliSeconds(), endTime.GetMilliSeconds(), true ));
    
    if( startFrameTime > endTime ) startFrameTime = endTime;
    if( endFrameTime > endTime ) endFrameTime = endTime;
//    fbxCurrentTime  = fbxStartTime + fbxFrameTime * tframe;
    
    ofxFBXAnimation animation;
    animation.setup( startFrameTime, endFrameTime, fbxFrameTime );
    animation.fbxCurrentTime = animation.fbxStartTime;
    FbxString tfbxName      = FbxString(*mAnimStackNameArray[aAnimStackIndex]);
    animation.fbxname   = tfbxName;
    animation.index     = animations.size();
    animation.name      = aname;//FbxString(*mAnimStackNameArray[i]);
    animations.push_back( animation );
    ofLogVerbose("ofxFBXSource::Scene") << " Adding animation :: name: " << animation.name << " starttime: " << startFrameTime.GetTimeString() << " end time: " << endFrameTime.GetTimeString() << " num frames: " << animation.getTotalNumFrames() << endl;
    
    if( areAnimationsEnabled() && _settings.useKeyFrames ) {
        FbxAnimStack* currentAnimationStack = getFBXScene()->FindMember<FbxAnimStack>( (&animation.fbxname)->Buffer() );
        if (currentAnimationStack != NULL) {
            getFBXScene()->SetCurrentAnimationStack( currentAnimationStack );
            populateKeyFrames( lScene->GetRootNode(), animation.index );
        } else {
            // this is a problem. The anim stack should be found in the scene!
            ofLogWarning("ofxFBXSource::Scene :: addAnimation : the anim stack was not found in the scene!");
        }
    }
    
    FbxArrayDelete(mAnimStackNameArray);
    return animations.back();
}

//--------------------------------------------------------------
void Scene::populateAnimations( vector<ofxFBXAnimation>& aInVector) {
    for(int i = 0; i < animations.size(); i++ ) {
        aInVector.push_back(animations[i]);
    }
}

#pragma mark - Bones / Skeleton
//--------------------------------------------------------------
void Scene::constructSkeletonsRecursive( ofxFBXSource::Skeleton* aSkeleton, FbxNode* pNode, int aBoneLevel ) {
    FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
    if (lNodeAttribute && lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton) {
        if(pNode->GetParent() && pNode->GetParent()->GetNodeAttribute()) {
            if(pNode->GetParent()->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton) {
                if(pNode->GetParent()->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton) {
                    ofxFBXSource::Bone* bonePtr         = static_cast<ofxFBXSource::Bone *>(pNode->GetUserDataPtr());
                    ofxFBXSource::Bone* bonePtrParent   = static_cast<ofxFBXSource::Bone *>(pNode->GetParent()->GetUserDataPtr());
                    
                    bonePtr->parentBoneName     = bonePtrParent->getName();
                    bonePtr->setParent( *bonePtrParent, true );
                    
                    bonePtr->setLocalTransformMatrix( GetGlobalPosition(pNode, FBXSDK_TIME_INFINITE, NULL ) );
                    bonePtr->cacheStartTransforms();
                    
                    if( bonePtrParent ) {
                        //                        bonePtrParent->childBones[ bonePtr->getName() ] = bonePtr;
                        //                        auto cbone = dynamic_pointer_cast<ofxFBXSource::Bone>(_getOfxNodeFromNodeUserData(pNode));
                        //                        ofxFBXSource::Bone* bonePtr = static_cast<ofxFBXSource::Bone *>(aNode->GetUserDataPtr());
                        shared_ptr<ofxFBXSource::Bone> cbone;
                        for( auto& tm : mSrcNodes ) {
                            if( tm.get() == bonePtr ) {
                                cbone = dynamic_pointer_cast<ofxFBXSource::Bone>(tm);
                                break;
                            }
                        }
                        if( cbone ) {
                            bonePtrParent->childBones[ bonePtr->getName() ] = cbone;
                        }
                    }
                }
            }
        }
    }
    
    const int lChildCount = pNode->GetChildCount();
    for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex) {
        constructSkeletonsRecursive( aSkeleton, pNode->GetChild(lChildIndex), aBoneLevel+1 );
    }
}

#pragma mark - Poses
//--------------------------------------------------------------
int Scene::getNumPoses() {
    if( lScene == NULL ) return 0;
    return lScene->GetPoseCount();
}

//--------------------------------------------------------------
void Scene::populatePoses( vector< shared_ptr<ofxFBXSource::Pose> >& aInPoses ) {
    
    for( int i = 0; i < lScene->GetPoseCount(); i++ ) {
        aInPoses.push_back( shared_ptr<ofxFBXSource::Pose>( new ofxFBXSource::Pose() ));
        shared_ptr< ofxFBXSource::Pose > pose = aInPoses.back();
        pose->setup( lScene->GetPose(i), i );
    }
    
}

#pragma mark - Private
//--------------------------------------------------------------
void Scene::_parseNodesRecursive( FbxNode* aNode ) {
    if( aNode == NULL ) { return; }
    
//    FbxNodeAttribute* lNodeAttribute = aNode->GetNodeAttribute();
    ofxFBXSource::Node::NodeType ftype = getOFXNodeType( aNode );
    if (ftype == ofxFBXSource::Node::OFX_FBX_MESH) {
        auto tmesh = _getMeshForNode( aNode );
        if( tmesh ) {
//            meshes.push_back( tmesh );
            mSrcNodes.push_back(tmesh);
        }
    } else if(ftype == ofxFBXSource::Node::OFX_FBX_NURBS_CURVE) {
        if( _settings.importNurbsCurves ) {
            auto tcurve = _getNurbsCurveForNode( aNode );
            if( tcurve ) {
//                mNurbsCurves.push_back(tcurve);
                mSrcNodes.push_back(tcurve);
            }
        }
    } else if(ftype == ofxFBXSource::Node::OFX_FBX_NULL ) {
        auto tnull = _getNullForNode( aNode );
        if( tnull ) {
//            mNullNodes.push_back(tnull);
            mSrcNodes.push_back(tnull);
        }
    } else if( ftype == ofxFBXSource::Node::OFX_FBX_SKELETON || ftype == ofxFBXSource::Node::OFX_FBX_BONE ) {
        if( _settings.importBones ) {
            auto tbone = _getBoneForNode(aNode);
            if( tbone ) {
//                bones.push_back( tbone );
                mSrcNodes.push_back(tbone);
            }
        }
    }
    
    
    const int lChildCount = aNode->GetChildCount();
    for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex) {
        _parseNodesRecursive( aNode->GetChild(lChildIndex) );
    }
}

//--------------------------------------------------------------
void Scene::_constructSkeletons( FbxNode* aNode ) {
    list<FbxNode*> skeletonBases;
    
    _getSkeletonBases( aNode, skeletonBases );
    
    list<FbxNode*>::iterator it;
    for(it = skeletonBases.begin(); it != skeletonBases.end(); ++it ) {
        
        auto skeletonPtr = make_shared<ofxFBXSource::Skeleton>();
//        skeletons.push_back( skeletonPtr );
        mSrcNodes.push_back( skeletonPtr );
        FbxNode* skeletonNode = *it;
        //auto bonePtr = static_cast<ofxFBXSource::Bone *>( skeletonNode->GetUserDataPtr() );
        
//        auto cbone = dynamic_pointer_cast<ofxFBXSource::Bone>(_getOfxNodeFromNodeUserData(skeletonNode));
        ofxFBXSource::Bone* bonePtr = static_cast<ofxFBXSource::Bone *>( skeletonNode->GetUserDataPtr() );
        shared_ptr<ofxFBXSource::Bone> cbone;
        for( auto& tm : mSrcNodes ) {
            if( tm.get() == bonePtr ) {
                cbone = dynamic_pointer_cast<ofxFBXSource::Bone>(tm);
                break;
            }
        }
        
//        cout << "ofxFBXSrcScene :: _constructSkeletons : we have a bone " << (cbone ? "Good" : "Bad" ) << endl;
        
        if( cbone ) {
            skeletonPtr->root = cbone;//static_cast<ofxFBXSource::Bone *>( skeletonNode->GetUserDataPtr() );
            skeletonPtr->setup( skeletonNode );
        
            constructSkeletonsRecursive( skeletonPtr.get(), skeletonNode, 0 );
        }
    }
}

//--------------------------------------------------------------
void Scene::_getSkeletonBases( FbxNode* aNode, list<FbxNode*>& aSkeletonBases ) {
    ofxFBXSource::Node::NodeType ftype = getOFXNodeType( aNode );
    if( ftype == ofxFBXSource::Node::OFX_FBX_SKELETON ) {
        aSkeletonBases.push_back( aNode );
    }
    
    const int lChildCount = aNode->GetChildCount();
    for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex) {
        _getSkeletonBases( aNode->GetChild(lChildIndex), aSkeletonBases );
    }
}

//--------------------------------------------------------------
shared_ptr<ofxFBXSource::Mesh> Scene::_getMeshForNode( FbxNode* aNode ) {
    shared_ptr<ofxFBXSource::Mesh> tmesh;
    if( aNode == NULL ) return tmesh;
    
    if(aNode->GetMesh()) {
        FbxGeometryConverter lGeomConverter( lSdkManager );
        lGeomConverter.Triangulate( aNode->GetMesh(), true );
        
        tmesh = make_shared<ofxFBXSource::Mesh>();
        tmesh->setup( aNode );
        
        if( !aNode->GetUserDataPtr() ) {
            aNode->SetUserDataPtr( tmesh.get() );
        }
        
//        cout << "ofxFBXScene :: _getMeshForNode : name = " << ofxFBXNode::getFbxTypeStringFromNode( aNode ) << endl;
//        cout << "ofxFBXScene :: GetShapeCount " << aNode->GetSubdiv() << " patch: " << aNode->GetPatch() << endl;
        
        FbxAMatrix lGlobalPosition = GetGlobalPosition(aNode, FBXSDK_TIME_INFINITE, NULL );
        if( !aNode->GetParent() ) {
            tmesh->setLocalTransformMatrix(lGlobalPosition);
        } else {
            FbxAMatrix lLocalPosition = GetLocalPositionForNode(aNode, FBXSDK_TIME_INFINITE, NULL );
            tmesh->setLocalTransformMatrix(lLocalPosition);
        }
        tmesh->cacheStartTransforms();
        
        // Associtate the clusters with some user data so that we can move them around //
        int lSkinCount = aNode->GetMesh()->GetDeformerCount(FbxDeformer::eSkin);
        
        if( lSkinCount > 0 && _settings.importBones ) {
            
            for ( int lSkinIndex=0; lSkinIndex<lSkinCount; ++lSkinIndex) {
                FbxSkin * lSkinDeformer = (FbxSkin *)aNode->GetMesh()->GetDeformer(lSkinIndex, FbxDeformer::eSkin);
                
                int lClusterCount = lSkinDeformer->GetClusterCount();
                for ( int lClusterIndex=0; lClusterIndex<lClusterCount; ++lClusterIndex) {
                    FbxCluster* lCluster = lSkinDeformer->GetCluster(lClusterIndex);
                    if (!lCluster->GetLink())
                        continue;
                    
                    if ( !lCluster->GetUserDataPtr() ) {
                        auto fcluster = make_shared<ofxFBXSource::Cluster>();
                        clusters.push_back( fcluster );
//                        mSrcNodes.push_back(fcluster);
                        fcluster->setup( lGlobalPosition, aNode->GetMesh(), lCluster );
                        lCluster->SetUserDataPtr( fcluster.get() );
                    }
                }
            }
        }
    }
    
    
    return tmesh;
}

//--------------------------------------------------------------
shared_ptr<ofxFBXSource::NurbsCurve> Scene::_getNurbsCurveForNode( FbxNode* aNode ) {
    shared_ptr<ofxFBXSource::NurbsCurve> tc;
    if( aNode == NULL ) return tc;
    
    if(aNode->GetNurbsCurve() != NULL) {
        
        tc = make_shared<ofxFBXSource::NurbsCurve>();
//            mNurbsCurves.push_back(curve);
        tc->setup( aNode );
        
        if( !aNode->GetUserDataPtr() ) {
            aNode->SetUserDataPtr( tc.get() );
        }
        
//        FbxAMatrix lGlobalPosition = GetGlobalPosition(aNode, FBXSDK_TIME_INFINITE, NULL );
//        tc->setLocalTransformMatrix(lGlobalPosition);
        
        FbxAMatrix lGlobalPosition = GetGlobalPosition(aNode, FBXSDK_TIME_INFINITE, NULL );
        if( !aNode->GetParent() ) {
            tc->setLocalTransformMatrix(lGlobalPosition);
        } else {
            FbxAMatrix lLocalPosition = GetLocalPositionForNode(aNode, FBXSDK_TIME_INFINITE, NULL );
            tc->setLocalTransformMatrix(lLocalPosition);
        }
        tc->cacheStartTransforms();
    }
    
    return tc;
}

//--------------------------------------------------------------
shared_ptr<ofxFBXSource::Node> Scene::_getNullForNode( FbxNode* aNode ) {
    shared_ptr<ofxFBXSource::Node> tn;
    if( aNode == NULL ) return tn;
    
//    if( aNode->GetNull() != NULL ) {
    tn = make_shared<ofxFBXSource::Node>();
    tn->setup(aNode);
    if( !aNode->GetUserDataPtr() ) {
        aNode->SetUserDataPtr( tn.get() );
    }
    
//    FbxAMatrix lGlobalPosition = GetGlobalPosition(aNode, FBXSDK_TIME_INFINITE, NULL );
//    tn->setLocalTransformMatrix(lGlobalPosition);
    
    FbxAMatrix lGlobalPosition = GetGlobalPosition(aNode, FBXSDK_TIME_INFINITE, NULL );
    if( !aNode->GetParent() ) {
        tn->setLocalTransformMatrix(lGlobalPosition);
    } else {
        FbxAMatrix lLocalPosition = GetLocalPositionForNode(aNode, FBXSDK_TIME_INFINITE, NULL );
        tn->setLocalTransformMatrix(lLocalPosition);
    }
    tn->cacheStartTransforms();
    
    return tn;
}

//--------------------------------------------------------------
shared_ptr<ofxFBXSource::Bone> Scene::_getBoneForNode( FbxNode* aNode ) {
    shared_ptr<ofxFBXSource::Bone> tbone;
    if( aNode == NULL ) return tbone;
    
    if ( !aNode->GetUserDataPtr() ) {
        tbone = make_shared<ofxFBXSource::Bone>();
//        bones.push_back( tbone );
        tbone->setup( aNode );
        aNode->SetUserDataPtr( tbone.get() );
    }
    
    return tbone;
}

//--------------------------------------------------------------
shared_ptr<ofxFBXSource::Node> Scene::_getOfxNodeFromNodeUserData( FbxNode* aNode ) {
    shared_ptr<ofxFBXSource::Node> fnode;
    if( aNode == NULL ) { return fnode; }
    
    ofxFBXSource::Node::NodeType ftype = getOFXNodeType( aNode );
    if(aNode->GetUserDataPtr() != NULL) {
        if (ftype == ofxFBXSource::Node::OFX_FBX_MESH) {
            ofxFBXSource::Mesh* nodePtr = static_cast<ofxFBXSource::Mesh *>(aNode->GetUserDataPtr());
            // try to find it in the meshes vector //
            for( auto& tm : mSrcNodes ) {
                if( tm.get() == nodePtr ) {
                    fnode = tm;
                    break;
                }
            }
//            auto tmesh = _getMeshForNode( aNode );
//                fnode = make_shared<ofxFBXMesh>(static_cast<ofxFBXMesh *>(aNode->GetUserDataPtr()));
//                std::shared_ptr<disk_node>* u_poi
//                = static_cast< std::shared_ptr<disk_node>* >(RayCallback.m_collisionObject->getUserPointer());
//                fnode = *static_cast< std::shared_ptr<ofxFBXMesh>* >(aNode->GetUserDataPtr());
        } else if(ftype == ofxFBXSource::Node::OFX_FBX_NURBS_CURVE) {
            ofxFBXSource::NurbsCurve* nodePtr = static_cast<ofxFBXSource::NurbsCurve *>(aNode->GetUserDataPtr());
            for( auto& tm : mSrcNodes ) {
                if( tm.get() == nodePtr ) {
                    fnode = tm;
                    break;
                }
            }
            
        } else if(ftype == ofxFBXSource::Node::OFX_FBX_NULL ) {
            ofxFBXSource::Node* nodePtr = static_cast<ofxFBXSource::Node *>(aNode->GetUserDataPtr());
            for( auto& tm : mSrcNodes ) {
                if( tm.get() == nodePtr ) {
                    fnode = tm;
                    break;
                }
            }
        } else if(ftype == ofxFBXSource::Node::OFX_FBX_SKELETON ) {
            ofxFBXSource::Bone* bonePtr = static_cast<ofxFBXSource::Bone *>(aNode->GetUserDataPtr());
            for( auto& tm : mSrcNodes ) {
                if( tm->getType() == ofxFBXSource::Node::OFX_FBX_SKELETON ) {
                    auto tskel = dynamic_pointer_cast<ofxFBXSource::Skeleton>(tm);
                    if( tskel->root.get() == bonePtr ) {
                        fnode = tm;
                        break;
                    }
                }
            }
        } else if(ftype == ofxFBXSource::Node::OFX_FBX_BONE ) {
            ofxFBXSource::Bone* bonePtr = static_cast<ofxFBXSource::Bone *>(aNode->GetUserDataPtr());
            for( auto& tm : mSrcNodes ) {
                if( tm.get() == bonePtr ) {
                    fnode = tm;
                    break;
                }
            }
        }
    }
    
    
    return fnode;
}

//--------------------------------------------------------------
void Scene::_parentNodesRecursive( FbxNode* aNode ) {
    if( aNode == NULL ) { return; }
    
//    cout << "_parentNodesRecursive " << mSceneNodes.size() << " name: " << aNode->GetName() << " type: " << getTypeStringFromNode(aNode) << endl;
//    if( aNode->GetParent() ) {
//        cout << "  -- parent name: " << aNode->GetParent()->GetName() << endl;
//    }
//    if(pNode->GetParent()) {
//        if(pNode->GetParent()->GetNodeAttribute()) {
//            if(pNode->GetParent()->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton) {
//                ofxFBXBone* bonePtr         = static_cast<ofxFBXBone *>(pNode->GetUserDataPtr());
//                ofxFBXBone* bonePtrParent   = static_cast<ofxFBXBone *>(pNode->GetParent()->GetUserDataPtr());
    
//    FbxNodeAttribute* lNodeAttribute = aNode->GetNodeAttribute();
    
    shared_ptr<ofxFBXSource::Node> cnode = _getOfxNodeFromNodeUserData( aNode );
    
//    if( cnode ) {
//
//    }
    
    if( aNode->GetParent() && aNode->GetParent() != lScene->GetRootNode() ) {
        shared_ptr<ofxFBXSource::Node> pnode = _getOfxNodeFromNodeUserData( aNode->GetParent() );
        if( pnode && cnode ) {
            pnode->addChild(cnode);
        }
    } else {
        if(cnode) {
            mSceneNodes.push_back( cnode );
        }
    }
    
    const int lChildCount = aNode->GetChildCount();
    for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex) {
        _parentNodesRecursive( aNode->GetChild(lChildIndex) );
    }
}

#pragma mark - Keyframes
//--------------------------------------------------------------
void Scene::populateKeyFrames( FbxNode* pNode, int aAnimIndex ) {
    
    if( !pNode ) return;
    
//    cout << pNode->GetName() << " - populateKeyFrames animation index : " << aAnimIndex << endl;
    
    // populate the position first //
    
    ofxFBXSource::Node* fnode = nullptr;
    
    ofxFBXSource::Node::NodeType ftype = getOFXNodeType( pNode );
    if( ftype != ofxFBXSource::Node::OFX_FBX_UNKNOWN && pNode->GetUserDataPtr() ) {
        if( ftype == ofxFBXSource::Node::OFX_FBX_BONE || ftype == ofxFBXSource::Node::OFX_FBX_SKELETON ) {
            ofxFBXSource::Bone* bonePtr = static_cast<ofxFBXSource::Bone *>(pNode->GetUserDataPtr());
            fnode = bonePtr;
        } else if( ftype == ofxFBXSource::Node::OFX_FBX_MESH ) {
            ofxFBXSource::Mesh* meshPtr = static_cast<ofxFBXSource::Mesh *>(pNode->GetUserDataPtr());
            fnode = meshPtr;
        }
    }
    
    if( !fnode ) {
        ofLogVerbose("Scene::populateKeyFrames : could not parse keyframes for node: ") << pNode->GetName();
    }
    
//    FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
//    if (lNodeAttribute) {
//        if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton) {
//            if ( pNode->GetUserDataPtr() ) {
//                ofxFBXSource::Bone* bonePtr = static_cast<ofxFBXSource::Bone *>(pNode->GetUserDataPtr());
//                fnode = bonePtr;
//            }
//        } else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh ||
//                   lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNurbs ||
//                   lNodeAttribute->GetAttributeType() == FbxNodeAttribute::ePatch ||
//                   lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNurbsSurface) {
//            if( pNode->GetUserDataPtr() ) {
//                ofxFBXSource::Mesh* meshPtr = static_cast<ofxFBXSource::Mesh *>(pNode->GetUserDataPtr());
//                fnode = meshPtr;
//            }
//        }
//    }
    
    if( fnode ) {
        bool bGrabGlobalTransform = false;
        FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
        if ( lNodeAttribute && lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton ) {
            if(pNode->GetSkeleton() && pNode->GetSkeleton()->IsSkeletonRoot() ) {
                bGrabGlobalTransform = true;
//                cout << "ofxFBXSrcScene :: skeleton global transform : " << pNode->GetName() << endl;
            }
        }
        if( ftype != ofxFBXSource::Node::OFX_FBX_SKELETON && ftype != ofxFBXSource::Node::OFX_FBX_BONE ) {
            if( !pNode->GetParent() ) {
                bGrabGlobalTransform = true;
            }
        }
        
        // figure out the parsing //
        ofxFBXAnimation& tanim = animations[ aAnimIndex ];
        ofxFBXSource::AnimKeyCollection& kcollection = fnode->getKeyCollection( aAnimIndex );
        
        
        FbxAnimCurve* lPosCurveX = pNode->LclTranslation.GetCurve(currentFbxAnimationLayer, FBXSDK_CURVENODE_COMPONENT_X);
        FbxAnimCurve* lPosCurveY = pNode->LclTranslation.GetCurve(currentFbxAnimationLayer, FBXSDK_CURVENODE_COMPONENT_Y);
        FbxAnimCurve* lPosCurveZ = pNode->LclTranslation.GetCurve(currentFbxAnimationLayer, FBXSDK_CURVENODE_COMPONENT_Z);
        auto posXKeys = getFloatKeys( lPosCurveX );
        auto posYKeys = getFloatKeys( lPosCurveY );
        auto posZKeys = getFloatKeys( lPosCurveZ );
        
        FbxAnimCurve* lScaleCurveX = pNode->LclScaling.GetCurve(currentFbxAnimationLayer, FBXSDK_CURVENODE_COMPONENT_X);
        FbxAnimCurve* lScaleCurveY = pNode->LclScaling.GetCurve(currentFbxAnimationLayer, FBXSDK_CURVENODE_COMPONENT_Y);
        FbxAnimCurve* lScaleCurveZ = pNode->LclScaling.GetCurve(currentFbxAnimationLayer, FBXSDK_CURVENODE_COMPONENT_Z);
        auto scaleXKeys = getFloatKeys( lScaleCurveX );
        auto scaleYKeys = getFloatKeys( lScaleCurveY );
        auto scaleZKeys = getFloatKeys( lScaleCurveZ );
        
        FbxAnimCurve* lRotCurveX = pNode->LclRotation.GetCurve(currentFbxAnimationLayer, FBXSDK_CURVENODE_COMPONENT_X);
        FbxAnimCurve* lRotCurveY = pNode->LclRotation.GetCurve(currentFbxAnimationLayer, FBXSDK_CURVENODE_COMPONENT_Y);
        FbxAnimCurve* lRotCurveZ = pNode->LclRotation.GetCurve(currentFbxAnimationLayer, FBXSDK_CURVENODE_COMPONENT_Z);
        
        auto rotXKeys = getFloatKeys( lRotCurveX );
        auto rotYKeys = getFloatKeys( lRotCurveY );
        auto rotZKeys = getFloatKeys( lRotCurveZ );
        
        int numPosKeys = 0;
        int numScaleKeys = 0;
        int numRotKeys = 0;
        
        for( int i = 0; i < tanim.getTotalNumFrames(); i++ ) {
            tanim.setFrame(i);
            
            signed long long cmillis = tanim.fbxCurrentTime.GetMilliSeconds();
            signed long long nmillis = tanim.fbxCurrentTime.GetMilliSeconds() + fbxFrameTime.GetMilliSeconds();
            
            // now get the information //
            bool bHasPosKey = false;
            bool bHasScaleKey = false;
            bool bHasRotKey = false;
            
            if( i == 0 || i == tanim.getTotalNumFrames()-1 ) {
                bHasPosKey = true; bHasScaleKey = true; bHasRotKey = true;
            }
            
            // check if there are no keys, if not, then force add some keys for proper animation //
            if( posXKeys.size() == 0 || posYKeys.size() == 0 || posZKeys.size() == 0 ) {
                bHasPosKey = true;
            }
            
            if( scaleXKeys.size() == 0 || scaleYKeys.size() == 0 || scaleZKeys.size() == 0 ) {
                bHasScaleKey = true;
            }
            
            if( rotXKeys.size() == 0 || rotYKeys.size() == 0 || rotZKeys.size() == 0 ) {
                bHasRotKey=true;
            }
            
            
            // check position //
            if( !bHasPosKey && requiresKeyForTime(posXKeys, cmillis, nmillis )) bHasPosKey=true;
            if( !bHasPosKey && requiresKeyForTime(posYKeys, cmillis, nmillis )) bHasPosKey=true;
            if( !bHasPosKey && requiresKeyForTime(posZKeys, cmillis, nmillis )) bHasPosKey=true;
            
            // check scale //
            if( !bHasScaleKey && requiresKeyForTime(scaleXKeys, cmillis, nmillis )) bHasScaleKey=true;
            if( !bHasScaleKey && requiresKeyForTime(scaleYKeys, cmillis, nmillis )) bHasScaleKey=true;
            if( !bHasScaleKey && requiresKeyForTime(scaleZKeys, cmillis, nmillis )) bHasScaleKey=true;
            
            // check rotation //
            if( !bHasRotKey && requiresKeyForTime(rotXKeys, cmillis, nmillis )) bHasRotKey=true;
            if( !bHasRotKey && requiresKeyForTime(rotYKeys, cmillis, nmillis )) bHasRotKey=true;
            if( !bHasRotKey && requiresKeyForTime(rotZKeys, cmillis, nmillis )) bHasRotKey=true;
            
//            cout << pNode->GetName() << " : " << i << " - " << bHasScaleKey << endl;

			glm::vec3 tpos, tscale;
			glm::quat tquat;
			//fbxToGlmComponents(FbxAMatrix& ainput, glm::vec3& apos, glm::quat& aquat, glm::vec3& ascale)

//            if( pNode->GetParent() ) {
            if( bHasPosKey || bHasScaleKey || bHasRotKey ) {
                
                if( !bGrabGlobalTransform ) {
                    //setTransformMatrix( ofGetLocalTransform( fbxNode, pTime, pPose, NULL ));
                    FbxAMatrix& tmatrix = pNode->EvaluateLocalTransform( tanim.fbxCurrentTime );
                    //tmat = ( fbxToOf(tmatrix) );
                    fbxToGlmComponents(tmatrix, tpos, tquat, tscale);
                } else {
                    FbxAMatrix& tmatrix = pNode->EvaluateGlobalTransform( tanim.fbxCurrentTime );
                   // tmat = ( fbxToOf(tmatrix) );
                    fbxToGlmComponents(tmatrix, tpos, tquat, tscale);
                }
                
                if( bHasPosKey ) {
                    ofxFBXSource::AnimKey<float> tkeyPosX;
                    tkeyPosX.millis = tanim.fbxCurrentTime.GetMilliSeconds();
                    tkeyPosX.value = tpos.x;
                    kcollection.posKeysX.push_back( tkeyPosX );
                    ofxFBXSource::AnimKey<float> tkeyPosY;
                    tkeyPosY.millis = tanim.fbxCurrentTime.GetMilliSeconds();
                    tkeyPosY.value = tpos.y;
                    kcollection.posKeysY.push_back( tkeyPosY );
                    ofxFBXSource::AnimKey<float> tkeyPosZ;
                    tkeyPosZ.millis = tanim.fbxCurrentTime.GetMilliSeconds();
                    tkeyPosZ.value = tpos.z;
                    kcollection.posKeysZ.push_back( tkeyPosZ );
                    
                    numPosKeys++;
                }
                
                if( bHasScaleKey ) {
                    ofxFBXSource::AnimKey<float> tkeyScaleX;
                    tkeyScaleX.millis = tanim.fbxCurrentTime.GetMilliSeconds();
                    tkeyScaleX.value = tscale.x;
                    kcollection.scaleKeysX.push_back( tkeyScaleX );
                    ofxFBXSource::AnimKey<float> tkeyScaleY;
                    tkeyScaleY.millis = tanim.fbxCurrentTime.GetMilliSeconds();
                    tkeyScaleY.value = tscale.y;
                    kcollection.scaleKeysY.push_back( tkeyScaleY );
                    ofxFBXSource::AnimKey<float> tkeyScaleZ;
                    tkeyScaleZ.millis = tanim.fbxCurrentTime.GetMilliSeconds();
                    tkeyScaleZ.value = tscale.z;
                    kcollection.scaleKeysZ.push_back( tkeyScaleZ );
                    
                    numScaleKeys++;
                }
                
                if( bHasRotKey ) {
                    ofxFBXSource::AnimKey<ofQuaternion> tRotKey;
                    tRotKey.millis = tanim.fbxCurrentTime.GetMilliSeconds();
                    tRotKey.value = tquat;
                    kcollection.rotKeys.push_back( tRotKey );
                    
                    numRotKeys++;
                }
                
            }
            
        }
        
        ofLogVerbose("ofxFBXScene :: ") << fnode->getName() << " num pos keys: " << numPosKeys << " scale: " << numScaleKeys << " rot: " << numRotKeys << " total: " << tanim.getTotalNumFrames();
        
        // rotation //
//        FbxAnimCurve* lRotCurveX = pNode->LclRotation.GetCurve(currentFbxAnimationLayer, FBXSDK_CURVENODE_COMPONENT_X);
//        FbxTime lKeyTime;
//        int lCount, lKeyCount;
//        if(lRotCurveX) {
//            lKeyCount = lRotCurveX->KeyGetCount();
//            cout << "ofxFBXScene :: PopulateKeyFrames " << fnode->getName() << " num keys: " << lKeyCount << endl;
//        }
        
        
//        cout << "ofxFBXScene :: adding keyframes for : " << pNode->GetName() << " is root: " << (fnode->isRoot()) << " num pos keys: " << kcollection.posKeysX.size() << endl;
        
    }
    
    
    
//    if( fnode && false ) {
//        FbxAnimCurve* lAnimCurveX = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
//        FbxAnimCurve* lAnimCurveY = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
//        FbxAnimCurve* lAnimCurveZ = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
//
//        ofxFBXKeyCollection& kcollection = fnode->getKeyCollection( aAnimIndex );
//        kcollection.posKeysX = getFloatKeys( lAnimCurveX );
//        kcollection.posKeysY = getFloatKeys( lAnimCurveY );
//        kcollection.posKeysZ = getFloatKeys( lAnimCurveZ );
//
//        FbxAnimCurve* lScaleCurveX = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
//        FbxAnimCurve* lScaleCurveY = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
//        FbxAnimCurve* lScaleCurveZ = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
//        kcollection.scaleKeysX = getFloatKeys( lScaleCurveX );
//        kcollection.scaleKeysY = getFloatKeys( lScaleCurveY );
//        kcollection.scaleKeysZ = getFloatKeys( lScaleCurveZ );
//
//
//        // rotation //
//        FbxAnimCurve* lRotCurveX = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
//        FbxAnimCurve* lRotCurveY = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
//        FbxAnimCurve* lRotCurveZ = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
//
//        map< signed long, int > mTimeCount;
//        vector< ofxFBXKey<ofQuaternion> > newRotKeys;
//        int xKeyCount = lRotCurveX ? lRotCurveX->KeyGetCount() : 0;
//        int yKeyCount = lRotCurveY ? lRotCurveY->KeyGetCount() : 0;
//        int zKeyCount = lRotCurveZ ? lRotCurveZ->KeyGetCount() : 0;
//
//        FbxTime lKeyTime;
//        int lCount, lKeyCount;
//        if(lRotCurveX) {
//            lKeyCount = lRotCurveX->KeyGetCount();
//            for(lCount = 0; lCount < lKeyCount; lCount++) {
//                lKeyTime  = lRotCurveX->KeyGetTime(lCount);
//                if( mTimeCount.count(lKeyTime.GetMilliSeconds()) == 0 ) {
//                    mTimeCount[lKeyTime.GetMilliSeconds()] += 1;
//
//                    ofxFBXKey<ofQuaternion> tkey;
//                    //glm::mat4 tOfMat;
//                    glm::vec3 translation, scale;
//                    glm::quat rotation;
//                    if( pNode->GetParent() ) {
//                        FbxAMatrix& tmatrix = pNode->EvaluateLocalTransform( lKeyTime );
//                        //tOfMat = fbxToOf( tmatrix );
//                        fbxToGlmComponents(tmatrix, translation, rotation, scale);
//                    } else {
//                        FbxAMatrix& tmatrix = pNode->EvaluateGlobalTransform( lKeyTime );
//                        //tOfMat = fbxToOf( tmatrix );
//                        fbxToGlmComponents(tmatrix, translation, rotation, scale);
//                    }
//
//                    //glm::vec3 scale;
//                   // glm::quat rotation;
//                   // glm::vec3 translation;
//                    //glm::vec3 skew;
//                    //glm::vec4 perspective;
//
//                    //glm::decompose(tOfMat, scale, rotation, translation, skew, perspective);
//
//                    tkey.value = rotation;
////                    ofVec3f t,s;
////                    ofQuaternion so;
////
////                    tOfMat.decompose( t, tkey.value, s, so);
//
//                    tkey.millis = lKeyTime.GetMilliSeconds();
//                    newRotKeys.push_back( tkey );
//                }
//            }
//        }
//
//        if( newRotKeys.size() ) {
//            kcollection.rotKeys = newRotKeys;
//        }
//
//        ofLogVerbose("ofxFBX :: populateKeyFrames : ") << "anim: " << aAnimIndex << " node: " << fnode->getName() << " num pos keys: " << kcollection.posKeysX.size() << " num rot keys: " << kcollection.rotKeys.size() << " num scale keys: " << kcollection.scaleKeysX.size() << " | " << ofGetFrameNum() << endl;
//    }
    
    // cycle through the children //
    const int lChildCount = pNode->GetChildCount();
    for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex) {
        populateKeyFrames( pNode->GetChild(lChildIndex), aAnimIndex );
    }
}

//--------------------------------------------------------------
vector< ofxFBXSource::AnimKey<float> > Scene::getFloatKeys( FbxAnimCurve* pCurve ) {
    vector< ofxFBXSource::AnimKey<float> > tkeys;
    
    FbxTime lKeyTime;
    float lKeyValue;
    int lKeyCount, lCount;
    
    if( pCurve ) {
        lKeyCount = pCurve->KeyGetCount();
        for(lCount = 0; lCount < lKeyCount; lCount++) {
            lKeyValue = static_cast<float>(pCurve->KeyGetValue(lCount));
            lKeyTime  = pCurve->KeyGetTime(lCount);
            ofxFBXSource::AnimKey<float> tkey;
            tkey.value = lKeyValue;
            tkey.millis = lKeyTime.GetMilliSeconds();
            tkeys.push_back( tkey );
        }
    }
    
    return tkeys;
}

//--------------------------------------------------------------
bool Scene::requiresKeyForTime( vector< ofxFBXSource::AnimKey<float> >& tkeys, signed long amillis1, signed long amillis2 ) {
    for( int i = 0; i < tkeys.size(); i++ ) {
        if(tkeys[i].millis >= amillis1 && tkeys[i].millis < amillis2 ) {
            return true;
        }
    }
    return false;
}














