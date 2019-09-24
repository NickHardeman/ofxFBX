//
//  ofxFBXBone.h
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 11/1/13.
//
//

#pragma once
#include "ofxFBXNode.h"
#include "ofxFBXSrcBone.h"


// note: Bones are called clusters when referring to them from an FbxMesh in the FBX SDK
class ofxFBXBone : public ofxFBXNode {
public:
    
    ofxFBXSource::Node::NodeType getType() override;
    
    void setBoneSource( shared_ptr<ofxFBXSource::Bone> asrc, shared_ptr<ofxFBXNode> aself );
    
    void update() override;
    void lateUpdate();
    
    void enableAnimation( bool bRecursively );
    void disableAnimation( bool bRecursively);
    bool isAnimationEnabled();
    
    void draw( float aLen, bool aBDrawAxes );
    
    int getNumBones();
    
    shared_ptr<ofxFBXBone> getBone( string aName );
    void findBoneRecursive( string aName, shared_ptr<ofxFBXBone>& returnBone );
    
    void pointTo( glm::vec3 aTarget );
    void pointTo( glm::vec3 aTarget, glm::vec3 aAxis );
    
    shared_ptr<ofxFBXSource::Bone> boneSrc;// = NULL;
    map< string, shared_ptr<ofxFBXBone> > childBones;
    
    string getAsString( int aLevel=0) override;
    
    bool bUpdateFromAnimation=true;
    
    
    
    
//    ofxFBXBone();
//    ~ofxFBXBone();
//
//    //ofxFBXNode::NodeType getType() override { return OFX_FBX_BONE; }
//
//    bool doesExist();
//
//    void setAsRoot();
//    bool isRoot() { return bIsRoot; }
//    void setup( FbxNode* pNode );
////    void setupFromSourceBones();
//
//    void update( FbxTime& pTime, FbxPose* pPose );
//    virtual void update( int aAnimIndex, signed long aMillis ) override;
//    virtual void update( int aAnimIndex1, signed long aAnim1Millis, int aAnimIndex2, signed long aAnim2Millis, float aMixPct ) override;
//
//
//    void lateUpdate();
//    void draw( float aLen = 6.f, bool aBDrawAxes = true);
//
////    void pointTo( glm::vec3 aTarget );
////    void pointTo( glm::vec3 aTarget, glm::vec3 aAxis );
//
//    bool isLimb();
//    bool hasSkeletonParent();
//
//    void enableAnimation( bool bRecursively=false );
//    void disableAnimation( bool bRecursively=false );
//    bool isAnimationEnabled();
//
//    void updateFbxTransform();
////    void updateFbxTransformLocal();
//    glm::quat& getOriginalLocalRotation();
//
//    virtual void clearKeyFrames() override;
//
//    int getNumBones();
//    map< string, ofxFBXBone* > getAllBones();
//
//    string getAsString( int aLevel=0) override;
//    ofxFBXBone* getBone( string aName );
//
//    FbxSkeleton* getFbxSkeleton();
////    FbxNode* fbxNode;
//    FbxAMatrix fbxTransform;
//
//    string parentBoneName;
//
////    int level;
//
////    map< string, ofxFBXBone > bones;
////    map< string, ofxFBXBone* > sourceBones;
//    map< string, ofxFBXBone* > childBones;
    
//    ofxFBXBone* sourceBone;
    
protected:
//    void findBoneRecursive( string aName, ofxFBXBone*& returnBone );
//    void populateBonesRecursive( map< string, ofxFBXBone* >& aBoneMap );
//    bool bExists, bIsRoot;
    
//    bool bExternalControlEnabled;
//    bool bUpdateFromAnimation;
};







