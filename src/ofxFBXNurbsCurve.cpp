//
//  ofxFBXNurbsCurve.cpp
//
//  Created by Nick Hardeman on 5/8/19.
//

#include "ofxFBXNurbsCurve.h"
#include "ofxFBXSrcNurbsCurve.h"

//----------------------------------------
ofxFBXSource::Node::NodeType ofxFBXNurbsCurve::getType() {
    return ofxFBXSource::Node::OFX_FBX_NURBS_CURVE;
}

//--------------------------------------------------------------
void ofxFBXNurbsCurve::setup( shared_ptr<ofxFBXSource::Node> anode ) {
    ofxFBXNode::setup( anode );
    if( anode && anode->getType() == ofxFBXSource::Node::OFX_FBX_NURBS_CURVE ) {
        auto tc = dynamic_pointer_cast<ofxFBXSource::NurbsCurve>( anode );
        tc->configurePolyline( mPolyline );
        setTransform( tc );
    }
}

//--------------------------------------------------------------
void ofxFBXNurbsCurve::draw() {
    if(mPolyline.size()) {
        transformGL(); {
            mPolyline.draw();
        } restoreTransformGL();
    }
}
//
//--------------------------------------------------------------
void ofxFBXNurbsCurve::setPolyline( ofPolyline& apoly ) {
    mPolyline = apoly;
}

//--------------------------------------------------------------
ofPolyline ofxFBXNurbsCurve::getGlobalPolyline() {
    glm::mat4 gmat = getGlobalTransformMatrix();
    // transform points into global space //
    ofPolyline tpoly = getPolyline();
    
    for( int i = 0; i < tpoly.size(); i++ ) {
        auto& mp = tpoly[i];
        mp = gmat * glm::vec4(mp, 1.0);
    }
    return tpoly;
}

//--------------------------------------------------------------
ofPolyline ofxFBXNurbsCurve::getGlobalPolylineAroundPosition() {
    glm::vec3 gpos = getGlobalPosition();
    auto tpoly = getGlobalPolyline();
    for( auto& tv : tpoly.getVertices() ) {
        tv -= gpos;
    }
    return tpoly;
}

