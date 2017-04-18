//========= Copyright Valve Corporation ============//

// Apple's version of glut.h #undef's APIENTRY, redefine it
#include "..\lib\openvr\headers\openvr.h"

#include <stdio.h>
#include <string>
#include <cstdlib>
#include "..\code\globalincs\pstypes.h"

#ifndef __IVR_H__
#define __IVR_H__
#endif

// hand == 0 == right, 1 == left 
#define HAND_RIGHT 0
#define HAND_LEFT 1


#include "shared/Matrices.h"


#ifndef _countof
#define _countof(x) (sizeof(x)/sizeof((x)[0]))
#endif




class iVr
{
public:
	
	//Variables 
	Matrix4 m_mat4HMDPose;
	Matrix4 m_mat4eyePosLeft;
	Matrix4 m_mat4eyePosRight;

	Matrix4 m_mat4ProjectionCenter;
	Matrix4 m_mat4ProjectionLeft;
	Matrix4 m_mat4ProjectionRight;
	Matrix4 m_mat4ProjectionCurrent;
	matrix orientation;
	uint32_t m_nRenderWidth;
	uint32_t m_nRenderHeight;
	vr::Hmd_Eye current_eye;

	float m_fNearClip;
	float m_fFarClip;
	//Class Constructor

	iVr(){};

	

	//virtual ~iVr();

	bool VR_Init();

	//void SetupRenderModels();

	void VR_Shutdown();

	//-----------------------------------------------------------------------------
	// Purpose: Helper to get a string from a tracked device property and turn it
	//			into a std::string
	//-----------------------------------------------------------------------------
	std::string GetTrackedDeviceString(vr::IVRSystem *pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError);

	void UpdateHMDMatrixPose();
	//-----------------------------------------------------------------------------
	// Purpose: Setups up camera transformation stuff
	//-----------------------------------------------------------------------------
	void SetupCameras();
	//-----------------------------------------------------------------------------
	// Purpose: This appears to set up frame buffers based on openVR suggested width and height
	//-----------------------------------------------------------------------------
	bool SetupStereoRenderTargets();

	Matrix4 ComposeProjection(float zNear, float zFar);

	//-----------------------------------------------------------------------------
	// Purpose: Gets a Matrix Projection Eye with respect to nEye.
	//-----------------------------------------------------------------------------
	Matrix4 GetHMDMatrixProjectionEye();

	Matrix4 GetEyeViewMatrix();

private: 

	vr::EVRInitError eError = vr::VRInitError_None;
	vr::IVRSystem *m_pHMD;
	vr::IVRRenderModels *m_pRenderModels;
	std::string m_strDriver;
	std::string m_strDisplay;
	vr::TrackedDevicePose_t m_rTrackedDevicePose[ vr::k_unMaxTrackedDeviceCount ];
	Matrix4 m_rmat4DevicePose[ vr::k_unMaxTrackedDeviceCount ];
	bool m_rbShowTrackedDevice[ vr::k_unMaxTrackedDeviceCount ];


	//This stuff will be used for the rendering

	std::string m_strPoseClasses;                            // what classes we saw poses for this frame
	char m_rDevClassChar[vr::k_unMaxTrackedDeviceCount];   // for each device, a character representing its class


	int m_iTrackedControllerCount;
	int m_iTrackedControllerCount_Last;
	int m_iValidPoseCount;
	int m_iValidPoseCount_Last;



	//Internal Functions

	Matrix4 ConvertSteamVRMatrixToMatrix4(const vr::HmdMatrix34_t &matPose);
	






	//-----------------------------------------------------------------------------
	// Purpose: Gets an HMDMatrixPoseEye with respect to nEye.
	//-----------------------------------------------------------------------------
	Matrix4 GetHMDMatrixPoseEye(vr::Hmd_Eye nEye);

	//-----------------------------------------------------------------------------
	// Purpose: Gets a Current View Projection Matrix with respect to nEye,
	//          which may be an Eye_Left or an Eye_Right.
	//-----------------------------------------------------------------------------
	Matrix4 GetCurrentViewProjectionMatrix(vr::Hmd_Eye nEye);


	//-----------------------------------------------------------------------------
	// Purpose: This should render the frame
	//-----------------------------------------------------------------------------
	void iVr::RenderFrame();

};

