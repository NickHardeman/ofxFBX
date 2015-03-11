//
//  ofxFBXNode.h
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 10/31/13.
//
//

#pragma once

#if defined(TARGET_LINUX) && !defined(TARGET_OPENGLES)
    #include <fbxsdk.h>
#endif

#include "ofMain.h"
#if defined(TARGET_OSX)
    #include "fbxsdk.h"
#endif

#include "ofxFBXUtils.h"

class ofxFBXNode : public ofNode {
public:
    
    void setup( FbxNode *pNode );
    string getName();
    FbxString getFbxName();
    void setName( FbxString aName );
    
    virtual void customDraw() {}
    
    virtual void onPositionChanged() {}
	virtual void onOrientationChanged() {}
	virtual void onScaleChanged() {}
    
protected:
    string name;
};


















