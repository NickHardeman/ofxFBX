//
//  ofxFBXSrcNode.h
//  ConnectionsWall-Nick
//
//  Created by Nick Hardeman on 7/11/19.
//

#pragma once

#include "ofMain.h"
#include <fbxsdk.h>
#include "ofxFBXUtils.h"

namespace ofxFBXSource {

template<class T>
struct AnimKey {
    T value;
    signed long millis;
};

class AnimKeyCollection {
public:
    string name;
    vector< ofxFBXSource::AnimKey<float> > posKeysX, posKeysY, posKeysZ;
    vector< ofxFBXSource::AnimKey<ofQuaternion> > rotKeys;
    vector< ofxFBXSource::AnimKey<float> > scaleKeysX, scaleKeysY, scaleKeysZ;
};

class Node : public ofNode {
public:
    
    enum NodeType {
        OFX_FBX_NULL=0,
        OFX_FBX_MESH,
        OFX_FBX_SKELETON,
        OFX_FBX_BONE,
        OFX_FBX_NURBS_CURVE,
        OFX_FBX_UNKNOWN
    };
    
    Node();
    ~Node();
    
    static string getFbxTypeStringFromNode( FbxNode* aNode );
    string getFbxTypeString();
    
    static string getNodeTypeAsString( NodeType atype );
    string getTypeAsString();
    virtual Node::NodeType getType() { return OFX_FBX_NULL; }
    
    virtual void setup( FbxNode *pNode );
    string getName();
    FbxString getFbxName();
    void setName( FbxString aName );
    
    void setUseKeyFrames( bool ab );
    bool usingKeyFrames();
    virtual void clearKeyFrames();
    
    void cacheStartTransforms();
    
    virtual void update( FbxTime& pTime, FbxPose* pPose );
    virtual void update( int aAnimIndex, signed long aMillis );
    virtual void update( int aAnimIndex1, signed long aAnim1Millis, int aAnimIndex2, signed long aAnim2Millis, float aMixPct );
    
    glm::vec3 getKeyTranslation( int aAnimIndex, signed long aMillis );
    ofQuaternion getKeyRotation( int aAnimIndex, signed long aMillis );
    glm::vec3 getKeyScale( int aAnimIndex, signed long aMillis );
    
    float getKeyValue( std::vector<ofxFBXSource::AnimKey<float> >& keys, signed long ms );
    ofQuaternion getKeyRotation(vector<ofxFBXSource::AnimKey<ofQuaternion> >& keys, signed long ms);
    
    ofxFBXSource::AnimKeyCollection& getKeyCollection( int aAnimIndex );
    
//    void setGlobalTransformMatrix( FbxAMatrix ainput );
    void setLocalTransformMatrix( FbxAMatrix ainput );
    
    FbxNode* getFbxNode() { return mFbxNode; }
    
    void clearChildren();
    void addChild( shared_ptr<ofxFBXSource::Node> akiddo );
    int getNumChildren();
    vector< shared_ptr<ofxFBXSource::Node> >& getChildren();
    
    virtual string getAsString( int aLevel=0 );
    
protected:
//    void _getNodeForNameRecursive( vector<string>& aNamesToFind, shared_ptr<Node>& aTarget, vector< shared_ptr<Node> >& aElements, bool bStrict );
    
    glm::quat origLocalRotation, origGlobalRotation;
    glm::mat4 origGlobalTransform;
    glm::mat4 origLocalTransform;
    
    glm::vec3 origPos, origScale;
    
    string name = "";
    int mAnimIndex = 0;
    bool bUseKeyFrames = false;
    map<int, ofxFBXSource::AnimKeyCollection> mKeyCollections;
    vector< shared_ptr<ofxFBXSource::Node> > mKids;
    
    FbxNode* mFbxNode = NULL;
};
    
}
