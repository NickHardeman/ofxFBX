//
//  ofxFBXSrcBone.cpp
//  ConnectionsWall-Nick
//
//  Created by Nick Hardeman on 7/11/19.
//

#include "ofxFBXSrcBone.h"
#include "ofxFBXUtils.h"
using namespace ofxFBXSource;

//--------------------------------------------------------------
Bone::Bone() {
    bExists         = false;
    //    sourceBone      = NULL;
    parentBoneName  = "";
    bIsRoot         = false;
    enableAnimation();
}

//--------------------------------------------------------------
Bone::~Bone() {
//    map<string, ofxFBXBone >::iterator it;
//    for(it = bones.begin(); it != bones.end(); ++it ) {
//        it->second.clearParent();
//    }
//
//    if( getParent() != NULL ) {
//        clearParent();
//    }
}

//--------------------------------------------------------------
bool Bone::doesExist() {
    return bExists;
}

//--------------------------------------------------------------
void Bone::setAsRoot() {
    bIsRoot = true;
}

//--------------------------------------------------------------
void Bone::setup( FbxNode* pNode ) {
    ofxFBXSource::Node::setup( pNode );
    //    fbxNode = pNode;
    
    //    setTransformMatrix( ofGetGlobalTransform( fbxNode, FBXSDK_TIME_INFINITE, NULL ) );
    setLocalTransformMatrix( fbxGetGlobalTransform( mFbxNode, FBXSDK_TIME_INFINITE, NULL ) );
    bExists = true;
    
    updateFbxTransform();
}

//--------------------------------------------------------------
void Bone::update( FbxTime& pTime, FbxPose* pPose ) {
    
//    if(bIsRoot) {
//        cout << "ofxFBXSource::Bone :: update : is root : " << getName() << " animations: " << isAnimationEnabled() << endl;
//    }
    
    if( isAnimationEnabled() ) {
        if( !bIsRoot ) {
            //            setTransformMatrix( ofGetLocalTransform( fbxNode, pTime, pPose, NULL ));
            FbxAMatrix& tmatrix = mFbxNode->EvaluateLocalTransform( pTime );
            //            setTransformMatrix( fbxToOf(tmatrix) );
            //            setTransformMatrix(tmatrix);
            setLocalTransformMatrix(tmatrix);
            //            setGlobalTransformMatrix(tmatrix);
        } else {
//            cout << "ofxFBXSource::Bone :: update : is root : " << getName() << endl;
            FbxAMatrix& tmatrix = mFbxNode->EvaluateGlobalTransform( pTime );
            //            setTransformMatrix(tmatrix);
            setLocalTransformMatrix(tmatrix);
            //            setTransformMatrix( fbxToOf(tmatrix) );
            
            //            setTransformMatrix( ofGetLocalTransform( fbxNode, pTime, pPose, NULL ));
            //FbxAMatrix& tmatrix = fbxNode->EvaluateLocalTransform( pTime );
            //setTransformMatrix( toOf(tmatrix) );
        }
    }
    
    map<string, shared_ptr<Bone> >::iterator it;
    for(it = childBones.begin(); it != childBones.end(); ++it ) {
        it->second->update( pTime, pPose );
    }
}

//--------------------------------------------------------------
void Bone::update( int aAnimIndex, signed long aMillis ) {
    if( isAnimationEnabled() ) {
        ofxFBXSource::Node::update( aAnimIndex, aMillis );
//        if( ofGetFrameNum() > 300 ) {
//            cout << "ofxFBXSource::Bone :: update : " << getName() << " isAnimationEnabled(): " << isAnimationEnabled() << " | " << ofGetFrameNum() << endl;
//        }
    }
    
    if( bIsRoot ) {
//        cout << "ofxFBXSource::Bone :: update : is root : " << getName() << " animations: " << isAnimationEnabled() << " skel root: " << mFbxNode->GetSkeleton()->IsSkeletonRoot() << " | " << ofGetFrameNum() << endl;
//        if( getParent() != NULL ) {
//            cout << " getParent position: " << getParent()->getPosition() << endl;
//        }
        
    }
    
    map<string, shared_ptr<Bone> >::iterator it;
    for(it = childBones.begin(); it != childBones.end(); ++it ) {
        it->second->update( aAnimIndex, aMillis );
    }
}

//--------------------------------------------------------------
void Bone::update( int aAnimIndex1, signed long aAnim1Millis, int aAnimIndex2, signed long aAnim2Millis, float aMixPct ) {
    if( isAnimationEnabled() ) {
        ofxFBXSource::Node::update( aAnimIndex1, aAnim1Millis, aAnimIndex2, aAnim2Millis, aMixPct );
    }
    
    map<string, shared_ptr<Bone> >::iterator it;
    for(it = childBones.begin(); it != childBones.end(); ++it ) {
        it->second->update( aAnimIndex1, aAnim1Millis, aAnimIndex2, aAnim2Millis, aMixPct );
    }
}

//--------------------------------------------------------------
void Bone::lateUpdate() {
//    ofxFBXBone* sbone   = sourceBone;
//    if( sbone != NULL ) {
//        setPosition( getPosition() );
//        setOrientation( getOrientationQuat() );
//        setScale( getScale() );
//        sbone->setGlobalPosition( getGlobalPosition() );
//        sbone->setGlobalOrientation( getGlobalOrientation() );
//        sbone->setScale( getScale() );
//        sbone->setGlobalSca( getGlobalScale() );

//        sbone->setTransformMatrix( getLocalTransformMatrix() );
//        sbone->setTransformMatrix( getGlobalTransformMatrix() );
    updateFbxTransform();
    //    }
    
    map<string, shared_ptr<Bone> >::iterator it;
    for(it = childBones.begin(); it != childBones.end(); ++it ) {
        it->second->lateUpdate();
    }
}

//--------------------------------------------------------------
//void Bone::draw( float aLen, bool aBDrawAxes ) {
//    if(aBDrawAxes) {
//        transformGL(); {
//            ofDrawAxis( aLen );
//        } restoreTransformGL();
//    }
//    //    if(!hasSkeletonParent()) return;
//    //    if(!isLimb()) return;
//    if( getParent() != NULL && !bIsRoot ) {
//        //ofSetColor(255, 0, 130 );
//        glm::vec3 ppos = getParent()->getGlobalPosition();
//        ofDrawLine( ppos, getGlobalPosition() );
//    }
//
//    map< string, Bone* >::iterator it;
//    for(it = childBones.begin(); it != childBones.end(); ++it ) {
//        it->second->draw( aLen );
//    }
//}

//--------------------------------------------------------------
void Bone::updateFbxTransform() {
    // we need to convert this back before sending //
    fbxTransform = toFbx( getGlobalPosition(), getGlobalOrientation(), getGlobalScale() );
}

//--------------------------------------------------------------
void Bone::clearKeyFrames() {
    ofxFBXSource::Node::clearKeyFrames();
    
    map<string, shared_ptr<ofxFBXSource::Bone> >::iterator it;
    for(it = childBones.begin(); it != childBones.end(); ++it ) {
        it->second->clearKeyFrames();
    }
}

//--------------------------------------------------------------
bool Bone::isLimb() {
    FbxSkeleton* lSkeleton = getFbxSkeleton();
    if(lSkeleton) {
        lSkeleton->GetSkeletonType() == FbxSkeleton::eLimbNode;
        return true;
    }
    return false;
}

//--------------------------------------------------------------
bool Bone::hasSkeletonParent() {
    if(!mFbxNode->GetParent()) return false;
    if(!mFbxNode->GetParent()->GetNodeAttribute()) return false;
    return mFbxNode->GetParent()->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton;
}

//--------------------------------------------------------------
FbxSkeleton* Bone::getFbxSkeleton() {
    return (FbxSkeleton*) mFbxNode->GetNodeAttribute();
}

//--------------------------------------------------------------
void Bone::enableAnimation( bool bRecursively ) {
    bUpdateFromAnimation = true;
    if( bRecursively ) {
        map< string, shared_ptr<ofxFBXSource::Bone> >::iterator it;
        for(it = childBones.begin(); it != childBones.end(); ++it ) {
            it->second->enableAnimation( bRecursively );
        }
    }
}

//--------------------------------------------------------------
void Bone::disableAnimation( bool bRecursively) {
    bUpdateFromAnimation = false;
    if( bRecursively ) {
        map< string, shared_ptr<ofxFBXSource::Bone> >::iterator it;
        for(it = childBones.begin(); it != childBones.end(); ++it ) {
            it->second->disableAnimation( bRecursively );
        }
    }
}

//--------------------------------------------------------------
bool Bone::isAnimationEnabled() {
    return bUpdateFromAnimation;
}

//--------------------------------------------------------------
glm::quat& Bone::getOriginalLocalRotation() {
    return origLocalRotation;
}

//--------------------------------------------------------------
int Bone::getNumBones() {
    int ttotal = childBones.size();
    map<string, shared_ptr<Bone> >::iterator it;
    for(it = childBones.begin(); it != childBones.end(); ++it ) {
        ttotal += it->second->getNumBones();
    }
    return ttotal;
}

//--------------------------------------------------------------
//map< string, shared_ptr<Bone> > Bone::getAllBones() {
//    map< string, shared_ptr<Bone> > tbones;
//    tbones[ getName() ] = this;
//    populateBonesRecursive( tbones );
//    return tbones;
//}

//--------------------------------------------------------------
void Bone::populateBonesRecursive( map< string, shared_ptr<Bone> >& aBoneMap ) {
    
    map<string, shared_ptr<Bone> >::iterator it;
    for(it = childBones.begin(); it != childBones.end(); ++it ) {
        if( !aBoneMap.count(it->first) ) {
            aBoneMap[ it->first ] = (it->second);
        }
        it->second->populateBonesRecursive( aBoneMap );
    }
}

//--------------------------------------------------------------
string Bone::getAsString( int aLevel ) {
    stringstream oStr;// = "";
    for( int i = 0; i < aLevel; i++ ) {
        oStr << "  ";
    }
    if( aLevel > 0 ) {
        oStr <<" '";
    }
    if( childBones.size() ) {
        oStr << "+ ";
    } else {
        oStr << "- ";
    }
    string pname = "";
    if( getParent() != NULL ) {
        pname = " parent: " + parentBoneName;
    }
    
    oStr << getTypeAsString() << ": " << getName() << pname << " fbx type: " << getFbxTypeString() << " anim: " << isAnimationEnabled();
    if( childBones.size() > 0 ) {
        oStr << " child bones: " << childBones.size();
    }
    if(usingKeyFrames()) {
        oStr << " num keys: " << mKeyCollections.size();
    }
    oStr << endl;
    
    //    oStr << getTypeAsString()+": " + getName() + " kids: " +ofToString( childBones.size(), 0) + pname + " anim: " + ofToString( isAnimationEnabled(), 0) + " num keys: " +ofToString(mKeyCollections.size(),0) + "\n";
    
    map<string, shared_ptr<Bone> >::iterator it;
    for(it = childBones.begin(); it != childBones.end(); ++it ) {
        oStr << it->second->getAsString( aLevel + 1);
    }
    return oStr.str();
}

//--------------------------------------------------------------
shared_ptr<Bone> Bone::getBone( string aName ) {
    shared_ptr<Bone> tbone;// = NULL;
    //    map< string, ofxFBXBone >::iterator it;
    findBoneRecursive( aName, tbone );
    if( tbone ) {
        return tbone;
    }
    return NULL;
}

//--------------------------------------------------------------
void Bone::findBoneRecursive( string aName, shared_ptr<Bone>& returnBone ) {
    
    if( !returnBone ) {
        map< string, shared_ptr<Bone> >::iterator it;
        for(it = childBones.begin(); it != childBones.end(); ++it ) {
            if( it->second->getName() == aName ) {
                returnBone = (it->second);
                break;
            }
            it->second->findBoneRecursive( aName, returnBone );
        }
    }
}
















