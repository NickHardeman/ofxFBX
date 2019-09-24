//
//  Node.cpp
//  ConnectionsWall-Nick
//
//  Created by Nick Hardeman on 7/11/19.
//

#include "ofxFBXSrcNode.h"
using namespace ofxFBXSource;

//----------------------------------------
Node::Node() {
    //    parent          = NULL;
    //    globalParent    = NULL;
    origScale = glm::vec3(1,1,1);
}

//----------------------------------------
Node::~Node() {
    if( getParent() != NULL ) {
        clearParent();
    }
}

//----------------------------------------
// this is painful :( -- probably should make an array of names
string Node::getFbxTypeStringFromNode( FbxNode* aNode ) {
    if( aNode == NULL ) { return "Node is NULL";}
    FbxNodeAttribute* lNodeAttribute = aNode->GetNodeAttribute();
    if( lNodeAttribute ) {
        
        //        -eUnknown,
        //        -eNull,
        //        -eMarker,
        //        -eSkeleton,
        //        -eMesh,
        //        -eNurbs,
        //        -ePatch,
        //        -eCamera,
        //        -eCameraStereo,
        //        -eCameraSwitcher,
        //        -eLight,
        //        -eOpticalReference,
        //        -eOpticalMarker,
        //        -eNurbsCurve,
        //        -eTrimNurbsSurface,
        //        -eBoundary,
        //        -eNurbsSurface,
        //        -eShape,
        //        -eLODGroup,
        //        -eSubDiv,
        //        -eCachedEffect,
        //        -eLine
        
        FbxNodeAttribute::EType et = lNodeAttribute->GetAttributeType();
        if( et == FbxNodeAttribute::eUnknown ) {
            return "eUnknown";
        }
        if( et == FbxNodeAttribute::eNull ) {
            return "eNull";
        }
        if( et == FbxNodeAttribute::eMarker ) {
            return "eMarker";
        }
        if( et == FbxNodeAttribute::eSkeleton ) {
            return "eSkeleton";
        }
        if (et == FbxNodeAttribute::eMesh ) {
            return "eMesh";
        }
        if( et == FbxNodeAttribute::eNurbs ) {
            return "eNurbs";
        }
        if(et == FbxNodeAttribute::ePatch) {
            return "ePatch";
        }
        if(et == FbxNodeAttribute::eCamera) {
            return "eCamera";
        }
        if(et == FbxNodeAttribute::eCameraStereo) {
            return "eCameraStereo";
        }
        if(et == FbxNodeAttribute::eCameraSwitcher) {
            return "eCameraSwitcher";
        }
        if(et == FbxNodeAttribute::eLight) {
            return "eLight";
        }
        if( et == FbxNodeAttribute::eOpticalReference ) {
            return "eOpticalReference";
        }
        if( et == FbxNodeAttribute::eOpticalMarker ) {
            return "eOpticalMarker";
        }
        if( et == FbxNodeAttribute::eNurbsCurve ) {
            return "eNurbsCurve";
        }
        if( et == FbxNodeAttribute::eTrimNurbsSurface ) {
            return "eTrimNurbsSurface";
        }
        if( et == FbxNodeAttribute::eBoundary ) {
            return "eBoundary";
        }
        if(et == FbxNodeAttribute::eNurbsSurface) {
            return "eNurbsSurface";
        }
        if( et == FbxNodeAttribute::eShape ) {
            return "eShape";
        }
        if( et == FbxNodeAttribute::eLODGroup ) {
            return "eLODGroup";
        }
        if( et == FbxNodeAttribute::eSubDiv ) {
            return "eSubDiv";
        }
        if( et == FbxNodeAttribute::eCachedEffect ) {
            return "eCachedEffect";
        }
        if( et == FbxNodeAttribute::eLine ) {
            return "eLine";
        }
        return ("default - "+ofToString((int)et));
    }
    return "default - no attribute";
}

//----------------------------------------
string Node::getFbxTypeString() {
    return Node::getFbxTypeStringFromNode( mFbxNode );
}

//----------------------------------------
string Node::getNodeTypeAsString( NodeType atype ) {
    if( atype == OFX_FBX_NULL ) {
        return "Null";
    } else if( atype == OFX_FBX_MESH ) {
        return "Mesh";
    } else if( atype == OFX_FBX_SKELETON ) {
        return "Skeleton";
    } else if( atype == OFX_FBX_BONE ) {
        return "Bone";
    } else if( atype == OFX_FBX_NURBS_CURVE ) {
        return "Nurbs Curve";
    }
    //OFX_FBX_UNKNOWN
    return "Unknown";
}

//----------------------------------------
string Node::getTypeAsString() {
    return Node::getNodeTypeAsString(getType());
}

//----------------------------------------
void Node::setup( FbxNode *pNode ) {
    setName( pNode->GetNameOnly() );
    mFbxNode = pNode;
}

//----------------------------------------
string Node::getName() {
    return name;
}

//----------------------------------------
FbxString Node::getFbxName() {
    return FbxString( name.c_str() );
}

//----------------------------------------
void Node::setName( FbxString aName ) {
    name = aName;
}

//--------------------------------------------------------------
void Node::setUseKeyFrames( bool ab ) {
    bUseKeyFrames = ab;
}

//--------------------------------------------------------------
bool Node::usingKeyFrames() {
    return bUseKeyFrames;
}

//--------------------------------------------------------------
void Node::clearKeyFrames() {
    mKeyCollections.clear();
}

//--------------------------------------------------------------
void Node::cacheStartTransforms() {
    // cache the orientations for use later //
    origGlobalRotation  = getGlobalOrientation();
    origLocalRotation   = getOrientationQuat();
    origGlobalTransform = getGlobalTransformMatrix();
    origLocalTransform  = getLocalTransformMatrix();
    origPos             = getPosition();
    origScale           = getScale();
}

//----------------------------------------
void Node::update( FbxTime& pTime, FbxPose* pPose ) {
    if( mFbxNode ) {
        if( !mFbxNode->GetParent() ) {
            FbxAMatrix lGlobalPosition = GetGlobalPosition(mFbxNode, pTime, NULL );
            setLocalTransformMatrix(lGlobalPosition);
        } else {
            FbxAMatrix lLocalPosition = GetLocalPositionForNode(mFbxNode, pTime, NULL );
            setLocalTransformMatrix(lLocalPosition);
        }
    }
}

//----------------------------------------
void Node::update( int aAnimIndex, signed long aMillis ) {
    if( aAnimIndex < 0 ) return;
    if( mKeyCollections.size() == 0 ) return;
    if( aAnimIndex >= mKeyCollections.size() ) return;
    
    glm::vec3 tpos = getKeyTranslation( aAnimIndex, aMillis );
    glm::vec3 cpos = getPosition();
    if( cpos.x != tpos.x || cpos.y != tpos.y || cpos.z != tpos.z ) {
        setPosition( tpos );
    }
    
    glm::vec3 tscale = getKeyScale( aAnimIndex, aMillis );
    glm::vec3 cscale = getScale();
    if( cscale.x != tscale.x || cscale.y != tscale.y || cscale.z != tscale.z ) {
        setScale( tscale );
    }
    
    ofQuaternion tquat = getKeyRotation( aAnimIndex, aMillis );
    ofQuaternion cquat = getOrientationQuat();
    if( cquat.x() != tquat.x() || cquat.y() != tquat.y() || cquat.z() != tquat.z() || cquat.w() != tquat.w() ) {
        setOrientation( tquat );
    }
}

//----------------------------------------
void Node::update( int aAnimIndex1, signed long aAnim1Millis, int aAnimIndex2, signed long aAnim2Millis, float aMixPct ) {
    if( mKeyCollections.size() == 0 ) return;
    if( aAnimIndex1 < 0 ) return;
    if( aAnimIndex1 >= mKeyCollections.size() ) return;
    if( aAnimIndex2 < 0 ) return;
    if( aAnimIndex2 >= mKeyCollections.size() ) return;
    
    aMixPct = ofClamp(aMixPct, 0.0, 1.0);
    float invpct = 1.0 - aMixPct;
    
    glm::vec3 tpos1 = getKeyTranslation( aAnimIndex1, aAnim1Millis );
    glm::vec3 tpos2 = getKeyTranslation( aAnimIndex2, aAnim2Millis);
    
    glm::vec3 tscale1 = getKeyScale( aAnimIndex1, aAnim1Millis );
    glm::vec3 tscale2 = getKeyScale( aAnimIndex2, aAnim2Millis );
    
    ofQuaternion tquat1 = getKeyRotation( aAnimIndex1, aAnim1Millis );
    ofQuaternion tquat2 = getKeyRotation( aAnimIndex2, aAnim2Millis );
    
    
    if( aMixPct <= 0.0f ) {
        setPosition( tpos1 );
        setScale(tscale1);
        setOrientation(tquat1);
    } else if( aMixPct >= 1.0 ) {
        setPosition( tpos2 );
        setScale(tscale2);
        setOrientation(tquat2);
    } else {
        setPosition( tpos1 * invpct + tpos2 * aMixPct );
        setScale( tscale1 * invpct + tscale2 * aMixPct );
        tquat1.slerp( aMixPct, tquat1, tquat2 );
        setOrientation( tquat1 );
    }
}

//----------------------------------------
glm::vec3 Node::getKeyTranslation( int aAnimIndex, signed long aMillis ) {
    if( aAnimIndex < 0 ) return origPos;
    if( mKeyCollections.size() == 0 ) return origPos;
    if( aAnimIndex >= mKeyCollections.size() ) return origPos;
    
    AnimKeyCollection& tcollection = mKeyCollections[aAnimIndex];
    mAnimIndex = aAnimIndex;
    glm::vec3 tpos = origPos;
    if(tcollection.posKeysX.size() > 0) tpos.x = getKeyValue( tcollection.posKeysX, aMillis );
    if(tcollection.posKeysY.size() > 0) tpos.y = getKeyValue( tcollection.posKeysY, aMillis );
    if(tcollection.posKeysZ.size() > 0) tpos.z = getKeyValue( tcollection.posKeysZ, aMillis );
    return tpos;
}

//----------------------------------------
ofQuaternion Node::getKeyRotation( int aAnimIndex, signed long aMillis ) {
    if( aAnimIndex < 0 ) return origLocalRotation;
    if( mKeyCollections.size() == 0 ) return origLocalRotation;
    if( aAnimIndex >= mKeyCollections.size() ) return origLocalRotation;
    
    AnimKeyCollection& tcollection = mKeyCollections[aAnimIndex];
    mAnimIndex = aAnimIndex;
    
    return getKeyRotation( tcollection.rotKeys, aMillis );
}

//----------------------------------------
glm::vec3 Node::getKeyScale( int aAnimIndex, signed long aMillis ) {
    if( aAnimIndex < 0 ) return origScale;
    if( mKeyCollections.size() == 0 ) return origScale;
    if( aAnimIndex >= mKeyCollections.size() ) return origScale;
    
    AnimKeyCollection& tcollection = mKeyCollections[aAnimIndex];
    mAnimIndex = aAnimIndex;
    
    glm::vec3 tscale = origScale;
    if(tcollection.scaleKeysX.size() > 0) tscale.x = getKeyValue( tcollection.scaleKeysX, aMillis );
    if(tcollection.scaleKeysY.size() > 0) tscale.y = getKeyValue( tcollection.scaleKeysY, aMillis );
    if(tcollection.scaleKeysZ.size() > 0) tscale.z = getKeyValue( tcollection.scaleKeysZ, aMillis );
    
    return tscale;
}

//----------------------------------------
// from Arturo Castro's ofxFBX ///
float Node::getKeyValue( vector<AnimKey<float> >& keys, signed long ms ) {
    for(int i=0;i<keys.size();i++){
        if(keys[i].millis==ms){
            return keys[i].value;
        }else if(keys[i].millis>ms){
            if(i>0){
                signed long delta = ms - keys[i-1].millis;
                float pct = double(delta) / double(keys[i].millis - keys[i-1].millis);
                return ofLerp(keys[i-1].value,keys[i].value,pct);
            }else{
                return keys[0].value;
                //u_long delta = ms;
                //float pct = double(delta) / double(keys[i].millis);
                //return ofLerp(0.0f,keys[i].value,pct);
            }
        }
    }
    if(keys.empty()){
        return 0.0f;
    }else{
        return keys.back().value;
    }
    return 0.0f;
}

//----------------------------------------
// from Arturo Castro's ofxFBX ///
ofQuaternion Node::getKeyRotation(vector<AnimKey<ofQuaternion> >& keys, signed long ms) {
    for(int i=0;i<keys.size();i++){
        if(keys[i].millis==ms){
            return keys[i].value;
        }else if(keys[i].millis>ms){
            if(i>0){
                signed long delta = ms - keys[i-1].millis;
                float pct = double(delta) / double(keys[i].millis - keys[i-1].millis);
                ofQuaternion q;
                q.slerp(pct,keys[i-1].value,keys[i].value);
                return q;
            }else{
                signed long delta = ms;
                float pct = double(delta) / double(keys[i].millis);
                ofQuaternion q = keys[i].value;
                q.slerp(pct,origLocalRotation,keys[i].value);
                return q;
            }
        }
    }
    if(keys.empty()){
        return origLocalRotation;
    }else{
        return keys.back().value;
    }
    return origLocalRotation;
}

//----------------------------------------
AnimKeyCollection& Node::getKeyCollection( int aAnimIndex ) {
    if( mKeyCollections.count(aAnimIndex) < 1 ) {
        AnimKeyCollection temp;
        mKeyCollections[ aAnimIndex ] = temp;
    }
    return mKeyCollections[aAnimIndex];
}

//----------------------------------------
//void Node::setGlobalTransformMatrix( FbxAMatrix ainput ) {
//    glm::vec3 tpos, tscale;
//    glm::quat trot;
//    
//    fbxToGlmComponents( ainput, tpos, trot, tscale );
//    
//    setScale( tscale );
//    setGlobalPosition( tpos );
//    setGlobalOrientation( trot);
//}

//----------------------------------------
void Node::setLocalTransformMatrix( FbxAMatrix ainput ) {
    glm::vec3 tpos, tscale;
    glm::quat trot;
    
    fbxToGlmComponents( ainput, tpos, trot, tscale );
    
    setScale( tscale );
    setPosition( tpos );
    setOrientation( trot);
}

//----------------------------------------
void Node::clearChildren() {
    mKids.clear();
}

//----------------------------------------
void Node::addChild( shared_ptr<Node> akiddo ) {
    mKids.push_back( akiddo );
}

//----------------------------------------
int Node::getNumChildren() {
    return mKids.size();
}

//----------------------------------------
vector< shared_ptr<Node> >& Node::getChildren() {
    return mKids;
}

//--------------------------------------------------------------
string Node::getAsString( int aLevel ) {
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
    oStr << getTypeAsString() << ": " << getName() << " fbx type: " << getFbxTypeString();
    if( getNumChildren() > 0 ) {
        oStr << " kids: " << mKids.size();
    }
    if(usingKeyFrames()) {
        oStr << " num keys collections: " << mKeyCollections.size();
    }
    oStr << endl;// "\n";
    
    for( auto& kid : mKids ) {
        oStr << kid->getAsString( aLevel + 1);
    }
    
    return oStr.str();
}

//#pragma mark - Search
//--------------------------------------------------------------
//shared_ptr<Node> Node::getNodeforName( shared_ptr<Node>& aBSelf, string aPath, bool bStrict ) {
//    vector< string > tsearches;
//    if( ofIsStringInString( aPath, ":" ) ) {
//        tsearches = ofSplitString( aPath, ":" );
//    } else {
//        tsearches.push_back( aPath );
//        if(aBSelf) {
//            if(bStrict) {
//                if( aBSelf->getName() == aPath ) {
//                    //                    cout << "FOUND SELF" << endl;
//                    return aBSelf;
//                }
//            } else {
//                if( ofIsStringInString( aBSelf->getName(), aPath )) {
//                    //                    cout << "FOUND SELF" << endl;
//                    return aBSelf;
//                }
//            }
//        }
//    }
//
//    shared_ptr<Node> temp;// = aBSelf;
//    _getNodeForNameRecursive( tsearches, temp, mKids, bStrict );
//    return temp;
//}
//
////--------------------------------------------------------------
//shared_ptr<Node> Node::getKidforName( string aPath, bool bStrict ) {
//
//    vector< string > tsearches;
//    if( ofIsStringInString( aPath, ":" ) ) {
//        tsearches = ofSplitString( aPath, ":" );
//    } else {
//        tsearches.push_back( aPath );
//    }
//
//    shared_ptr<Node> temp;
//    _getNodeForNameRecursive( tsearches, temp, mKids, bStrict );
//    return temp;
//}
//
////--------------------------------------------------------------
//void Node::_getNodeForNameRecursive( vector<string>& aNamesToFind, shared_ptr<Node>& aTarget, vector< shared_ptr<Node> >& aElements, bool bStrict ) {
//
//    for( int i = 0; i < aElements.size(); i++ ) {
//        bool bFound = false;
//        if(bStrict) {
//            if( aElements[i]->getName() == aNamesToFind[0] ) {
//                bFound = true;
//            }
//        } else {
//            if( ofIsStringInString( aElements[i]->getName(), aNamesToFind[0] )) {
//                //                cout << "Found--- " << aNamesToFind[0] << " num names: " << aNamesToFind.size() << endl;
//                bFound = true;
//            }
//        }
//
//        if( bFound == true ) {
//            aNamesToFind.erase( aNamesToFind.begin() );
//            if( aNamesToFind.size() == 0 ) {//}|| aElements[i]->getNumChildren() < 1 ) {
//                bool bgood = false;
//                if( aElements[i] ) {
//                    bgood = true;
//                }
//                //                cout << "going to return one of the elements " << aNamesToFind.size() << " good: " << bgood << " " << endl;
//                aTarget = aElements[i];
//                break;
//            } else {
//                if( aElements[i]->getNumChildren() > 0 ) {
////                    shared_ptr<Node> tgroup = dynamic_pointer_cast< ofxSvgGroup >( aElements[i] );
//                    _getNodeForNameRecursive( aNamesToFind, aTarget, aElements[i]->getChildren(), bStrict );
//                    break;
//                }
//            }
//        }
//    }
//
//
//}
