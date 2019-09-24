//
//  ofxFBXSrcNurbsCurve.cpp
//  ConnectionsWall-Nick
//
//  Created by Nick Hardeman on 7/11/19.
//

#include "ofxFBXSrcNurbsCurve.h"

using namespace ofxFBXSource;

//--------------------------------------------------------------
NurbsCurve::NurbsCurve() {
    
}

//--------------------------------------------------------------
NurbsCurve::~NurbsCurve() {
    
}

//--------------------------------------------------------------
void NurbsCurve::setup( FbxNode* aNode ) {
    ofxFBXSource::Node::setup( aNode );
    setFBXCurve( aNode->GetNurbsCurve() );
}

//--------------------------------------------------------------
void NurbsCurve::setFBXCurve( FbxNurbsCurve* aNurbsCurve ) {
    if(aNurbsCurve != NULL ) {
        FbxLine * line = aNurbsCurve->TessellateCurve();
        if( line ){
            mPolyline.clear();
            for(int k = 0; k < line->GetIndexArraySize(); k++){
                FbxVector4 pt = line->GetControlPointAt(line->GetPointIndexAt(k));
                mPolyline.addVertex(glm::vec3(pt.mData[0],pt.mData[1],pt.mData[2]));
            }
        }
    }
}

//--------------------------------------------------------------
//void NurbsCurve::draw() {
//    if(mPolyline.size()) {
//        transformGL(); {
//            mPolyline.draw();
//        } restoreTransformGL();
//    }
//}

//--------------------------------------------------------------
void NurbsCurve::configurePolyline( ofPolyline& apoly ) {
    apoly = mPolyline;
}

//--------------------------------------------------------------
//ofPolyline NurbsCurve::getGlobalPolyline() {
//    glm::mat4 gmat = getGlobalTransformMatrix();
//    // transform points into global space //
//    ofPolyline tpoly = getPolyline();
//    
//    for( int i = 0; i < tpoly.size(); i++ ) {
//        auto& mp = tpoly[i];
//        mp = gmat * glm::vec4(mp, 1.0);
//    }
//    return tpoly;
//}
//
////--------------------------------------------------------------
//ofPolyline NurbsCurve::getGlobalPolylineAroundPosition() {
//    glm::vec3 gpos = getGlobalPosition();
//    auto tpoly = getGlobalPolyline();
//    for( auto& tv : tpoly.getVertices() ) {
//        tv -= gpos;
//    }
//    return tpoly;
//}
