//
//  ofxFBXNurbsCurve.h
//
//  Created by Nick Hardeman on 5/8/19.
//

#pragma once
#define FBXSDK_NEW_API
#include "ofxFBXNode.h"

class ofxFBXNurbsCurve : public ofxFBXNode {
public:
    
    ofxFBXSource::Node::NodeType getType() override;
    
    void setup( shared_ptr<ofxFBXSource::Node> anode ) override;
    
    void draw();
    void setPolyline( ofPolyline& apoly );
    
    ofPolyline& getPolyline() { return mPolyline; }
    ofPolyline getGlobalPolyline();
    ofPolyline getGlobalPolylineAroundPosition();
 
protected:
    ofPolyline mPolyline;
};
