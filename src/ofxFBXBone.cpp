//
//  ofxFBXBone.cpp
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 11/1/13.
//
//

#include "ofxFBXBone.h"
#include "ofxFBXUtils.h"

//----------------------------------------
ofxFBXSource::Node::NodeType ofxFBXBone::getType() {
    return ofxFBXSource::Node::OFX_FBX_BONE;
}

//--------------------------------------------------------------------------------
void ofxFBXBone::setBoneSource( shared_ptr<ofxFBXSource::Bone> asrc, shared_ptr<ofxFBXNode> aself ) {
    ofxFBXNode::setup( asrc );
    boneSrc = asrc;
    setup( asrc );
    setPosition( boneSrc->getPosition() );
    setOrientation( boneSrc->getOrientationQuat() );
    setScale( boneSrc->getScale() );
    
    map<string, shared_ptr<ofxFBXSource::Bone> >::iterator it;
    for(it = boneSrc->childBones.begin(); it != boneSrc->childBones.end(); ++it ) {
        auto nb = make_shared<ofxFBXBone>();
        nb->setParent( *this );
        nb->setParentNode( aself );
        addChild(nb);
        childBones[ it->first ] = nb;
        nb->setBoneSource( it->second, nb );
    }
    
}

//--------------------------------------------------------------------------------
void ofxFBXBone::update() {
    if(bUpdateFromAnimation) {
        ofxFBXNode::setTransform( boneSrc );
//        cout << "ofxFBXBone :: update : " << getName() << " bUpdateFromAnimation: " << bUpdateFromAnimation << " | " << ofGetFrameNum() << endl;
    }
    
    map<string, shared_ptr<ofxFBXBone> >::iterator it;
    for(it = childBones.begin(); it != childBones.end(); ++it ) {
        it->second->update();
    }
}

//--------------------------------------------------------------------------------
void ofxFBXBone::lateUpdate() {
    if( bUpdateFromAnimation ) {
        if( boneSrc->isRoot() && getParent() != NULL ) {
            boneSrc->setPosition( getPosition() + getParent()->getPosition() );
            auto invParent = glm::inverse( getParent()->getOrientationQuat() );
            auto m44 = invParent * getOrientationQuat();
            boneSrc->setOrientation(m44);
    //        boneSrc->setOrientation( getGlobalOrientation() );
    //        boneSrc->setScale( getScale()*getParent()->getGlobalScale() );
            boneSrc->setScale( getScale() );
        } else {
            boneSrc->setPosition( getPosition() );
            boneSrc->setOrientation( getOrientationQuat() );
            boneSrc->setScale( getScale() );
        }
        boneSrc->updateFbxTransform();
    }
    
    map<string, shared_ptr<ofxFBXBone> >::iterator it;
    for(it = childBones.begin(); it != childBones.end(); ++it ) {
        it->second->lateUpdate();
    }
}

//--------------------------------------------------------------------------------
void ofxFBXBone::enableAnimation( bool bRecursively ) {
    bUpdateFromAnimation = true;
    if( bRecursively ) {
        map< string, shared_ptr<ofxFBXBone> >::iterator it;
        for(it = childBones.begin(); it != childBones.end(); ++it ) {
            it->second->enableAnimation( bRecursively );
        }
    }
}

//--------------------------------------------------------------------------------
void ofxFBXBone::disableAnimation( bool bRecursively) {
    bUpdateFromAnimation = false;
    if( bRecursively ) {
        map< string, shared_ptr<ofxFBXBone> >::iterator it;
        for(it = childBones.begin(); it != childBones.end(); ++it ) {
            it->second->disableAnimation( bRecursively );
        }
    }
}

//--------------------------------------------------------------------------------
bool ofxFBXBone::isAnimationEnabled() {
    return bUpdateFromAnimation;
}

//--------------------------------------------------------------------------------
void ofxFBXBone::draw( float aLen, bool aBDrawAxes ) {
    if(aBDrawAxes) {
        transformGL(); {
            ofDrawAxis( aLen );
        } restoreTransformGL();
    }
    
    if( boneSrc != NULL && !boneSrc->isRoot() ) {
        glm::vec3 ppos = getParent()->getGlobalPosition();
        ofDrawLine( ppos, getGlobalPosition() );
    }
    
    map< string, shared_ptr<ofxFBXBone> >::iterator it;
    for(it = childBones.begin(); it != childBones.end(); ++it ) {
        it->second->draw( aLen, aBDrawAxes );
    }
}

//---------------------------------------------------------------
int ofxFBXBone::getNumBones() {
    int ttotal = childBones.size();
    map<string, shared_ptr<ofxFBXBone> >::iterator it;
    for(it = childBones.begin(); it != childBones.end(); ++it ) {
        ttotal += it->second->getNumBones();
    }
    return ttotal;
}

//--------------------------------------------------------------------------------
shared_ptr<ofxFBXBone> ofxFBXBone::getBone( string aName ) {
    shared_ptr<ofxFBXBone> tbone;// = NULL;
    findBoneRecursive( aName, tbone );
    if( tbone ) {
        return tbone;
    }
    return NULL;
}

//--------------------------------------------------------------------------------
void ofxFBXBone::findBoneRecursive( string aName, shared_ptr<ofxFBXBone>& returnBone ) {
    
    if( !returnBone ) {
        map< string, shared_ptr<ofxFBXBone> >::iterator it;
        for(it = childBones.begin(); it != childBones.end(); ++it ) {
            if( it->second->name == aName ) {
                returnBone = (it->second);
                break;
            }
            it->second->findBoneRecursive( aName, returnBone );
        }
    }
}

//--------------------------------------------------------------------------------
void ofxFBXBone::pointTo( glm::vec3 aTarget ) {
    glm::vec3 axis( 1, 0, 0 );
    pointTo( aTarget, axis );
}

//--------------------------------------------------------------------------------
void ofxFBXBone::pointTo( glm::vec3 aTarget, glm::vec3 aAxis ) {
    auto relPosition = (getGlobalPosition() - aTarget);
    glm::quat tq = glm::rotation( aAxis, glm::normalize(relPosition));
    setGlobalOrientation(tq);
}

//--------------------------------------------------------------
string ofxFBXBone::getAsString( int aLevel ) {
    
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
    if( hasParentNode() ) {
        pname = " parent: " + getParentNode()->getName();
    }
    
    if( mSrcNode ) {
        oStr << mSrcNode->getTypeAsString() << ": " << getName() << pname << " fbx type: " << mSrcNode->getFbxTypeString() << " anim: " << isAnimationEnabled();
    } else {
        oStr << ": " << getName() << pname << " anim: " << isAnimationEnabled();
    }
    if( childBones.size() > 0 ) {
        oStr << " child bones: " << childBones.size();
    }
//    if(usingKeyFrames()) {
//        oStr << " num keys: " << mKeyCollections.size();
//    }
    oStr << endl;
    
    //    oStr << getTypeAsString()+": " + getName() + " kids: " +ofToString( childBones.size(), 0) + pname + " anim: " + ofToString( isAnimationEnabled(), 0) + " num keys: " +ofToString(mKeyCollections.size(),0) + "\n";
    
    map<string, shared_ptr<ofxFBXBone> >::iterator it;
    for(it = childBones.begin(); it != childBones.end(); ++it ) {
        oStr << it->second->getAsString( aLevel + 1);
    }
    return oStr.str();
}









