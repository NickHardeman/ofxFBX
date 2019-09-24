//
//  ofxFBXNode.cpp
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 10/31/13.
//
//

#include "ofxFBXNode.h"

//----------------------------------------
ofxFBXNode::ofxFBXNode() {
//    parent          = NULL;
//    globalParent    = NULL;
//    origScale = glm::vec3(1,1,1);
}

//----------------------------------------
ofxFBXNode::~ofxFBXNode() {
    if( getParent() != NULL ) {
        clearParent();
    }
    if( mParentNode ) {
        mParentNode.reset();
    }
    clearChildren();
}

//----------------------------------------
ofxFBXSource::Node::NodeType ofxFBXNode::getType() {
    return ofxFBXSource::Node::OFX_FBX_NULL;
}

//----------------------------------------
void ofxFBXNode::setup( shared_ptr<ofxFBXSource::Node> anode ) {
    name = anode->getName();
    mSrcNode = anode;
    if( mSrcNode ) {
        setTransform( anode );
    }
}

//----------------------------------------
void ofxFBXNode::setTransform( shared_ptr<ofxFBXSource::Node> anode ) {
    setTransform( anode.get() );
}

//----------------------------------------
void ofxFBXNode::setTransform( ofxFBXSource::Node* anode ) {
    setPosition( anode->getPosition() );
    setOrientation( anode->getOrientationQuat() );
    setScale( anode->getScale() );
}

//----------------------------------------
string ofxFBXNode::getName() {
    return name;
}

//----------------------------------------
string ofxFBXNode::getFbxTypeString() {
    if(mSrcNode) {
        return mSrcNode->getFbxTypeString();
    }
    return "Unknown";
}

//----------------------------------------
void ofxFBXNode::setParentNode( shared_ptr<ofxFBXNode> anode ) {
    mParentNode = anode;
}

//----------------------------------------
bool ofxFBXNode::hasParentNode() {
    if( !mParentNode ) return false;
    return true;
}

//----------------------------------------
shared_ptr<ofxFBXNode> ofxFBXNode::getParentNode() {
    return mParentNode;
}

//----------------------------------------
void ofxFBXNode::clearChildren() {
    mKids.clear();
}

//----------------------------------------
void ofxFBXNode::addChild( shared_ptr<ofxFBXNode> akiddo ) {
    mKids.push_back( akiddo );
}

//----------------------------------------
int ofxFBXNode::getNumChildren() {
    return mKids.size();
}

//----------------------------------------
vector< shared_ptr<ofxFBXNode> >& ofxFBXNode::getChildren() {
    return mKids;
}

//--------------------------------------------------------------
string ofxFBXNode::getAsString( int aLevel ) {
    stringstream oStr;// = "";
    for( int i = 0; i < aLevel; i++ ) {
        oStr << "  ";
    }
    if( aLevel > 0 ) {
        oStr <<" '";
    }
    if( mKids.size() ) {
        oStr << "+ ";
    } else {
        oStr << "- ";
    }
//    string pname = "";
//    if( getParent() != NULL ) {
//        pname = " parent: " + parentBoneName;
//    }
    
    if( !mSrcNode ) {
        return oStr.str();
    }
    
    oStr << mSrcNode->getTypeAsString() << ": " << getName() << " fbx type: " << mSrcNode->getFbxTypeString();
    if( getNumChildren() > 0 ) {
        oStr << " kids: " << mKids.size();
    }
//    if(mSrcNode->usingKeyFrames()) {
//        oStr << " num keys: " << mSrcNode->getKey.size();
//    }
    oStr << endl;// "\n";
    
    for( auto& kid : mKids ) {
        oStr << kid->getAsString( aLevel + 1);
    }
    
    return oStr.str();
}

#pragma mark - Search
//--------------------------------------------------------------
shared_ptr<ofxFBXNode> ofxFBXNode::getNodeforName( shared_ptr<ofxFBXNode>& aBSelf, string aPath, bool bStrict ) {
    vector< string > tsearches;
    if( ofIsStringInString( aPath, ":" ) ) {
        tsearches = ofSplitString( aPath, ":" );
    } else {
        tsearches.push_back( aPath );
        if(aBSelf) {
            if(bStrict) {
                if( aBSelf->getName() == aPath ) {
//                    cout << "FOUND SELF" << endl;
                    return aBSelf;
                }
            } else {
                if( ofIsStringInString( aBSelf->getName(), aPath )) {
//                    cout << "FOUND SELF" << endl;
                    return aBSelf;
                }
            }
        }
    }
    
    shared_ptr<ofxFBXNode> temp;// = aBSelf;
    _getNodeForNameRecursive( tsearches, temp, mKids, bStrict );
    return temp;
}

//--------------------------------------------------------------
shared_ptr<ofxFBXNode> ofxFBXNode::getKidforName( string aPath, bool bStrict ) {
    
    vector< string > tsearches;
    if( ofIsStringInString( aPath, ":" ) ) {
        tsearches = ofSplitString( aPath, ":" );
    } else {
        tsearches.push_back( aPath );
    }
    
    shared_ptr<ofxFBXNode> temp;
    _getNodeForNameRecursive( tsearches, temp, mKids, bStrict );
    return temp;
}

//--------------------------------------------------------------
void ofxFBXNode::_getNodeForNameRecursive( vector<string>& aNamesToFind, shared_ptr<ofxFBXNode>& aTarget, vector< shared_ptr<ofxFBXNode> >& aElements, bool bStrict ) {
    
    for( int i = 0; i < aElements.size(); i++ ) {
        bool bFound = false;
        if(bStrict) {
            if( aElements[i]->getName() == aNamesToFind[0] ) {
                bFound = true;
            }
        } else {
            if( ofIsStringInString( aElements[i]->getName(), aNamesToFind[0] )) {
//                cout << "Found--- " << aNamesToFind[0] << " num names: " << aNamesToFind.size() << endl;
                bFound = true;
            }
        }
        
        if( bFound == true ) {
            aNamesToFind.erase( aNamesToFind.begin() );
            if( aNamesToFind.size() == 0 ) {//}|| aElements[i]->getNumChildren() < 1 ) {
                bool bgood = false;
                if( aElements[i] ) {
                    bgood = true;
                }
//                cout << "going to return one of the elements " << aNamesToFind.size() << " good: " << bgood << " " << endl;
                aTarget = aElements[i];
                break;
            } else {
                if( aElements[i]->getNumChildren() > 0 ) {
//                    shared_ptr<ofxFBXNode> tgroup = dynamic_pointer_cast< ofxSvgGroup >( aElements[i] );
                    _getNodeForNameRecursive( aNamesToFind, aTarget, aElements[i]->getChildren(), bStrict );
                    break;
                }
            }
        }
    }
}

//--------------------------------------------------------------
void ofxFBXNode::_getKidsForTypeRecursive( int atype, string aNameToContain, vector< shared_ptr<ofxFBXNode> >& aFoundElements, vector< shared_ptr<ofxFBXNode> >& aElements ) {
    
    for( int i = 0; i < aElements.size(); i++ ) {
        
        if( aElements[i]->getType() == atype ) {
            bool bFound = false;
            if( aNameToContain != "" ) {
                if( ofIsStringInString( aElements[i]->getName(), aNameToContain )) {
                    bFound = true;
                }
            } else {
                bFound = true;
            }
            if( bFound ) {
                aFoundElements.push_back( aElements[i] );
            }
        }
        
        if( aElements[i]->getNumChildren() > 0 ) {
            _getKidsForTypeRecursive( atype, aNameToContain, aFoundElements, aElements[i]->getChildren() );
        }
    }
}

//--------------------------------------------------------------
vector< shared_ptr<ofxFBXNode> > ofxFBXNode::getAllChildren() {
    vector< shared_ptr<ofxFBXNode> > rnodes;
    _getKidsRecursive( rnodes, mKids );
    return rnodes;
}

//--------------------------------------------------------------
void ofxFBXNode::_getKidsRecursive( vector< shared_ptr<ofxFBXNode> >& aFoundElements, vector< shared_ptr<ofxFBXNode> >& aElements ) {
    for( int i = 0; i < aElements.size(); i++ ) {
        aFoundElements.push_back( aElements[i] );
        
        if( aElements[i]->getNumChildren() > 0 ) {
            _getKidsRecursive( aFoundElements, aElements[i]->getChildren() );
        }
    }
}
