//
//  ofxFBXSrcNurbsCurve.h
//  ConnectionsWall-Nick
//
//  Created by Nick Hardeman on 7/11/19.
//

#pragma once

#define FBXSDK_NEW_API
#include "ofMain.h"
#include <fbxsdk.h>

#include "ofxFBXSrcNode.h"
//#include "ofxFBXMeshMaterial.h"

namespace ofxFBXSource {

class NurbsCurve : public ofxFBXSource::Node {
public:
    NurbsCurve();
    virtual ~NurbsCurve();
    
    ofxFBXSource::Node::NodeType getType() override { return OFX_FBX_NURBS_CURVE; }
    
    void setup( FbxNode* aNode ) override;
    void setFBXCurve( FbxNurbsCurve* aNurbsCurve );
//    void draw();
    
    void configurePolyline( ofPolyline& apoly );
    
    ofPolyline& getPolyline() { return mPolyline; }
//    ofPolyline getGlobalPolyline();
//    ofPolyline getGlobalPolylineAroundPosition();
    
protected:
    ofPolyline mPolyline;
};
    
}
