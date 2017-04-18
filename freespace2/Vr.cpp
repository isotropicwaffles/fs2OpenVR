
#include "Vr.h"


//-----------------------------------------------------------------------------
// Purpose: Initializes openvr and the headset system. Returns true if successful. 
//          Returns false if it fails
//-----------------------------------------------------------------------------
iVr* VROBJ = new iVr();

bool iVr::VR_Init()
{	

	// Loading the SteamVR Runtime
	m_pHMD = vr::VR_Init( &eError, vr::VRApplication_Scene );

	if ( eError != vr::VRInitError_None )
	{
		m_pHMD = NULL;
		char buf[1024];
		sprintf_s( buf, sizeof( buf ), "Unable to init VR runtime: %s", vr::VR_GetVRInitErrorAsEnglishDescription( eError ) );
		//SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "VR_Init Failed", buf, NULL );
		return false;
	}
	
	//I believe this should get the models to render for the headset/controllers
	m_pRenderModels = (vr::IVRRenderModels *)vr::VR_GetGenericInterface( vr::IVRRenderModels_Version, &eError );
	
	if( !m_pRenderModels )
	{
		m_pHMD = NULL;
		vr::VR_Shutdown();

		char buf[1024];
		sprintf_s( buf, sizeof( buf ), "Unable to get render model interface: %s", vr::VR_GetVRInitErrorAsEnglishDescription( eError ) );
		//SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "VR_Init Failed", buf, NULL );
		return false;
	}


	m_strDriver = "No Driver";
	m_strDisplay = "No Display";

	m_strDriver = GetTrackedDeviceString(m_pHMD, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String, NULL);
	m_strDisplay = GetTrackedDeviceString(m_pHMD, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SerialNumber_String, NULL);
	current_eye = vr::Eye_Left;

	
	if ( !vr::VRCompositor() )
	{
		printf( "Compositor initialization failed." );
		return false;
	}
	

	SetupStereoRenderTargets();


	return true;
		
}
	
//-----------------------------------------------------------------------------
// Purpose: Shuts down openVR if it's started
//-----------------------------------------------------------------------------	
void iVr::VR_Shutdown()
{

	if( m_pHMD )
	{
		vr::VR_Shutdown();
		m_pHMD = NULL;
	}
}


//-----------------------------------------------------------------------------
// Purpose: Setups up camera transformation stuff
//-----------------------------------------------------------------------------
void iVr::SetupCameras()
{
	//m_mat4ProjectionLeft = GetHMDMatrixProjectionEye(vr::Eye_Left);
	//m_mat4ProjectionRight = GetHMDMatrixProjectionEye(vr::Eye_Right);
	m_mat4eyePosLeft = GetHMDMatrixPoseEye(vr::Eye_Left);
	m_mat4eyePosRight = GetHMDMatrixPoseEye(vr::Eye_Right);
}



//-----------------------------------------------------------------------------
// Purpose: Converts a SteamVR matrix to our local matrix class
//-----------------------------------------------------------------------------
Matrix4 iVr::ConvertSteamVRMatrixToMatrix4(const vr::HmdMatrix34_t &matPose)
{
	Matrix4 matrixObj(
		matPose.m[0][0], matPose.m[1][0], matPose.m[2][0], 0.0,
		matPose.m[0][1], matPose.m[1][1], matPose.m[2][1], 0.0,
		matPose.m[0][2], matPose.m[1][2], matPose.m[2][2], 0.0,
		matPose.m[0][3], matPose.m[1][3], matPose.m[2][3], 1.0f
	);
	return matrixObj;
}






//-----------------------------------------------------------------------------
// Purpose: Gets a Matrix Projection Eye with respect to nEye.
//-----------------------------------------------------------------------------
Matrix4 iVr::GetEyeViewMatrix()
{
	if (!m_pHMD)
		return Matrix4();


	if (current_eye == vr::Eye_Left)
	{
		return m_mat4eyePosLeft;// *m_mat4HMDPose;
	}
	else if (current_eye == vr::Eye_Right)
	{
		return  m_mat4eyePosRight;// *  m_mat4HMDPose;
	}


}


Matrix4 iVr::GetHMDMatrixProjectionEye()
{
	if (!m_pHMD)
		return Matrix4();

	vr::HmdMatrix44_t mat1 = m_pHMD->GetProjectionMatrix(current_eye, m_fNearClip, m_fFarClip);
	return Matrix4(
		mat1.m[0][0], mat1.m[1][0], mat1.m[2][0], mat1.m[3][0],
		mat1.m[0][1], mat1.m[1][1], mat1.m[2][1], mat1.m[3][1],
		mat1.m[0][2], mat1.m[1][2], mat1.m[2][2], mat1.m[3][2],
		mat1.m[0][3], mat1.m[1][3], mat1.m[2][3], mat1.m[3][3]	);
}


//-----------------------------------------------------------------------------
// Purpose: Gets an HMDMatrixPoseEye with respect to nEye.
//-----------------------------------------------------------------------------
Matrix4 iVr::GetHMDMatrixPoseEye(vr::Hmd_Eye nEye)
{
	if (!m_pHMD)
		return Matrix4();

	vr::HmdMatrix34_t matEyeRight = m_pHMD->GetEyeToHeadTransform(nEye);
	Matrix4 matrixObj(
		matEyeRight.m[0][0], matEyeRight.m[1][0], matEyeRight.m[2][0], 0.0,
		matEyeRight.m[0][1], matEyeRight.m[1][1], matEyeRight.m[2][1], 0.0,
		matEyeRight.m[0][2], matEyeRight.m[1][2], matEyeRight.m[2][2], 0.0,
		matEyeRight.m[0][3], matEyeRight.m[1][3], matEyeRight.m[2][3], 1.0f
	);

	return matrixObj;
}


//-----------------------------------------------------------------------------
// Purpose: Gets a Current View Projection Matrix with respect to nEye,
//          which may be an Eye_Left or an Eye_Right.
//-----------------------------------------------------------------------------
Matrix4 iVr::GetCurrentViewProjectionMatrix(vr::Hmd_Eye nEye)
{
	Matrix4 matMVP;
	if (nEye == vr::Eye_Left)
	{
		matMVP = m_mat4ProjectionLeft * m_mat4eyePosLeft * m_mat4HMDPose;
	}
	else if (nEye == vr::Eye_Right)
	{
		matMVP = m_mat4ProjectionRight * m_mat4eyePosRight *  m_mat4HMDPose;
	}

	return matMVP;
}



//-----------------------------------------------------------------------------
// Purpose: Updates the Pose of the HMD
//-----------------------------------------------------------------------------
void iVr::UpdateHMDMatrixPose()
{
	if (!m_pHMD)
		return;

	vr::VRCompositor()->WaitGetPoses(m_rTrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);

	m_iValidPoseCount = 0;
	m_strPoseClasses = "";
	for (int nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; ++nDevice)
	{
		if (m_rTrackedDevicePose[nDevice].bPoseIsValid)
		{
			m_iValidPoseCount++;
			m_rmat4DevicePose[nDevice] = ConvertSteamVRMatrixToMatrix4(m_rTrackedDevicePose[nDevice].mDeviceToAbsoluteTracking);
			if (m_rDevClassChar[nDevice] == 0)
			{
				switch (m_pHMD->GetTrackedDeviceClass(nDevice))
				{
				case vr::TrackedDeviceClass_Controller:        m_rDevClassChar[nDevice] = 'C'; break;
				case vr::TrackedDeviceClass_HMD:               m_rDevClassChar[nDevice] = 'H'; break;
				case vr::TrackedDeviceClass_Invalid:           m_rDevClassChar[nDevice] = 'I'; break;
				case vr::TrackedDeviceClass_GenericTracker:    m_rDevClassChar[nDevice] = 'G'; break;
				case vr::TrackedDeviceClass_TrackingReference: m_rDevClassChar[nDevice] = 'T'; break;
				default:                                       m_rDevClassChar[nDevice] = '?'; break;
				}
			}
			m_strPoseClasses += m_rDevClassChar[nDevice];
		}
	}

	if (m_rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
	{
		m_mat4HMDPose = m_rmat4DevicePose[vr::k_unTrackedDeviceIndex_Hmd];
		m_mat4HMDPose[12] = 0;
		m_mat4HMDPose[13] = 0;
		m_mat4HMDPose[14] = 0;

		//m_mat4HMDPose.invert();
	}
}


//-----------------------------------------------------------------------------
// Purpose: This appears to set up frame buffers based on openVR suggested width and height
//-----------------------------------------------------------------------------
bool iVr::SetupStereoRenderTargets()
{
	if (!m_pHMD)
		return false;

	m_pHMD->GetRecommendedRenderTargetSize(&m_nRenderWidth, &m_nRenderHeight);

	//CreateFrameBuffer(m_nRenderWidth, m_nRenderHeight, leftEyeDesc);
	//CreateFrameBuffer(m_nRenderWidth, m_nRenderHeight, rightEyeDesc);

	return true;
}


//-----------------------------------------------------------------------------
// Purpose: Helper to get a string from a tracked device property and turn it
//			into a std::string
//-----------------------------------------------------------------------------
std::string iVr::GetTrackedDeviceString( vr::IVRSystem *pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError = NULL )
{
	uint32_t unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty( unDevice, prop, NULL, 0, peError );
	if( unRequiredBufferLen == 0 )
		return "";

	char *pchBuffer = new char[ unRequiredBufferLen ];
	unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty( unDevice, prop, pchBuffer, unRequiredBufferLen, peError );
	std::string sResult = pchBuffer;
	delete [] pchBuffer;
	return sResult;
}



//THis functions gets the projection matrix
Matrix4 iVr::ComposeProjection(float zNear, float zFar)
{


	float fLeft, fRight, fTop, fBottom;




	m_pHMD->GetProjectionRaw(current_eye, &fLeft, &fRight, &fTop, &fBottom);


	float idx = 1.0f / (fRight - fLeft);
	float idy = 1.0f / (fBottom - fTop);
	float idz = 1.0f / (zFar - zNear);
	float sx = fRight + fLeft;
	float sy = fBottom + fTop;
	Matrix4 p;

	p[0] = 2 * idx; p[1] = 0;     p[2] = sx*idx;    p[3] = 0;
	p[4] = 0;     p[5] = 2 * idy; p[6] = sy*idy;    p[7] = 0;
	p[8] = 0;     p[9] = 0;     p[10] = -zFar*idz; p[11] = -zFar*zNear*idz;
	p[12] = 0;     p[13] = 0;     p[14] = -1.0f;     p[15] = 0;
	
	return p;
}

//-----------------------------------------------------------------------------
// Purpose: This should render the frame
//-----------------------------------------------------------------------------
void iVr::RenderFrame()
{
	// for now as fast as possible
	if (m_pHMD)
	{
		//RenderControllerAxes();
		//RenderStereoTargets();
		//RenderCompanionWindow();

		//vr::Texture_t leftEyeTexture = { (void*)(uintptr_t)leftEyeDesc.m_nResolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		//vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
		//vr::Texture_t rightEyeTexture = { (void*)(uintptr_t)rightEyeDesc.m_nResolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		//vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);
	}

	

	this->UpdateHMDMatrixPose();
}
