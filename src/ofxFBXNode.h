//
//  ofxFBXNode.h
//  ofxFBX-Example-Importer
//
//  Created by Nick Hardeman on 10/31/13.
//
//

#pragma once

//#if defined(TARGET_LINUX) && !defined(TARGET_OPENGLES)
//    #include <fbxsdk.h>
//#endif

#include "ofMain.h"
#include <fbxsdk.h>
//#if defined(TARGET_OSX)
//    #include "fbxsdk.h"
//#endif

#include "ofxFBXUtils.h"

// this class is basically ofNode, but opening up more of the vars so that we can
// set them directly so that it is much faster //

class ofxFBXNode : public ofNode {
public:
    
    ofxFBXNode();
    ~ofxFBXNode();
    
    void setup( FbxNode *pNode );
    string getName();
    FbxString getFbxName();
    void setName( FbxString aName );
    
//    // set parent to link nodes
//	// transformations are inherited from parent node
//	// set to NULL if not needed (default)
//	void setParent( ofxFBXNode* parent, bool bMaintainGlobalTransform = false);
//    
//	void clearParent(bool bMaintainGlobalTransform = false);
//	ofNode* getParent() const;
//    
//	ofVec3f& getPosition() const;
//	
//	ofVec3f getXAxis() const;
//	ofVec3f getYAxis() const;
//	ofVec3f getZAxis() const;
//	
//	ofQuaternion& getOrientationQuat() const;
//	ofVec3f getOrientationEuler() const;
//	ofVec3f& getScale() const;
//    
//	const ofMatrix4x4& getLocalTransformMatrix() const;
//	
//	// TODO: optimize and cache these
//	// (parent would need to know about its children so it can inform them
//	// to update their global matrices if it itself transforms)
//	ofMatrix4x4 getGlobalTransformMatrix() const;
//	ofVec3f getGlobalPosition() const;
//	ofQuaternion getGlobalOrientation() const;
//	ofVec3f getGlobalScale() const;
//    
//	// directly set transformation matrix
//	void setTransformMatrix(const ofMatrix4x4 &m44);
//	
//	// position
//	void setPosition(float px, float py, float pz);
//	void setPosition(const ofVec3f& p);
//	
//	void setGlobalPosition(float px, float py, float pz);
//	void setGlobalPosition(const ofVec3f& p);
//    
//    
//	// orientation
//	void setOrientation(const ofQuaternion& q);			// set as quaternion
//	void setOrientation(const ofVec3f& eulerAngles);	// or euler can be useful, but prepare for gimbal lock
//    //	void setOrientation(const ofMatrix3x3& orientation);// or set as m33 if you have transformation matrix
//	
//	void setGlobalOrientation(const ofQuaternion& q);
//    
//	
//	// scale set and get
//	void setScale(float s);
//	void setScale(float sx, float sy, float sz);
//	void setScale(const ofVec3f& s);
//	
//	
//	// helpful move methods
//	void move(float x, float y, float z);			// move by arbitrary amount
//	void move(const ofVec3f& offset);				// move by arbitrary amount
//	void truck(float amount);						// move sideways (in local x axis)
//	void boom(float amount);						// move up+down (in local y axis)
//	void dolly(float amount);						// move forward+backward (in local z axis)
//	
//	
//	// helpful rotation methods
//	void tilt(float degrees);						// tilt up+down (around local x axis)
//	void pan(float degrees);						// rotate left+right (around local y axis)
//	void roll(float degrees);						// roll left+right (around local z axis)
//	void rotate(const ofQuaternion& q);				// rotate by quaternion
//	void rotate(float degrees, const ofVec3f& v);	// rotate around arbitrary axis by angle
//	void rotate(float degrees, float vx, float vy, float vz);
//	
//	// orient node to look at position (-ve z axis pointing to node)
//	void lookAt(const ofVec3f& lookAtPosition, ofVec3f upVector = ofVec3f(0, 1, 0));
//	void lookAt(const ofNode& lookAtNode, const ofVec3f& upVector = ofVec3f(0, 1, 0));
//	
//	
//	// set opengl's modelview matrix to this nodes transform
//	// if you want to draw something at the position+orientation+scale of this node...
//	// ...call ofNode::transform(); write your draw code, and ofNode::restoreTransform();
//	// OR A simpler way is to extend ofNode and override ofNode::customDraw();
//	void transformGL() const;
//	void restoreTransformGL() const;
//    
//	// transforms the node to its position+orientation+scale
//	void draw();
    
protected:
    string name;
    
//    ofxFBXNode* parent;
//    ofxFBXNode* globalParent;
//    
//    ofVec3f position;
//	ofQuaternion orientation;
//	ofVec3f scale;
//	
//	ofVec3f axis[3];
//	
//	ofMatrix4x4 localTransformMatrix;
//    ofMatrix4x4 globalTransformMatrix;
    
};


















