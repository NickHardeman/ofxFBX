//
//  ofxFBXSkeleton.cpp
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 11/5/13.
//
//

#include "ofxFBXSkeleton.h"
ofxFBXBone dummyBone;

//--------------------------------------------------------------
ofxFBXSkeleton::ofxFBXSkeleton() {
    numBones = 0;
}

//--------------------------------------------------------------
ofxFBXSkeleton::~ofxFBXSkeleton() {
    
}

//--------------------------------------------------------------
void ofxFBXSkeleton::setup( FbxNode *pNode ) {
    ofxFBXNode::setup( pNode );
}

//--------------------------------------------------------------
void ofxFBXSkeleton::reconstructNodeParenting() {
    
    map<string, ofxFBXBone >::iterator it;
    for(it = bones.begin(); it != bones.end(); ++it ) {
        ofxFBXBone* bone    = &it->second;
        if( bone->parentBoneName == "" ) continue;
//        ofxFBXBone* pbone   = getBone( bone->parentBoneName );
//        if( bone->parentBoneName == "Armature|FootIK.L" || bone->parentBoneName == "Armature|FootIK.R" ) {
//            cout << "removing parent with name " << bone->parentBoneName << endl;
//            bone->clearParent( false );
//        }
//
//        if( pbone == NULL ) continue;
//        
//        cout << "reconstructNodeParenting :: " << bone->getName() << " parent: " << bone->parentBoneName << endl;
//        
//        if( bone->getParent() != NULL ) {
//            bone->clearParent( false );
//        }
//        
//        bone->setParent( *pbone, false );
//        bone->setGlobalPosition( bone->origNode.getGlobalPosition() );
//        bone->setGlobalOrientation( bone->origNode.getGlobalOrientation() );
    }
}

//--------------------------------------------------------------
void ofxFBXSkeleton::update( FbxTime& pTime, FbxPose* pPose ) {
    
    
    updateBoneProperties();
//    map<string, ofxFBXBone >::iterator it;
//    for(it = bones.begin(); it != bones.end(); ++it ) {
//        ofxFBXBone* bone    = &it->second;
//        ofxFBXBone* sbone   = getSourceBone( it->first );
//        if( sbone != NULL ) {
//            if( bone->isExternalControlEnabled() ) {
//                sbone->enableExternalControl();
//            } else {
//                sbone->disableExternalControl();
//            }
//            
//            if( bone->isAnimationEnabled() ) {
//                sbone->enableAnimation();
//            } else {
//                sbone->disableAnimation();
//            }
//        }
//    }
    
    map<string, ofxFBXBone >::iterator it;
    for(it = bones.begin(); it != bones.end(); ++it ) {
        ofxFBXBone* bone    = &it->second;
        ofxFBXBone* sbone   = getSourceBone( it->first );
        if( sbone != NULL ) {
            sbone->update( pTime, pPose );
        }
    }
    for(it = bones.begin(); it != bones.end(); ++it ) {
        ofxFBXBone* bone    = &it->second;
        ofxFBXBone* sbone   = getSourceBone( it->first );
        if( sbone != NULL ) {
            bone->setTransformMatrix( sbone->getLocalTransformMatrix() );
        }
    }
}

//--------------------------------------------------------------
void ofxFBXSkeleton::lateUpdate() {
    map<string, ofxFBXBone >::iterator it;
    for(it = bones.begin(); it != bones.end(); ++it ) {
        ofxFBXBone* bone    = &it->second;
        ofxFBXBone* sbone   = getSourceBone( it->first );
        if( sbone != NULL ) {
            sbone->setGlobalOrientation( bone->getGlobalOrientation() );
            sbone->setGlobalPosition( bone->getGlobalPosition() );
        }
    }
    
    for(it = bones.begin(); it != bones.end(); ++it ) {
        ofxFBXBone* bone    = &it->second;
        ofxFBXBone* sbone   = getSourceBone( it->first );
        if( sbone != NULL ) {
            sbone->updateFbxTransform();
        }
    }
}

//--------------------------------------------------------------
void ofxFBXSkeleton::updateWithAnimation( FbxTime& pTime, FbxPose* pPose ) {
    
    updateBoneProperties();
//    map<string, ofxFBXBone >::iterator it;
//    for(it = bones.begin(); it != bones.end(); ++it ) {
//        ofxFBXBone* bone    = &it->second;
//        ofxFBXBone* sbone   = getSourceBone( it->first );
//        if( sbone != NULL ) {
//            if( bone->isExternalControlEnabled() ) {
//                sbone->enableExternalControl();
//            } else {
//                sbone->disableExternalControl();
//            }
//            
//            if( bone->isAnimationEnabled() ) {
//                sbone->enableAnimation();
//            } else {
//                sbone->disableAnimation();
//            }
//        }
//    }
    
    map<string, ofxFBXBone >::iterator it;
    for(it = bones.begin(); it != bones.end(); ++it ) {
        ofxFBXBone* bone    = &it->second;
        ofxFBXBone* sbone   = getSourceBone( it->first );
        if( sbone != NULL ) {
            sbone->update( pTime, pPose );
            bone->setTransformMatrix( sbone->getLocalTransformMatrix() );
        }
    }
}

//--------------------------------------------------------------
void ofxFBXSkeleton::lateUpdateWithAnimation() {
    map<string, ofxFBXBone >::iterator it;
    for(it = bones.begin(); it != bones.end(); ++it ) {
        ofxFBXBone* bone    = &it->second;
        ofxFBXBone* sbone   = getSourceBone( it->first );
        if( sbone != NULL ) {
//            sbone->setTransformMatrix( bone->getLocalTransformMatrix() );
            sbone->localTransformMatrix = bone->localTransformMatrix;
            sbone->setPosition( bone->getPosition() );
            sbone->setOrientation( bone->getOrientationQuat() );
            sbone->setScale( bone->getScale() );
        }
    }
    
    for(it = bones.begin(); it != bones.end(); ++it ) {
        ofxFBXBone* bone    = &it->second;
        ofxFBXBone* sbone   = getSourceBone( it->first );
        if( sbone != NULL ) {
            sbone->updateFbxTransform();
        }
    }
}

//--------------------------------------------------------------
void ofxFBXSkeleton::updateFromCachedSkeleton( shared_ptr< ofxFBXSkeleton> aSkeleton ) {
    
//    updateBoneProperties();
    map<string, ofxFBXBone >::iterator it;
    for(it = bones.begin(); it != bones.end(); ++it ) {
        ofxFBXBone* bone    = &it->second;
//        ofxFBXBone* sbone   = getSourceBone( it->first );
//        ofxFBXBone* sbone = aSkeleton->getSourceBone( it->first );
        ofxFBXBone* obone = aSkeleton->getBone( it->first );
        if( obone != NULL ) {
//            
//            localTransformMatrix = m44;
//
//            ofQuaternion so;
//            localTransformMatrix.decompose(position, orientation, scale, so);
//
//            onPositionChanged();
//            onOrientationChanged();
//            onScaleChanged();
            
//            sbone->update( pTime, NULL );
//            bone->setTransformMatrix( obone->getLocalTransformMatrix() );
            bone->localTransformMatrix = obone->localTransformMatrix;
            bone->setPosition( obone->getPosition() );
            bone->setOrientation( obone->getOrientationQuat() );
            bone->setScale( obone->getScale() );
            
        } else {
            ofLogWarning() << "updateFromCachedSkeleton: " << it->first << " is null !!" << endl;
        }
    }
}

//--------------------------------------------------------------
void ofxFBXSkeleton::updateBoneProperties() {
    map<string, ofxFBXBone >::iterator it;
    for(it = bones.begin(); it != bones.end(); ++it ) {
        ofxFBXBone* bone    = &it->second;
        ofxFBXBone* sbone   = getSourceBone( it->first );
        if( sbone != NULL ) {
            if( bone->isExternalControlEnabled() ) {
                sbone->enableExternalControl();
            } else {
                sbone->disableExternalControl();
            }
            
            if( bone->isAnimationEnabled() ) {
                sbone->enableAnimation();
            } else {
                sbone->disableAnimation();
            }
        }
    }
}

//--------------------------------------------------------------
void ofxFBXSkeleton::draw( float aLen ) {
    map<string, ofxFBXBone>::iterator it;
    for(it = bones.begin(); it != bones.end(); ++it ) {
        it->second.draw(aLen);
    }
}

//--------------------------------------------------------------
void ofxFBXSkeleton::addBone( string aName, ofxFBXBone* aBone ) {
    if(aName == "") {
        ofLogWarning("ofxFBXSkeleton :: addBone : name is empty, not adding bone");
        return;
    }
    if(bones.find(aName) != bones.end()) {
        // the bone has a duplicate name, try to make a unique key //
        aName += ofToString(numBones,0);
    }
    
    vector <string> strs;// = ofSplitString(aName, ":");
    
    
    ofxFBXBone tbone = *aBone;
    string newBoneName = aName;
    if(strs.size() > 1) {
        newBoneName = strs[1];
    }
    
//    cout << "adding a bone with name -" << newBoneName << "- parent name -" << getName() <<"-" << endl;
    
    sourceBones[ newBoneName ]    = aBone;
    bones[ newBoneName ]          = tbone;
    
//    cout << "ofxFBXSkeleton :: addBone : name: " << aName << " ptr parent = " << aBone->getParent() << " other: " << tbone.getParent() << endl;
    
    numBones++;
}

//--------------------------------------------------------------
ofxFBXBone* ofxFBXSkeleton::getBone( string aName ) {
    if(bones.find(aName) != bones.end()) {
        return &bones[ aName ];
    }
    cout << "did not find the bone with name: " << aName << " | " << ofGetElapsedTimef() << endl;
    return NULL;
}

//--------------------------------------------------------------
ofxFBXBone* ofxFBXSkeleton::getSourceBone( string aName ) {
    if(bones.find(aName) != bones.end()) {
        return sourceBones[ aName ];
    }
    return NULL;
}

//--------------------------------------------------------------
void ofxFBXSkeleton::enableExternalControl( string aName ) {
    // loop through hierarchy to make children also controlled externally
    ofxFBXBone* bone = getBone( aName );
    if(bone == NULL) {
        ofLogWarning("ofxFBXSkeleton :: enableExternalControl : can not find bone with name ") << aName;
        return;
    }
    bone->enableExternalControl();
    
    enableExternalControlRecursive( getSourceBone(aName) );
    
}

//--------------------------------------------------------------
void ofxFBXSkeleton::enableExternalControlRecursive( ofxFBXBone* aBone ) {
    if( aBone == NULL ) return;
    map<string, ofxFBXBone* >::iterator it;
    for(it = sourceBones.begin(); it != sourceBones.end(); ++it ) {
        if( it->second->fbxNode->GetParent() == aBone->fbxNode ) {
//            it->second->enableExternalControl();
            getBone( it->first )->enableExternalControl();
            enableExternalControlRecursive( it->second );
        }
    }
}

//--------------------------------------------------------------
void ofxFBXSkeleton::enableAnimation() {
    map<string, ofxFBXBone >::iterator it;
    for(it = bones.begin(); it != bones.end(); ++it ) {
        it->second.enableAnimation();
    }
}

//--------------------------------------------------------------
void ofxFBXSkeleton::disableAnimation() {
    map<string, ofxFBXBone >::iterator it;
    for(it = bones.begin(); it != bones.end(); ++it ) {
        it->second.disableAnimation();
    }
}

//--------------------------------------------------------------
int ofxFBXSkeleton::getNumBones() {
    return (int)bones.size();
}

//--------------------------------------------------------------
void ofxFBXSkeleton::reset() {
    map<string, ofxFBXBone >::iterator it;
    for(it = bones.begin(); it != bones.end(); ++it ) {
        it->second.reset();
    }
}

//--------------------------------------------------------------
string ofxFBXSkeleton::toString() {
    map<string, ofxFBXBone >::iterator it;
    map< string, bool > printMap;
    for(it = bones.begin(); it != bones.end(); ++it ) {
        printMap[ it->first ] = false;
    }
    
    stringstream ss;
    ss << "Skeleton: " << getName() << " - Number of bones: " << bones.size() << endl;
    
    if( bones.size() ) {
        printOutBoneRecursive(ss, sourceBones.begin()->first, printMap, "" );
    } else {
        ss << "NO BONES" << endl;
    }
    
    return ss.str().c_str();
}

//--------------------------------------------------------------
void ofxFBXSkeleton::printOutBoneRecursive( stringstream& aSS, string aBoneName, map< string, bool >& aPrintMap, string aSpacer ) {
    if( aPrintMap[ aBoneName ] ) return;
    
    aPrintMap[ aBoneName ] = true;
    ofxFBXBone* bone = getSourceBone( aBoneName );
    if( bone == NULL ) return;
    
    aSS << aSpacer << " bone: " << aBoneName << " parent: " << bone->parentBoneName << endl;
    aSpacer += "---";
    
    map<string, ofxFBXBone*>::iterator it;
    for(it = sourceBones.begin(); it != sourceBones.end(); ++it ) {
        if( it->second->fbxNode->GetParent() == bone->fbxNode ) {
            printOutBoneRecursive( aSS, it->first, aPrintMap, aSpacer );
        }
    }
}









