//
//  ofxFBXNode.cpp
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 10/31/13.
//
//

#include "ofxFBXNode.h"

void ofxFBXNode::setup( FbxNode *pNode ) {
    setName( pNode->GetNameOnly() );
    
//    if( pNode->GetRotationActive() ) {
//        if(pNode)
//    }
}

string ofxFBXNode::getName() {
    return name;
}

FbxString ofxFBXNode::getFbxName() {
    return FbxString( name.c_str() );
}

void ofxFBXNode::setName( FbxString aName ) {
    name = aName;
}



