//
//  ofxFBX.h
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 10/31/13.
//
//

#pragma once
#include "ofxFBXSrcScene.h"
#include "ofxFBXMesh.h"
#include "ofxFBXSkeleton.h"
#include "ofxFBXNurbsCurve.h"
#include "ofxFBXSrcPose.h"
//#include "ofxFBXAnimation.h"
//#include "ofxFBXManager.h"

class ofxFBX : public ofNode {
public:
    
    class AnimationTransition {
    public:
        bool bActive = false;
        float percent = 0.f;
        float startTime = 0;
        float duration = 1.0f;
        int animIndex1 = 0;
        int animIndex2 = 0;
        
    };
    
    ofxFBX();
    ~ofxFBX();
    
    static bool shouldRemoveScene( const shared_ptr<ofxFBXSource::Scene>& ascene );
    
    void clear();
    bool load(ofxFBXSource::Scene::Settings aSettings );
    bool load(string path, ofxFBXSource::Scene::Settings aSettings = ofxFBXSource::Scene::Settings() );
    void setup( ofxFBXSource::Scene* aScene );
    
    
    void earlyUpdate(float aElapsedSeconds=-1);
    void update(float aElapsedSeconds=-1);
    void lateUpdate();
    
    void draw();
    void drawMesh(int aindex);
    void drawMeshWireframe(int aindex);
    void drawMeshes();
    void drawMeshWireframes();
    void drawMeshNormals( float aLen = 4.f, bool aBFaceNormals = true );
    void drawSkeletons( float aLen = 10.f, bool aBDrawAxes = true );
    void drawCurves();
    
    string getInfoString();
    ofxFBXSource::Scene* getFbxScene();
    string getFbxFilePath();
    // NULL NODES ////////////////////////
    vector< shared_ptr<ofxFBXNode> >& getNullNodes();
    int getNumNullNodes();
    
    // MESHES ////////////////////////////
    vector< shared_ptr<ofxFBXMesh> >& getMeshes();
    int getNumMeshes();
    string getMeshName( int aMeshIndex );
    void setMaterialsEnabled( bool ab );
    vector< shared_ptr<ofxFBXSource::MeshTexture> > getSourceTextures();
    int getNumSourceTextures();
    void setMeshesDirty( bool ab );
    // cache mesh keyframes, will disable bone control //
    void cacheMeshKeyframes(bool aBlendMeshKeys);
    void setMeshVboUpdate(bool ab);
    
    // CURVES ////////////////////////////
    vector< shared_ptr<ofxFBXNurbsCurve> >& getNurbsCurves();
    int getNumNurbsCurves();
    
    // SKELETONS /////////////////////////
    vector< shared_ptr<ofxFBXSkeleton> >& getSkeletons();
    bool hasBones();
    int getNumSkeletons();
    int getNumBones();
    shared_ptr<ofxFBXBone> getBone( string aBoneName, int aSkeletonIndex=0 );
    string getSkeletonInfo();
    
    // POSES ////////////////////////////
    bool hasPoses();
    bool arePosesEnabled();
    void enablePoses();
    void disablePoses();
    void togglePosesEnabled();
    int getNumPoses();
    void setPoseIndex( int aIndex );
    shared_ptr<ofxFBXSource::Pose> getCurrentPose();
    vector< shared_ptr<ofxFBXSource::Pose> > getPoses();
    
    // ANIMATIONS ///////////////////////
    int getNumAnimations();
    int getCurrentAnimationIndex();
    int getAnimationIndex( string aname );
    ofxFBXAnimation& getCurrentAnimation();
    ofxFBXAnimation& getAnimation( int aIndex );
    ofxFBXAnimation& getAnimation( string aname );
    bool hasAnimation( string aname );
    void setAnimation( int aIndex );
    void setAnimation( string aname );
    void enableAnimations();
    void disableAnimations();
    void toggleAnimationsEnabled();
    bool areAnimationsEnabled();
    bool hasAnimations();
    void reloadAnimationsFromScene();
    
    void transition( int aAnimIndex1, int aNumIndex2, float aduration );
    void transition( string aAnimName1, string aNumName2, float aduration );
    void transition( string aToAnimName, float aduration );
    void transition( int aToAnimIndex, float aduration );
    
    bool isTransitioning();
    float getTransitionPercent();
    AnimationTransition& getTransition() { return mAnimTrans; }
    
    
    // getter / hierarchy functions //
    shared_ptr<ofxFBXNode> getNodeForName( string aPath, bool bStrict=false);

    template<typename ofxFBXNodeType>
    shared_ptr<ofxFBXNodeType> get( string aPath, bool bStrict = false ) {
        auto stemp = getNodeForName( aPath, bStrict );
        return dynamic_pointer_cast<ofxFBXNodeType>( stemp );
    }

    template<typename ofxFBXNodeType>
    vector< shared_ptr<ofxFBXNodeType> > getNodesForType( string aPath = "", bool bStrict = false ) {

        auto stemp = make_shared<ofxFBXNodeType>();
        int sType = stemp->getType();

        vector< shared_ptr<ofxFBXNodeType> > telements;

        if( aPath != "" ) {
            shared_ptr<ofxFBXNode> temp = getNodeForName( aPath, bStrict );
            if( temp ) {
                return temp->getKidsForType<ofxFBXNodeType>();
            }
        }

        for( int i = 0; i < mRootNodes.size(); i++ ) {
            if( mRootNodes[i]->getType() == sType ) {
                telements.push_back( dynamic_pointer_cast<ofxFBXNodeType>(mRootNodes[i]) );
            }
        }
        return telements;
    }

    template<typename ofxFBXNodeType>
    vector< shared_ptr<ofxFBXNodeType> > getAllNodesForType( string aNameToContain="", bool bStrict=false ) {
        auto stemp = make_shared<ofxFBXNodeType>();
        int sType = stemp->getType();

//        auto allNodes = getAllNodes();

        vector< shared_ptr<ofxFBXNodeType> > telements;

        for( int i = 0; i < mAllNodes.size(); i++ ) {
            if( mAllNodes[i]->getType() == sType ) {
                if( aNameToContain != "" ) {
                    if( bStrict ) {
                        if(mAllNodes[i]->getName() != aNameToContain ) {
                            continue;
                        }
                    } else {
                        if( !ofIsStringInString( mAllNodes[i]->getName(), aNameToContain )) {
                            continue;
                        }
                    }
                }
                
                shared_ptr<ofxFBXNodeType> tele = dynamic_pointer_cast<ofxFBXNodeType>(mAllNodes[i]);
//                cout << "ofxFBX :: getAllNodesForType : adding " << mAllNodes[i]->getName() << " valid: " << (tele ? "yes" : "no") << endl;
                telements.push_back( tele );
            }
        }
        return telements;
    }
    
    // Flattens out scene hierarchy //
    vector< shared_ptr<ofxFBXNode> > getAllNodes();
    vector< shared_ptr<ofxFBXNode> > getRootNodes();
    
    
protected:
    void _parseSceneNodesRecursive( shared_ptr<ofxFBXSource::Node> anode, shared_ptr<ofxFBXNode> aParentNode );
    
    // if called load directly on the ofxFBXManager, then will create in internal instance //
    static vector< shared_ptr<ofxFBXSource::Scene> > mCachedScenes;
    
    vector< shared_ptr<ofxFBXNode> > mNullNodes;
    vector< shared_ptr<ofxFBXMesh> > meshes;
    vector< shared_ptr<ofxFBXNurbsCurve> > mNurbsCurves;
    vector< shared_ptr<ofxFBXSkeleton> > mSkeletons;
    vector< shared_ptr<ofxFBXSource::Pose> > poses;
    
    vector<ofxFBXAnimation> animations;
    
    vector< shared_ptr<ofxFBXNode> > mRootNodes;
    vector< shared_ptr<ofxFBXNode> > mAllNodes;
    
    FbxAnimLayer* currentFbxAnimationLayer = NULL;
    FbxAnimStack* currentAnimationStack = NULL;
    ofxFBXSource::Scene* fbxScene = NULL;
    
    int animationIndex = 0;
    bool bAnimationsEnabled = true;
    int poseIndex = 0;
    bool bPosesEnabled = false;
    bool bUsingKeyframes = false;
    
    bool bFirstRun = true;
    bool bUpdateMeshVbo = true;
    
    // only one for now //
    AnimationTransition mAnimTrans;
    
    shared_ptr<ofxFBXSource::Scene> mSceneInternal;
    
};

