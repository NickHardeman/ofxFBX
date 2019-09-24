//
//  ofxFBXSrcBone.h
//  ConnectionsWall-Nick
//
//  Created by Nick Hardeman on 7/11/19.
//

#pragma once
#include "ofMain.h"
#include <fbxsdk.h>

#include "ofxFBXSrcNode.h"

namespace ofxFBXSource {
// note: Bones are called clusters when referring to them from an FbxMesh in the FBX SDK
class Bone : public ofxFBXSource::Node {
public:
    Bone();
    ~Bone();
    
    ofxFBXSource::Node::NodeType getType() override { return OFX_FBX_BONE; }
    
    bool doesExist();
    
    void setAsRoot();
    bool isRoot() { return bIsRoot; }
    void setup( FbxNode* pNode ) override;
    //    void setupFromSourceBones();
    
    void update( FbxTime& pTime, FbxPose* pPose ) override;
    virtual void update( int aAnimIndex, signed long aMillis ) override;
    virtual void update( int aAnimIndex1, signed long aAnim1Millis, int aAnimIndex2, signed long aAnim2Millis, float aMixPct ) override;
    
    
    void lateUpdate();
//    void draw( float aLen = 6.f, bool aBDrawAxes = true);
    
    bool isLimb();
    bool hasSkeletonParent();
    
    void enableAnimation( bool bRecursively=false );
    void disableAnimation( bool bRecursively=false );
    bool isAnimationEnabled();
    
    void updateFbxTransform();
    //    void updateFbxTransformLocal();
    glm::quat& getOriginalLocalRotation();
    
    virtual void clearKeyFrames() override;
    
    int getNumBones();
//    map< string, shared_ptr<ofxFBXSource::Bone> > getAllBones();
    
    string getAsString( int aLevel=0) override;
    shared_ptr<ofxFBXSource::Bone> getBone( string aName );
    
    FbxSkeleton* getFbxSkeleton();
    FbxAMatrix fbxTransform;
    
    string parentBoneName = "";
    
    
//    map< string, ofxFBXSource::Bone* > childBones;
    map< string, shared_ptr<ofxFBXSource::Bone> > childBones;
    
protected:
    void findBoneRecursive( string aName, shared_ptr<ofxFBXSource::Bone>& returnBone );
    void populateBonesRecursive( map< string, shared_ptr<ofxFBXSource::Bone> >& aBoneMap );
    bool bExists, bIsRoot;
    
    //    bool bExternalControlEnabled;
    bool bUpdateFromAnimation;
};
    
}
