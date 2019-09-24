//
//  ofxFBXManager.cpp
//  ScenesSetup_Nick
//
//  Created by Nick Hardeman on 12/20/13.
//
//

#include "ofxFBX.h"

ofxFBXAnimation dummyAnimation;

vector< shared_ptr<ofxFBXSource::Scene> > ofxFBX::mCachedScenes;

//--------------------------------------------------------------
ofxFBX::ofxFBX() {
    bAnimationsEnabled              = true;
    animationIndex                  = 0;
    dummyAnimation.name             = "dummy";
    poseIndex                       = 0;
    bPosesEnabled                   = false;
    currentAnimationStack           = NULL;
}

//--------------------------------------------------------------
ofxFBX::~ofxFBX() {
    clear();
//    for( int i = 0; i < mSkeletons.size(); i++ ) {
//        mSkeletons[i]->clearParent();
////        skeletons[i]->root.clearParent();
//    }
//    mSkeletons.clear();
//
//    if( mSceneInternal ) {
//        if( fbxScene != NULL && fbxScene->getSettings().unloadIfUnused ) {
//            mSceneInternal.reset();
//            ofRemove( mCachedScenes, shouldRemoveScene );
//
//            ofLogVerbose("~ofxFBXManager :: mCachedScenes : size(): ") << mCachedScenes.size();
//        }
//    }
}

//--------------------------------------------------------------
bool ofxFBX::shouldRemoveScene( const shared_ptr<ofxFBXSource::Scene>& ascene ) {
    return (!ascene || ascene.use_count() <= 1); // only stored in the cache //
}

//--------------------------------------------------------------
bool ofxFBX::load(ofxFBXSource::Scene::Settings aSettings ) {
    return load( aSettings.filePath, aSettings );
}

//--------------------------------------------------------------
void ofxFBX::clear() {
    for( auto node : mAllNodes ) {
        node->clearParent();
    }
    
    mSkeletons.clear();
    poses.clear();
    meshes.clear();
    mNurbsCurves.clear();
    mNullNodes.clear();
    animations.clear();
    mRootNodes.clear();
    mAllNodes.clear();
    
//    for( int i = 0; i < mSkeletons.size(); i++ ) {
//        mSkeletons[i]->clearParent();
////        skeletons[i]->root.clearParent();
//    }
//    mSkeletons.clear();
    
    if( mSceneInternal ) {
        if( fbxScene != NULL && fbxScene->getSettings().unloadIfUnused ) {
            mSceneInternal.reset();
            ofRemove( mCachedScenes, shouldRemoveScene );
            ofLogVerbose("~ofxFBXManager :: mCachedScenes : size(): ") << mCachedScenes.size();
        }
    }
}

//--------------------------------------------------------------
bool ofxFBX::load( string path, ofxFBXSource::Scene::Settings aSettings ) {
    
    if( !ofFile::doesFileExist(path)) {
        ofLogError("ofxFBX::load()") << " File does not exist!!: " << path << " :(";
        return false;
    }
    
    // check to see if we already have a scene cached //
    string fpath = ofToDataPath(path);
    if( mSceneInternal && mSceneInternal->getFbxFilePath() == fpath ) {
        ofLogNotice("ofxFBX::load() already have this scene loaded") << mSceneInternal->getFbxFilePath();
        return true;
    }
    
    // clean up the internal scene if it is present //
    clear();
    
    for( auto scene : mCachedScenes ) {
        if( scene->getFbxFilePath() == fpath ) {
            mSceneInternal = scene;
            break;
        }
    }
    if( !mSceneInternal ) {
        mSceneInternal = make_shared<ofxFBXSource::Scene>();
        if( mSceneInternal->load( path, aSettings )) {
            mCachedScenes.push_back( mSceneInternal );
        } else {
            mSceneInternal.reset();
        }
    }
//    for( int i = 0; i < mCachedScenes.size(); i++ ) {
//        cout << i << " - " << "ofxFBXManager :: load : " << mCachedScenes[i]->getFbxFilePath() << endl;
//    }
    
    
    if( mSceneInternal ) {
        setup( mSceneInternal.get() );
        return true;
    }
    
    return false;
}

//--------------------------------------------------------------
void ofxFBX::setup( ofxFBXSource::Scene* aScene ) {
    fbxScene = aScene;
    if(fbxScene == NULL ) {
        ofLogError("ofxFBXManager::setup : ofxFBXScene is NULL, make sure set and load the ofxFBXScene!");
        return;
    }
    if(fbxScene->getFBXScene() == NULL) {
        ofLogWarning("ofxFBXManager::setup : FbxScene is NULL, make sure to not unload the ofxFBXScene!");
        return;
    }
    
//    cout << "ofxFBX::setup : parsing ofxFBXSource :: " << fbxScene->getFbxFilePath() << endl;
    
    // clear out any existing data //
    mSkeletons.clear();
    poses.clear();
    meshes.clear();
    mNurbsCurves.clear();
    mNullNodes.clear();
    animations.clear();
    mRootNodes.clear();
    mAllNodes.clear();
    
    // see if we are using keyframes //
    bUsingKeyframes = fbxScene->getSettings().useKeyFrames;
    
    aScene->populateAnimations( animations );
    
    // we need to parse the nodes from the scene to preserve the hierarchy //
    vector< shared_ptr<ofxFBXSource::Node> > fsceneNodes = fbxScene->getSceneNodes();
    shared_ptr<ofxFBXNode> tempParentNode;
    for( auto snode : fsceneNodes ) {
        _parseSceneNodesRecursive( snode, tempParentNode );
    }
    
    for( auto& tskel : mSkeletons ) {
        tskel->setupRoot(tskel);
    }
    
    aScene->populatePoses( poses );
    
    // check the fbx scene if the mesh keyframes have been cached //
    if( aScene->getSettings().cacheMeshKeyframes ) {
        cacheMeshKeyframes( aScene->getSettings().blendCachedMeshKeyframes );
    }
    
    if( hasAnimations() ) {
        // set to the first animation
        setAnimation(0);
    }
    
}

//--------------------------------------------------------------
void ofxFBX::_parseSceneNodesRecursive( shared_ptr<ofxFBXSource::Node> anode, shared_ptr<ofxFBXNode> aParentNode ) {
    if( !anode ) return;
    
    shared_ptr<ofxFBXNode> newNode;
    ofxFBXSource::Node::NodeType ftype = anode->getType();
    if (ftype == ofxFBXSource::Node::OFX_FBX_MESH) {
        auto tmesh = make_shared<ofxFBXMesh>();
        if( tmesh ) {
            meshes.push_back( tmesh );
            newNode = tmesh;
        }
    } else if(ftype == ofxFBXSource::Node::OFX_FBX_NURBS_CURVE) {
        auto tcurve = make_shared<ofxFBXNurbsCurve>();
        if( tcurve ) {
            mNurbsCurves.push_back(tcurve);
            newNode = tcurve;
        }
    } else if(ftype == ofxFBXSource::Node::OFX_FBX_NULL ) {
        auto tnull = make_shared<ofxFBXNode>();
        if( tnull ) {
            mNullNodes.push_back(tnull);
            newNode = tnull;
        }
    } else if( ftype == ofxFBXSource::Node::OFX_FBX_SKELETON ) {
        auto tskel = make_shared<ofxFBXSkeleton>();
        if( tskel ) {
            mSkeletons.push_back( tskel );
            newNode = tskel;
        }
    }
    
    if( newNode ) {
        if( aParentNode ) {
            aParentNode->addChild( newNode );
            newNode->setParent( *aParentNode );
            newNode->setParentNode( aParentNode );
        } else {
            newNode->setParent( *this );
            mRootNodes.push_back( newNode );
        }
        newNode->setup( anode );
        mAllNodes.push_back( newNode );
    }
    
    for( int i = 0; i < anode->getNumChildren(); i++ ) {
        _parseSceneNodesRecursive( anode->getChildren()[i], newNode );
    }
}

//--------------------------------------------------------------
void ofxFBX::earlyUpdate(float aElapsedSeconds) {
    FbxPose * lPose = NULL;
    // poses will override the animations and the settings of the bones //
    if( arePosesEnabled() && hasPoses() && (poseIndex >= 0 && poseIndex < getNumPoses() ) ) {
        cout << "Got a pose | " << ofGetFrameNum() << endl;
        lPose = fbxScene->getFBXScene()->GetPose( poseIndex );
    }
    
    if( !areAnimationsEnabled() || !hasAnimations() ) {
        if( bFirstRun ) {
            FbxTime ttime(FBXSDK_TIME_INFINITE);
            //        cout << "Calling update bones: " << " | " << ofGetElapsedTimef() << endl;
            for( auto& node : mAllNodes ) {
//                if( node->getType() == ofxFBXSource::Node::OFX_FBX_SKELETON ) {
                    node->update( ttime, lPose );
                    node->update();
//                }
            }
        }
        
    }
    
    if(animations.size() < 1) return;
    if(!areAnimationsEnabled()) return;
    
    
    float etimef = aElapsedSeconds;//ofGetElapsedTimef();
    if (etimef < 0) {
        etimef = ofGetElapsedTimef();
    }
    //    cout << "Should not be reaching here: ofxFBXManager :: update | " << ofGetFrameNum() << endl;
    
    animations[animationIndex].update(etimef);
    if( mAnimTrans.bActive ) {
        animations[mAnimTrans.animIndex2].update(etimef);
    }
    
    
    
    if( currentAnimationStack != NULL ) {
        fbxScene->getFBXScene()->SetCurrentAnimationStack( currentAnimationStack );
    }
    
    //    cout << "ofxFBXManager :: update : animations | " << ofGetElapsedTimef() << endl;
    // TODO: is there a way to check if we need to update the bone positions? Right now it always updates.
    // If other fbxManagers are playing animations at different times or moving around bones, then it will get weird if it doesn't
    // update.  //
    //    if(animations[animationIndex].isFrameNew() || animations[animationIndex].isPaused() ) {
    signed long tFbxAnimTime = (signed long)animations[animationIndex].fbxCurrentTime.GetMilliSeconds();
    signed long tFbxAnimTime2 = 0;
    if( mAnimTrans.bActive ) {
        tFbxAnimTime2 = (signed long)animations[mAnimTrans.animIndex2].fbxCurrentTime.GetMilliSeconds();
    }
    
    for( auto& node : mAllNodes ) {
        if( bUsingKeyframes ) {
            if( mAnimTrans.bActive ) {
                node->update( mAnimTrans.animIndex1, tFbxAnimTime, mAnimTrans.animIndex2, tFbxAnimTime2, mAnimTrans.percent );
            } else {
                node->update( animationIndex, tFbxAnimTime );
            }
        } else {
            node->update( animations[animationIndex].fbxCurrentTime, lPose );
        }
        node->update();
    }
    
    if( mAnimTrans.bActive ) {
//        float etimef = aElapsedSeconds;//ofGetElapsedTimef();
//        if (etimef < 0) {
//            etimef = ofGetElapsedTimef();
//        }
        mAnimTrans.percent = (etimef - mAnimTrans.startTime) / mAnimTrans.duration;
        if( mAnimTrans.percent >= 1.f ) {
            mAnimTrans.percent = 1.0;
            animationIndex = mAnimTrans.animIndex2; // switch to the targeted animation
            mAnimTrans.bActive = false;
        }
    }
}

//--------------------------------------------------------------
void ofxFBX::update(float aElapsedSeconds) {
    earlyUpdate(aElapsedSeconds);
    lateUpdate();
}

//--------------------------------------------------------------
void ofxFBX::lateUpdate() {
    
    if( !areAnimationsEnabled() || !hasAnimations() ) {
        if( bFirstRun ) {
            FbxTime ttime(FBXSDK_TIME_INFINITE);
            for( auto& node : mAllNodes ) {
                node->lateUpdate(ttime, currentFbxAnimationLayer, NULL);
            }
        }
    }
    
    if(animations.size() > 0 && areAnimationsEnabled() ) {
        for( auto& node : mAllNodes ) {
            node->lateUpdate(animations[animationIndex].fbxCurrentTime, currentFbxAnimationLayer, NULL);
        }
    }
    
    if(!bUpdateMeshVbo) {
        // tell all of the meshes not to update the vbo //
        for( auto& m : meshes ) {
            m->setMeshDirty(false);
        }
    }
    
    bFirstRun = false;
}

#pragma mark - Draw
//--------------------------------------------------------------
void ofxFBX::draw() {
    drawMeshes();
    drawCurves();
}

//--------------------------------------------------------------
void ofxFBX::drawMesh(int aindex) {
	if (aindex < 0 || aindex >= meshes.size()) {
		return;
	}
    meshes[aindex]->draw();
}

//--------------------------------------------------------------
void ofxFBX::drawMeshWireframe(int aindex) {
	if (aindex < 0 || aindex >= meshes.size()) {
		return;
	}
    meshes[aindex]->drawWireframe();
}

//--------------------------------------------------------------
void ofxFBX::drawMeshes() {
    for( auto& tmesh : meshes ) {
        tmesh->draw();
    }
}

//--------------------------------------------------------------
void ofxFBX::drawMeshWireframes() {
    for( auto& tmesh : meshes ) {
        tmesh->drawWireframe();
    }
}

//--------------------------------------------------------------
void ofxFBX::drawMeshNormals( float aLen, bool aBFaceNormals ) {
    for( auto& tmesh : meshes ) {
        tmesh->drawNormals(aLen, aBFaceNormals);
    }
}

//--------------------------------------------------------------
void ofxFBX::drawSkeletons( float aLen, bool aBDrawAxes ) {
    for(int i = 0; i < mSkeletons.size(); i++ ) {
        mSkeletons[i]->draw( aLen, aBDrawAxes );
    }
}

//--------------------------------------------------------------
void ofxFBX::drawCurves() {
    for( auto& nc : mNurbsCurves ) {
        nc->draw();
    }
}

#pragma mark - Getters

//--------------------------------------------------------------
string ofxFBX::getInfoString() {
    stringstream ss;
    if(fbxScene) {
        ss << "ofxFBX :: " << ofFilePath::getBaseName(fbxScene->getFbxFilePath())<<"."<<ofFilePath::getFileExt(fbxScene->getFbxFilePath()) << endl;
    }
    for( auto& sn : mRootNodes ) {
        ss << sn->getAsString();
    }
    return ss.str();
}

//--------------------------------------------------------------
ofxFBXSource::Scene* ofxFBX::getFbxScene() {
    return fbxScene;
}

//--------------------------------------------------------------
string ofxFBX::getFbxFilePath() {
    if( fbxScene ) {
        return fbxScene->getFbxFilePath();
    }
    return "";
}

#pragma mark Null Nodes
//--------------------------------------------------------------
vector< shared_ptr<ofxFBXNode> >& ofxFBX::getNullNodes() {
    return mNullNodes;
}

//--------------------------------------------------------------
int ofxFBX::getNumNullNodes() {
    return mNullNodes.size();
}

#pragma mark Meshes
//--------------------------------------------------------------
vector< shared_ptr<ofxFBXMesh> >& ofxFBX::getMeshes() {
    return meshes;
}

//--------------------------------------------------------------
int ofxFBX::getNumMeshes() {
    return (int)meshes.size();
}

//--------------------------------------------------------------
string ofxFBX::getMeshName( int aMeshIndex ) {
    return meshes[aMeshIndex]->getName();//fbxScene->getMeshes()[aMeshIndex]->getName();
}

//--------------------------------------------------------------
void ofxFBX::setMaterialsEnabled( bool ab ) {
    for( auto mesh : meshes ) {
        mesh->setMaterialsEnabled( ab );
    }
}

//--------------------------------------------------------------
vector< shared_ptr<ofxFBXSource::MeshTexture> > ofxFBX::getSourceTextures() {
    vector< shared_ptr<ofxFBXSource::MeshTexture> > rtexs;
    for( auto mesh : meshes ) {
        auto mats = mesh->getMaterials();
        for( auto mat : mats ) {
            if( mat->hasSourceTexture() ) {
                rtexs.push_back( mat->getSrcTexture() );
            }
        }
    }
    return rtexs;
}

//--------------------------------------------------------------
int ofxFBX::getNumSourceTextures() {
    int tnum = 0;
    for( auto mesh : meshes ) {
        auto mats = mesh->getMaterials();
        for( auto mat : mats ) {
            if( mat->hasSourceTexture() ) {
                tnum++;
            }
        }
    }
    return tnum;
}

//--------------------------------------------------------------
void ofxFBX::setMeshesDirty( bool ab ) {
    for( auto mesh : meshes ) {
        mesh->setMeshDirty( ab );
    }
}

//--------------------------------------------------------------
void ofxFBX::cacheMeshKeyframes( bool aBlendMeshKeys ) {
    if( fbxScene == NULL ) return;
    
    if( !fbxScene->getSettings().useKeyFrames ) {
        ofLogWarning("ofxFBX :: cacheMeshKeyframes : must use .useKeyFrames when loading scene in Settings ");
        return;
    }
    
//    if( fbxScene->areMeshKeyframesCached() ) {
//        return;
//    }
    
    if( !fbxScene->areMeshKeyframesCached() ) {
        
        ofLogNotice(" ofxFBX :: cacheMeshKeyframes : ") << getFbxFilePath();
        
        // one mesh per keyframe per animation //
        for( int ia = 0; ia < animations.size(); ia++ ) {
            setAnimation(ia);
            auto& tanim = getCurrentAnimation();
            
            ofLogNotice( "ofxFBX :: cacheMeshKeyframes : animation: " ) << tanim.name << " num key frames: " << tanim.getTotalNumFrames();
            
            for( int i = 0; i < tanim.getTotalNumFrames(); i++ ) {
                tanim.setFrame(i);
                signed long tFbxAnimTime = (signed long)tanim.fbxCurrentTime.GetMilliSeconds();
                // update all the nodes //
                for( auto& node : mAllNodes ) {
                    if( bUsingKeyframes ) {
                        node->update( ia, tFbxAnimTime );
                    } else {
                        node->update( tanim.fbxCurrentTime, NULL );
                    }
                    node->update();
                }
                
                for( auto& node : mAllNodes ) {
                    node->lateUpdate( tanim.fbxCurrentTime, currentFbxAnimationLayer, NULL );
                    // now we need to cache the meshes in the meshes //
                    if( node->getType() == ofxFBXSource::Node::OFX_FBX_MESH && node->getofxFbxSrcNode() ) {
                        // now lets cache the mesh //
                        shared_ptr<ofxFBXMesh> meshNode = dynamic_pointer_cast<ofxFBXMesh>(node);
                        shared_ptr<ofxFBXSource::Mesh> tSrcMesh = dynamic_pointer_cast<ofxFBXSource::Mesh>(node->getofxFbxSrcNode());
                        auto& kMeshKeyCollection = tSrcMesh->getMeshKeyCollection(ia);
                        ofxFBXSource::MeshAnimKey meshKey;
                        meshKey.mesh = meshNode->getMesh();
                        meshKey.millis = tFbxAnimTime;
                        kMeshKeyCollection.meshKeys.push_back( meshKey );
                    }
                }
            }
        }
    }
    
    // disable all of the bones, since the meshes are cached, it will not be influenced by the bones anymore //
    for( auto& node : mAllNodes ) {
        if( node->getType() == ofxFBXSource::Node::OFX_FBX_BONE ) {
            auto bone = dynamic_pointer_cast<ofxFBXBone>(node);
            bone->disableAnimation( true );
            
            if(node->getofxFbxSrcNode()) {
                // now disable the source bones //
                auto sbone = dynamic_pointer_cast<ofxFBXSource::Bone>(node->getofxFbxSrcNode());
                if( sbone ) sbone->disableAnimation(true);
            }
        }
    }
    
    for( int i = 0; i < mSkeletons.size(); i++ ) {
        mSkeletons[i]->disableAnimation();
        if( mSkeletons[i]->getofxFbxSrcNode() ) {
            auto sskel = dynamic_pointer_cast<ofxFBXSource::Skeleton>(mSkeletons[i]->getofxFbxSrcNode());
            if( sskel->root ) {
                sskel->root->disableAnimation(true);
            }
        }
        
    }
    
    
    // let the source meshes know that they are cached //
    for( auto& node : mAllNodes ) {
        if( node->getType() == ofxFBXSource::Node::OFX_FBX_MESH && node->getofxFbxSrcNode() ) {
            auto mesh = dynamic_pointer_cast<ofxFBXMesh>(node);
            if( mesh ) {
                mesh->setBlendMeshFrames( aBlendMeshKeys );
            }
            auto tSrcMesh = dynamic_pointer_cast<ofxFBXSource::Mesh>(node->getofxFbxSrcNode());
            if(tSrcMesh) tSrcMesh->setUsingCachedMeshes(true);
        }
    }
    fbxScene->getSettings().cacheMeshKeyframes = true;
    fbxScene->setMeshKeyframesCached( true );
}

//--------------------------------------------------------------
void ofxFBX::setMeshVboUpdate(bool ab) {
    bUpdateMeshVbo=ab;
}

#pragma mark Curves
//--------------------------------------------------------------
vector< shared_ptr<ofxFBXNurbsCurve> >& ofxFBX::getNurbsCurves() {
    return mNurbsCurves;
}

//--------------------------------------------------------------
int ofxFBX::getNumNurbsCurves() {
    return mNurbsCurves.size();
}

#pragma mark Skeletons
//--------------------------------------------------------------
vector< shared_ptr<ofxFBXSkeleton> >& ofxFBX::getSkeletons() {
    return mSkeletons;
}

//--------------------------------------------------------------
bool ofxFBX::hasBones() {
    return getNumBones() > 0;
}

//--------------------------------------------------------------
int ofxFBX::getNumSkeletons() {
    return mSkeletons.size();
}

//--------------------------------------------------------------
int ofxFBX::getNumBones() {
    int tNumBones = 0;
    for( int i = 0; i < mSkeletons.size(); i++ ) {
        tNumBones += mSkeletons[i]->getNumBones();
    }
    return tNumBones;
}

//--------------------------------------------------------------
shared_ptr<ofxFBXBone> ofxFBX::getBone( string aBoneName, int aSkeletonIndex ) {
    if( aSkeletonIndex < 0 ) return NULL;
    if( aSkeletonIndex >= mSkeletons.size() ) return NULL;
    return mSkeletons[ aSkeletonIndex ]->getBone( aBoneName );
}

//--------------------------------------------------------------
string ofxFBX::getSkeletonInfo() {
    string retStr = "";
    for( int i = 0; i < mSkeletons.size(); i++ ) {
        retStr += ofToString(i,0)+" - " + mSkeletons[i]->getAsString();
    }
    return retStr;
}

#pragma mark - Poses
//--------------------------------------------------------------
bool ofxFBX::hasPoses() {
    return getNumPoses() > 0;
}

//--------------------------------------------------------------
bool ofxFBX::arePosesEnabled() {
    return bPosesEnabled;
}

//--------------------------------------------------------------
void ofxFBX::enablePoses() {
    bPosesEnabled = true;
}

//--------------------------------------------------------------
void ofxFBX::disablePoses() {
    bPosesEnabled = false;
}

//--------------------------------------------------------------
void ofxFBX::togglePosesEnabled() {
    if( arePosesEnabled() ) disablePoses();
    else enablePoses();
}

//--------------------------------------------------------------
int ofxFBX::getNumPoses() {
    return (int)poses.size();
}

//--------------------------------------------------------------
void ofxFBX::setPoseIndex( int aIndex ) {
    aIndex = ofClamp( aIndex, 0, getNumPoses()-1 );
    poseIndex = aIndex;
}

//--------------------------------------------------------------
shared_ptr< ofxFBXSource::Pose > ofxFBX::getCurrentPose() {
    if( hasPoses() && (poseIndex >= 0 && poseIndex < getNumPoses() ) ) {
        return poses[ poseIndex ];
    }
    shared_ptr<ofxFBXSource::Pose> tpose;
    return tpose;
}

//--------------------------------------------------------------
vector< shared_ptr<ofxFBXSource::Pose> > ofxFBX::getPoses() {
    return poses;
}

#pragma mark - Animation
//--------------------------------------------------------------
int ofxFBX::getNumAnimations() {
    return animations.size();
}

//--------------------------------------------------------------
int ofxFBX::getCurrentAnimationIndex() {
    return animationIndex;
}

//--------------------------------------------------------------
ofxFBXAnimation& ofxFBX::getCurrentAnimation() {
    if(animations.size() < 1) {
        ofLogWarning("ofxFBXAnimation :: return dummy animation, there are no animations");
        return dummyAnimation;
    }
    return animations[ animationIndex ];
}

//--------------------------------------------------------------
int ofxFBX::getAnimationIndex( string aname ) {
    int findex = -1;
    for( int i = 0; i < animations.size(); i++ ) {
        if( animations[i].name == aname ) {
            findex = i;
            break;
        }
    }
    return findex;
}

//--------------------------------------------------------------
ofxFBXAnimation& ofxFBX::getAnimation( int aIndex ) {
    if( aIndex > animations.size() -1 ) {
        ofLogWarning( "ofxFBXManager :: getAnimation : index is too high " ) << aIndex;
        aIndex = ofClamp(aIndex, 0, animations.size()-1);
    }
    
    if( aIndex < 0 ) {
        ofLogWarning( "ofxFBXManager :: getAnimation : index is too low " ) << aIndex;
        return dummyAnimation;
    }
    
    if( animations.size() == 0 ) {
        aIndex = 0;
        animations.push_back( dummyAnimation );
    }
    
    return animations[ aIndex ];
}

//--------------------------------------------------------------
ofxFBXAnimation& ofxFBX::getAnimation( string aname ) {
    return getAnimation( getAnimationIndex( aname ) );
}

//--------------------------------------------------------------
bool ofxFBX::hasAnimation( string aname ) {
    return (getAnimationIndex(aname) > -1);
}

//--------------------------------------------------------------
void ofxFBX::setAnimation( int aIndex ) {
    
    if( aIndex < 0) {
        ofLogWarning("ofxFBXManager :: setAnimation : returning because the index is less than 0!");
        return;
    }
    
    if(animations.size() < 1) {
        ofLogWarning("ofxFBXManager :: setAnimation : returning because there are no animations!");
        return;
    }
    if(aIndex >= animations.size()) {
        aIndex = ofClamp(aIndex, 0, animations.size()-1);
        ofLogWarning("ofxFBXManager :: setAnimation : index to high, clamping to ") << aIndex;
    }
    currentAnimationStack = fbxScene->getFBXScene()->FindMember<FbxAnimStack>( (&animations[aIndex].fbxname)->Buffer() );
    if (currentAnimationStack == NULL) {
        // this is a problem. The anim stack should be found in the scene!
        ofLogWarning("ofxFBXManager :: setAnimation : the anim stack was not found in the scene!");
        return;
    }
//    int numAnimLayers = lCurrentAnimationStack->GetMemberCount<FbxAnimLayer>();
//    cout << "Number of animation layers= " << numAnimLayers << endl;
    currentFbxAnimationLayer = currentAnimationStack->GetMember<FbxAnimLayer>();
    fbxScene->getFBXScene()->SetCurrentAnimationStack( currentAnimationStack );
    
    animationIndex = aIndex;
}

//--------------------------------------------------------------
void ofxFBX::setAnimation( string aname ) {
    setAnimation( getAnimationIndex( aname ) );
}

//--------------------------------------------------------------
void ofxFBX::enableAnimations() {
    if(!hasAnimations()) {
        ofLogWarning("ofxFBXManager :: enableAnimations : there are no animations in this scene. Disabling.");
        disableAnimations();
        return;
    }
    bAnimationsEnabled = true;
    for( int i = 0; i < mSkeletons.size(); i++ ) {
        mSkeletons[i]->enableAnimation();
    }
}

//--------------------------------------------------------------
void ofxFBX::disableAnimations() {
    bAnimationsEnabled = false;
    for( int i = 0; i < mSkeletons.size(); i++ ) {
        mSkeletons[i]->disableAnimation();
    }
    bFirstRun=true;
}

//--------------------------------------------------------------
void ofxFBX::toggleAnimationsEnabled() {
    if( bAnimationsEnabled ) {
        disableAnimations();
    } else {
        enableAnimations();
    }
}

//--------------------------------------------------------------
bool ofxFBX::areAnimationsEnabled() {
    return (bAnimationsEnabled && hasAnimations());
}

//--------------------------------------------------------------
bool ofxFBX::hasAnimations() {
    if( animations.size() > 1 ) return true;
    if( animations.size() == 1 ) {
        if( animations[0].name == "dummyAnimation" ) {
            return false;
        } else {
            return true;
        }
    }
    return false;
}

//--------------------------------------------------------------
void ofxFBX::reloadAnimationsFromScene() {
    if( getFbxScene() ) {
        animations.clear();
        getFbxScene()->populateAnimations( animations );
    }
}

//--------------------------------------------------------------
void ofxFBX::transition( int aAnimIndex1, int aNumIndex2, float aduration ) {
    if( !bUsingKeyframes ) {
        ofLogError("ofxFbxManager :: transition : not transitioning, must be using keyframes. Set ofxFBXSceneSettings.useKeyFrames to true when loading the fbx scene" );
        return;
    }
    if( !hasAnimations() ) {
        ofLogError("ofxFbxManager :: transition : not transitioning, no animations" );
        return;
    }
    if( !areAnimationsEnabled() ) {
        ofLogError("ofxFbxManager :: transition : not transitioning, animations disabled" );
        return;
    }
    if( aAnimIndex1 < 0 || aNumIndex2 < 0 || aAnimIndex1 >= getNumAnimations() || aNumIndex2 >= getNumAnimations() ) {
        ofLogError("ofxFbxManager :: transition : invalid index: " ) << aAnimIndex1 << " index2: " << aNumIndex2 << " num animations: " << getNumAnimations();
        return;
    }
    mAnimTrans.animIndex1 = aAnimIndex1;
    mAnimTrans.animIndex2 = aNumIndex2;
    mAnimTrans.duration = aduration;
    mAnimTrans.percent = 0.f;
    mAnimTrans.startTime = ofGetElapsedTimef();
    mAnimTrans.bActive = true;
}

//--------------------------------------------------------------
void ofxFBX::transition( string aAnimName1, string aNumName2, float aduration ) {
    transition( getAnimationIndex( aAnimName1 ), getAnimationIndex( aNumName2 ), aduration );
}

//--------------------------------------------------------------
void ofxFBX::transition( string aToAnimName, float aduration ) {
    transition( getCurrentAnimationIndex(), getAnimationIndex(aToAnimName), aduration );
}

//--------------------------------------------------------------
void ofxFBX::transition( int aToAnimIndex, float aduration ) {
    transition( getCurrentAnimationIndex(), aToAnimIndex, aduration );
}

//--------------------------------------------------------------
bool ofxFBX::isTransitioning() {
    return mAnimTrans.bActive;
}

//--------------------------------------------------------------
float ofxFBX::getTransitionPercent() {
    return mAnimTrans.percent;
}

#pragma mark Hierarchy
//--------------------------------------------------------------
shared_ptr<ofxFBXNode> ofxFBX::getNodeForName( string aPath, bool bStrict) {
    shared_ptr<ofxFBXNode> temp;
    for( auto& sn : mRootNodes ) {
        temp = sn->getNodeforName( sn, aPath, bStrict );
        if( temp ) break;
    }
    return temp;
}

// Flattens out scene hierarchy //
//--------------------------------------------------------------
vector< shared_ptr<ofxFBXNode> > ofxFBX::getAllNodes() {
    return mAllNodes;
}

//--------------------------------------------------------------
vector< shared_ptr<ofxFBXNode> > ofxFBX::getRootNodes() {
    return mRootNodes;
}













