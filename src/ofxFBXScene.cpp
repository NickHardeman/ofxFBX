//
//  ofxFBXScene.h
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 10/31/13.
//
//

#include "ofxFBXScene.h"
#include "Common/Common.h"

//--------------------------------------------------------------
ofxFBXScene::ofxFBXScene() {
    lSdkManager = NULL;
    lScene      = NULL;
    currentFbxAnimationLayer = NULL;
    fbxFilePath = "";
}

//--------------------------------------------------------------
ofxFBXScene::~ofxFBXScene() {
    
    if(lScene != NULL ) {
        deleteCachedTexturesInScene( lScene );
        deleteCachedMaterialsRecursive( lScene->GetRootNode() );
    }
    
    if(lSdkManager != NULL) {
        DestroySdkObjects(lSdkManager, true);
        lSdkManager = NULL;
    }
}

//--------------------------------------------------------------
bool ofxFBXScene::load( string path, ofxFBXSceneSettings aSettings ) {
    bool lResult;
    
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
    (*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_TEXTURE,         _settings.importTextures );
    (*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_LINK,            _settings.importBones );
    (*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_SHAPE,           _settings.importShapes );
    (*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_GOBO,            _settings.importGobos );
    (*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_ANIMATION,       _settings.importAnimations );
    
	lResult = LoadScene(lSdkManager, lScene, ofToDataPath(path).c_str());
    
	if(!lResult || !lScene ) {
		ofLogError("ofxFBXScene") << "An error occurred while loading the scene... :(";
        return false;
	}
    
    fbxFilePath = ofToDataPath(path);
    
    // Convert Axis System to what is used in this example, if needed
    FbxAxisSystem SceneAxisSystem = lScene->GetGlobalSettings().GetAxisSystem();
//    FbxAxisSystem OurAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
    FbxAxisSystem OurAxisSystem( FbxAxisSystem::OpenGL );
    if( SceneAxisSystem != OurAxisSystem ) {
        OurAxisSystem.ConvertScene(lScene);
    }
    
    // Convert Unit System to what is used in this example, if needed
    FbxSystemUnit SceneSystemUnit = lScene->GetGlobalSettings().GetSystemUnit();
    if( SceneSystemUnit.GetScaleFactor() != 1.0 ) {
        //The unit in this example is centimeter.
        FbxSystemUnit::cm.ConvertScene( lScene );
    }
    
//    FbxGeometryConverter lGeomConverter( lSdkManager );
//    lGeomConverter.Triangulate(lScene, /*replace*/true);
    
    // Split meshes per material, so that we only have one material per mesh (for VBO support)
//    if(!lGeomConverter.SplitMeshesPerMaterial(lScene, /*replace*/true)) {
//        cout << "There was an error splitting the meshes per material" << endl;
//    }
    
    // cache all of the textures by loading them from disk //
    if(_settings.importTextures) {
        cacheTexturesInScene( lScene );
    }
    
    // cache all of the materials in the scene as ofxFBXMeshMaterial //
    if(_settings.importMaterials) {
        cacheMaterialsRecursive( lScene->GetRootNode() );
    }
    
//    lScene->GetGlobalSettings().SetTimeMode( FbxTime::eCustom );
//    lScene->GetGlobalSettings().SetTimeMode( FbxTime::eCustom );
    // Initialize the frame period.
    fbxFrameTime.SetTime(0, 0, 0, 1, 0, lScene->GetGlobalSettings().GetTimeMode());
//    fbxFrameTime.Set( lScene->GetGlobalSettings().GetFrameRate() );
//    fbxFrameTime.SetTime(0, 0, 0, 1, 0, FbxTime::eFrames24 );
//    fbxFrameTime.SetSecondDouble( 1.f / 24.f );
    cout << "time mode: " << lScene->GetGlobalSettings().GetTimeMode() << endl;
    
    // Get the list of all the animation stack.
    if( areAnimationsEnabled() ) {
        populateAnimationInformation();
    }
    
    if(animations.size() > 0) {
        FbxAnimStack * lCurrentAnimationStack = lScene->FindMember<FbxAnimStack>( (&animations[0].fbxname)->Buffer());
        if (lCurrentAnimationStack == NULL) {
            cout << "this is a problem. The anim stack should be found in the scene!" << endl;
            // this is a problem. The anim stack should be found in the scene!
            return false;
        }
        
        // we assume that the first animation layer connected to the animation stack is the base layer
        // (this is the assumption made in the FBXSDK)
        currentFbxAnimationLayer = lCurrentAnimationStack->GetMember<FbxAnimLayer>();
    }
    
    // populate meshes //
    populateMeshesRecursive( lScene->GetRootNode(), currentFbxAnimationLayer );
    
    // populate bones //
    if( _settings.importBones ) {
        constructSkeletons( lScene->GetRootNode(), currentFbxAnimationLayer );
    }
    
    return true;
}


#pragma mark - Getters
// ---------------------------------------------- getters
FbxScene* ofxFBXScene::getFBXScene() {
    return lScene;
}

//--------------------------------------------------------------
vector< shared_ptr<ofxFBXMesh> >& ofxFBXScene::getMeshes() {
	return meshes;
}
//
//vector<ofxFBXSkeleton*>& ofxFBXScene::getSkeletons() {
//	return skeletons;
//}

#pragma mark - Utils
// ---------------------------------------------- utils
string ofxFBXScene::getFbxFilePath() {
    return fbxFilePath;
}

//--------------------------------------------------------------
string ofxFBXScene::getFbxFolderPath() {
    if(fbxFilePath == "") return "";
    const FbxString lAbsFbxFileName = FbxPathUtils::Resolve( getFbxFilePath().c_str() );
    return  (string)FbxPathUtils::GetFolderName(lAbsFbxFileName);
}

#pragma mark - Textures
// ---------------------------------------------- animation
void ofxFBXScene::cacheTexturesInScene( FbxScene* pScene ) {
    // Load the textures into GPU, only for file texture now
    const int lTextureCount = pScene->GetTextureCount();
    for (int lTextureIndex = 0; lTextureIndex < lTextureCount; ++lTextureIndex)
    {
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
                ofLogWarning("Could not find texture ") << filepath;
                continue;
            }
            
            if (bFoundTexture) {
                ofTexture* texture = new ofTexture();
                ofPixels pixels;
                bool loaded = ofLoadImage(pixels, filepath);
                if(loaded){
                    texture->allocate(pixels.getWidth(), pixels.getHeight(), ofGetGlInternalFormat(pixels), false);
                    pixels.mirror(true, false);
                    texture->loadData(pixels);
                }
                if(loaded) {
                    ofLogVerbose("Loaded the texture from ") << filepath << endl;
                    
                    texture->getTextureData().bFlipTexture = true;
                    texture->disableTextureMatrix();
                    
                    lFileTexture->SetUserDataPtr( texture );
                } else {
                    ofLogWarning("Failed to load texture for ") << lFileName;
                    delete texture;
                    texture = NULL;
                }
            }
        }
    }
}

void ofxFBXScene::deleteCachedTexturesInScene( FbxScene* pScene ) {
    const int lTextureCount = pScene->GetTextureCount();
    for (int lTextureIndex = 0; lTextureIndex < lTextureCount; ++lTextureIndex) {
        FbxTexture* lTexture = pScene->GetTexture(lTextureIndex);
        FbxFileTexture* lFileTexture = FbxCast<FbxFileTexture>(lTexture);
        if (lFileTexture && lFileTexture->GetUserDataPtr()) {
            ofTexture* texture = static_cast<ofTexture *>( lFileTexture->GetUserDataPtr() );
            lFileTexture->SetUserDataPtr(NULL);
            delete texture;
            texture = NULL;
        }
    }
}

bool ofxFBXScene::isValidTexturePath( string aPathToTexture ) {
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
void ofxFBXScene::cacheMaterialsRecursive( FbxNode* pNode ) {
    // Bake material and hook as user data.
    const int lMaterialCount = pNode->GetMaterialCount();
    for (int lMaterialIndex = 0; lMaterialIndex < lMaterialCount; ++lMaterialIndex) {
        FbxSurfaceMaterial * lMaterial = pNode->GetMaterial(lMaterialIndex);
        if (lMaterial && !lMaterial->GetUserDataPtr()) {
            ofxFBXMeshMaterial* materialCache = new ofxFBXMeshMaterial();
            materialCache->setup( lMaterial );
            lMaterial->SetUserDataPtr( materialCache );
        }
    }
    
    const int lChildCount = pNode->GetChildCount();
    for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex) {
        cacheMaterialsRecursive( pNode->GetChild(lChildIndex) );
    }
}

void ofxFBXScene::deleteCachedMaterialsRecursive( FbxNode* pNode ) {
    const int lMaterialCount = pNode->GetMaterialCount();
    for (int lMaterialIndex = 0; lMaterialIndex < lMaterialCount; ++lMaterialIndex) {
        FbxSurfaceMaterial * lMaterial = pNode->GetMaterial(lMaterialIndex);
        if (lMaterial && lMaterial->GetUserDataPtr()) {
            ofxFBXMeshMaterial* materialCache = static_cast<ofxFBXMeshMaterial *>(lMaterial->GetUserDataPtr());
            lMaterial->SetUserDataPtr(NULL);
            delete materialCache;
        }
    }
    
    const int lChildCount = pNode->GetChildCount();
    for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex) {
        deleteCachedMaterialsRecursive( pNode->GetChild(lChildIndex) );
    }
}

#pragma mark - Animation
// ---------------------------------------------- animation
int ofxFBXScene::getNumAnimations() {
    return animations.size();
}

//--------------------------------------------------------------
bool ofxFBXScene::areAnimationsEnabled() {
    return _settings.importAnimations;
}

//--------------------------------------------------------------
void ofxFBXScene::populateAnimationInformation() {
    FbxArray<FbxString*> mAnimStackNameArray;
    lScene->FillAnimStackNameArray(mAnimStackNameArray);
    animations.resize( mAnimStackNameArray.GetCount() );
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
        animations[i].setup( startTime, endTime, fbxFrameTime );
        animations[i].fbxCurrentTime = animations[i].fbxStartTime;
        FbxString tfbxName      = FbxString(*mAnimStackNameArray[i]);
        animations[i].fbxname   = tfbxName;
        animations[i].ID        = i;
        animations[i].name      = FbxString(*mAnimStackNameArray[i]);
//        cout << i << " - ofxFBXScene :: name " << animations[i].name << "| fbxname: " << FbxString(*mAnimStackNameArray[i]) << endl;
    }
    
    FbxArrayDelete(mAnimStackNameArray);
}

//--------------------------------------------------------------
void ofxFBXScene::populateAnimations( vector<ofxFBXAnimation>& aInVector) {
    for(int i = 0; i < animations.size(); i++ ) {
        aInVector.push_back(animations[i]);
    }
}

//--------------------------------------------------------------
void ofxFBXScene::populateSkeletons( vector< shared_ptr<ofxFBXSkeleton> >& aInSkeletons ) {
    aInSkeletons.resize( skeletons.size() );
    for( int i = 0; i < skeletons.size(); i++ ) {
        ofxFBXSkeleton tskeleton = *skeletons[i].get();
        aInSkeletons[i] = shared_ptr<ofxFBXSkeleton>( new ofxFBXSkeleton(tskeleton) );
        // re-do the node parenting so that it doesn't mess with it every skeleton instance //
        // the pointers will get changed for every ofxFBXManager instance //
        aInSkeletons[i]->reconstructNodeParenting();
    }
}

#pragma mark - Meshes
// ---------------------------------------------- meshes
void ofxFBXScene::populateMeshesRecursive( FbxNode* pNode, FbxAnimLayer* pAnimLayer ) {
    FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
//    cout << "populatesMeshesRecursive :: " << ofGetElapsedTimef() << endl;
    if (lNodeAttribute) {
        // Bake mesh as VBO(vertex buffer object) into GPU.
        if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh || lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNurbs || lNodeAttribute->GetAttributeType() == FbxNodeAttribute::ePatch || lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNurbsSurface) {
            
            FbxGeometryConverter lGeomConverter( lSdkManager );
            lGeomConverter.Triangulate( pNode->GetMesh(), true );
            
            if(pNode->GetMesh()) {
//                meshesList.push_back(ofxFBXMesh());
//                ofxFBXMesh & mesh = meshesList.back();
                meshes.push_back( shared_ptr<ofxFBXMesh>( new ofxFBXMesh() ));
                shared_ptr<ofxFBXMesh> mesh = meshes.back();
                mesh->setup( pNode );
                
//            cout << "ofxFBXScene :: populateMeshesRecursive : name = " << pNode->GetNameOnly() << endl;
                
                FbxAMatrix lGlobalPosition = GetGlobalPosition(pNode, FBXSDK_TIME_INFINITE, NULL );
                
                ofMatrix4x4 ofgpos = toOf(lGlobalPosition);
                mesh->setTransformMatrix( ofgpos );
                
                // Associtate the clusters with some user data so that we can move them around //
                int lSkinCount = pNode->GetMesh()->GetDeformerCount(FbxDeformer::eSkin);
                
                if( lSkinCount > 0 && _settings.importBones ) {
                
                    for ( int lSkinIndex=0; lSkinIndex<lSkinCount; ++lSkinIndex) {
                        FbxSkin * lSkinDeformer = (FbxSkin *)pNode->GetMesh()->GetDeformer(lSkinIndex, FbxDeformer::eSkin);
                        
                        int lClusterCount = lSkinDeformer->GetClusterCount();
                        for ( int lClusterIndex=0; lClusterIndex<lClusterCount; ++lClusterIndex) {
                            FbxCluster* lCluster = lSkinDeformer->GetCluster(lClusterIndex);
                            if (!lCluster->GetLink())
                                continue;
                            
                            if ( !lCluster->GetUserDataPtr() ) {
                                clusters.push_back( shared_ptr<ofxFBXCluster>( new ofxFBXCluster() ));
                                ofxFBXCluster* clusterPtr = clusters.back().get();
                                clusterPtr->setup( lGlobalPosition, pNode->GetMesh(), lCluster );
                                lCluster->SetUserDataPtr( clusterPtr );
                            }
                        }
                    }
                }
            }
        }
    }
    
    const int lChildCount = pNode->GetChildCount();
    for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex) {
        populateMeshesRecursive( pNode->GetChild(lChildIndex), pAnimLayer );
    }
}

#pragma mark - Bones / Skeleton
// ---------------------------------------------- bones
void ofxFBXScene::populateBonesRecursive( FbxNode* pNode, FbxAnimLayer* pAnimLayer ) {
    FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
    if (lNodeAttribute) {
        if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton) {
            if ( !pNode->GetUserDataPtr() ) {
                bones.push_back( shared_ptr<ofxFBXBone>( new ofxFBXBone() ));
                ofxFBXBone* bonePtr = bones.back().get();
                bonePtr->setup( pNode );
                pNode->SetUserDataPtr( bonePtr );
            }
        }
    }
    const int lChildCount = pNode->GetChildCount();
    for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex) {
        populateBonesRecursive( pNode->GetChild(lChildIndex), pAnimLayer );
    }
}

// lets just be safe and parent the nodes after we have gone through and assigned them all
// with their user data, to make sure all of them have been assigned
//--------------------------------------------------------------
void ofxFBXScene::parentBonesRecursive( FbxNode* pNode, list<FbxNode*>& aSkeletonBases, int aBoneLevel ) {
    FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
    if (lNodeAttribute) {
        if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton) {
            if(pNode->GetParent()) {
                if(pNode->GetParent()->GetNodeAttribute()) {
                    if(pNode->GetParent()->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton) {
                        ofxFBXBone* bonePtr         = static_cast<ofxFBXBone *>(pNode->GetUserDataPtr());
                        ofxFBXBone* bonePtrParent   = static_cast<ofxFBXBone *>(pNode->GetParent()->GetUserDataPtr());
                        bonePtr->parentBoneName     = bonePtrParent->getName();
                        bonePtr->setParent( *bonePtrParent, true );
                        bonePtr->setScale( 1.f );
//                        bonePtr->level = aBoneLevel;
                        
//                        if( bonePtrParent ) {
//                            bonePtrParent->bones.push_back( shared_ptr< ofxFBXBone >( new ofxFBXBone( *bonePtr )));
//                        }
//                        bonePtr->setGlobalPosition( bonePtr->origNode.getGlobalPosition() );
//                        bonePtr->setGlobalOrientation( bonePtr->origNode.getGlobalOrientation() );
//                        origNode.setTransformMatrix( ofGetGlobalTransform( fbxNode, FBXSDK_TIME_INFINITE, NULL ) );
                        ofNode tnode;
                        tnode.setTransformMatrix( ofGetGlobalTransform( bonePtr->fbxNode, FBXSDK_TIME_INFINITE, NULL ));
                        bonePtr->setGlobalPosition( tnode.getGlobalPosition() );
                        bonePtr->setGlobalOrientation( tnode.getGlobalOrientation() );
                        bonePtr->cacheStartTransforms();
                        
//                        cout << "parentBonesRecursive :: " << bonePtr->getName() << " num character link count = " <<  pNode->GetCharacterLinkCount() << endl;;
                    } else {
//                        cout << "found a skeleton " << endl;
//                        aSkeletonBases.push_back( pNode );
                    }
                } else {
//                    aSkeletonBases.push_back( pNode );
                }
            } else {
//                cout << "we found a skeleton " << endl;
//                ofxFBXBone* bonePtr         = static_cast<ofxFBXBone *>(pNode->GetUserDataPtr());
//                aSkeletonBases.push_back( pNode );
//                aSkeletonBases[pNode] = bonePtr;
            }
//            if(IsNodeSingleChainIKEffector(pNode)) {
//                cout << "parentBonesRecursive :: " << pNode->GetName() << " is an IKKKKK! " << endl;
//            }
            
//            cout << "pNode->Skeleton type = " << pNode->GetSkeleton()->GetSkeletonType() << endl;
            
//            if(pNode->GetSkeleton()->GetSkeletonType() == FbxSkeleton::eRoot || pNode->GetSkeleton()->GetSkeletonType() == FbxSkeleton::eEffector) {
            if(pNode->GetSkeleton()->IsSkeletonRoot() ) {
//                cout << "parentBonesRecursive :: " << pNode->GetName() << " is the ROOT! " << endl;
                aSkeletonBases.push_back( pNode );
            }
        }
    }
    const int lChildCount = pNode->GetChildCount();
    for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex) {
        parentBonesRecursive( pNode->GetChild(lChildIndex), aSkeletonBases, aBoneLevel+1 );
    }
}

//--------------------------------------------------------------
void ofxFBXScene::constructSkeletons( FbxNode* pNode, FbxAnimLayer* pAnimLayer ) {
    populateBonesRecursive( pNode, pAnimLayer);
    list<FbxNode*> skeletonBases;
    
    parentBonesRecursive( pNode, skeletonBases, 0 );
    list<FbxNode*>::iterator it;
    for(it = skeletonBases.begin(); it != skeletonBases.end(); ++it ) {
//        skeletonList.push_back( ofxFBXSkeleton() );
//        ofxFBXSkeleton* skeletonPtr = &skeletonList.back();
        shared_ptr<ofxFBXSkeleton> skeletonPtr( new ofxFBXSkeleton() );
        skeletons.push_back( skeletonPtr );
        FbxNode* skeletonNode = *it;
        skeletonPtr->setup( skeletonNode );
//        skeletonPtr->parentBonesRecursive( skeletonNode );
//        skeletonPtr->bones.push_back( );
        
//        skeletonPtr->sourceBones[ skeletonNode->GetName() ] = static_cast<ofxFBXBone *>( skeletonNode->GetUserDataPtr() );
        skeletonPtr->rootSource = static_cast<ofxFBXBone *>( skeletonNode->GetUserDataPtr() );
//        constructSkeletonsRecursive( skeletonPtr.get(), skeletonNode, 0 );
        constructSkeletonsRecursive( skeletonPtr.get(), skeletonNode, 0 );
    }
    
}

//--------------------------------------------------------------
void ofxFBXScene::constructSkeletonsRecursive( ofxFBXSkeleton* aSkeleton, FbxNode* pNode, int aBoneLevel ) {
    FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
    if (lNodeAttribute) {
        if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton) {
            if(pNode->GetParent()) {
                if(pNode->GetParent()->GetNodeAttribute()) {
                    if(pNode->GetParent()->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton) {
                        if(pNode->GetParent()->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton) {
                            ofxFBXBone* bonePtr         = static_cast<ofxFBXBone *>(pNode->GetUserDataPtr());
                            ofxFBXBone* bonePtrParent   = static_cast<ofxFBXBone *>(pNode->GetParent()->GetUserDataPtr());
                            
                            if( bonePtrParent ) {
                                bonePtrParent->sourceBones[ bonePtr->getName() ] = bonePtr;
                            }
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
int ofxFBXScene::getNumPoses() {
    if( lScene == NULL ) return 0;
    return lScene->GetPoseCount();
}

//--------------------------------------------------------------
void ofxFBXScene::populatePoses( vector< shared_ptr<ofxFBXPose> >& aInPoses ) {
    
    for( int i = 0; i < lScene->GetPoseCount(); i++ ) {
        aInPoses.push_back( shared_ptr<ofxFBXPose>( new ofxFBXPose() ));
        shared_ptr< ofxFBXPose > pose = aInPoses.back();
        pose->setup( lScene->GetPose(i), i );
    }
    
}













