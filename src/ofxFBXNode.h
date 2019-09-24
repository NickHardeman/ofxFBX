//
//  ofxFBXNode.h
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 10/31/13.
//
//

#pragma once

#include "ofMain.h"
#include <fbxsdk.h>
#include "ofxFBXUtils.h"
#include "ofxFBXSrcNode.h"

class ofxFBXNode : public ofNode {
public:
    
    ofxFBXNode();
    ~ofxFBXNode();
    
    virtual ofxFBXSource::Node::NodeType getType();
    
    virtual void setup( shared_ptr<ofxFBXSource::Node> anode );
    
    void setTransform( shared_ptr<ofxFBXSource::Node> anode );
    void setTransform( ofxFBXSource::Node* anode );
    
    string getName();
    
    virtual void update( FbxTime& pTime, FbxPose* pPose ) {}
    virtual void update( int aAnimIndex, signed long aMillis ) {}
    virtual void update( int aAnimIndex1, signed long aAnim1Millis, int aAnimIndex2, signed long aAnim2Millis, float aMixPct ) {}
    
    
    virtual void update() {}
    virtual void lateUpdate( FbxTime& pTime, FbxAnimLayer * pAnimLayer, FbxPose* pPose  ) {}
    
    shared_ptr<ofxFBXSource::Node> getofxFbxSrcNode() { return mSrcNode; }
    FbxNode* getFbxNode() { return mSrcNode->getFbxNode(); }
    string getFbxTypeString();
    
    void setParentNode( shared_ptr<ofxFBXNode> anode );
    bool hasParentNode();
    shared_ptr<ofxFBXNode> getParentNode();
    
    void clearChildren();
    void addChild( shared_ptr<ofxFBXNode> akiddo );
    int getNumChildren();
    vector< shared_ptr<ofxFBXNode> >& getChildren();
    
    virtual string getAsString( int aLevel=0 );
    
    // pass in a ref to this
    shared_ptr<ofxFBXNode> getNodeforName( shared_ptr<ofxFBXNode>& aBSelf, string aPath, bool bStrict );
    shared_ptr<ofxFBXNode> getKidforName( string aPath, bool bStrict );
    
    template<typename ofxFBXNodeType>
    shared_ptr<ofxFBXNodeType> get( string aPath, bool bStrict = false ) {
        auto stemp = getKidforName( aPath, bStrict );
        if( stemp ) {
            return dynamic_pointer_cast<ofxFBXNodeType>( stemp );
        }
        shared_ptr<ofxFBXNodeType> rtemp;
        return rtemp;
    }
    
    template<typename ofxFBXNodeType>
    vector< shared_ptr<ofxFBXNodeType> > getKidsForType() {
        
        auto stemp = make_shared<ofxFBXNodeType>();
        int sType = stemp->getType();
        
        vector< shared_ptr<ofxFBXNodeType> > telements;
        for( int i = 0; i < mKids.size(); i++ ) {
            if( mKids[i]->getType() == sType ) {
                telements.push_back( dynamic_pointer_cast<ofxFBXNodeType>(mKids[i]) );
            }
        }
        return telements;
    }
    
    template<typename ofxFBXNodeType>
    vector< shared_ptr<ofxFBXNodeType> > getAllKidsForType( string aNameToContain="" ) {
        auto stemp = make_shared<ofxFBXNodeType>();
        int sType = stemp->getType();
        
        vector< shared_ptr<ofxFBXNode> > tFoundNodes;
        
        _getKidsForTypeRecursive( sType, aNameToContain, tFoundNodes, mKids );
        
        vector< shared_ptr<ofxFBXNodeType> > tReturnNodes;
        for( auto tfn : tFoundNodes ) {
            tReturnNodes.push_back( dynamic_pointer_cast<ofxFBXNodeType>(tfn) );
        }
        return tReturnNodes;
    }
    
    vector< shared_ptr<ofxFBXNode> > getAllChildren();
    
protected:
    void _getNodeForNameRecursive( vector<string>& aNamesToFind, shared_ptr<ofxFBXNode>& aTarget, vector< shared_ptr<ofxFBXNode> >& aElements, bool bStrict );
    void _getKidsForTypeRecursive( int atype, string aNameToContain, vector< shared_ptr<ofxFBXNode> >& aFoundElements, vector< shared_ptr<ofxFBXNode> >& aElements );
    void _getKidsRecursive( vector< shared_ptr<ofxFBXNode> >& aFoundElements, vector< shared_ptr<ofxFBXNode> >& aElements );
    
    string name = "default";
    vector< shared_ptr<ofxFBXNode> > mKids;
    shared_ptr<ofxFBXNode> mParentNode;
    
    shared_ptr<ofxFBXSource::Node> mSrcNode;
    
};


















