//========= Copyright Valve Corporation ============//

// Apple's version of glut.h #undef's APIENTRY, redefine it
#include "..\lib\openvr\headers\openvr.h"

#include <stdio.h>
#include <string>
#include <cstdlib>

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


private: 

	vr::EVRInitError eError = vr::VRInitError_None;
	vr::IVRSystem *m_pHMD;
	vr::IVRRenderModels *m_pRenderModels;
	std::string m_strDriver;
	std::string m_strDisplay;
	vr::TrackedDevicePose_t m_rTrackedDevicePose[ vr::k_unMaxTrackedDeviceCount ];
	Matrix4 m_rmat4DevicePose[ vr::k_unMaxTrackedDeviceCount ];
	bool m_rbShowTrackedDevice[ vr::k_unMaxTrackedDeviceCount ];

};