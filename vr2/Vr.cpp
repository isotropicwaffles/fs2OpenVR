
#include "Vr.h"



//-----------------------------------------------------------------------------
// Purpose: Initializes openvr and the headset system. Returns true if successful. 
//          Returns false if it fails
//-----------------------------------------------------------------------------

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

	
	if ( !vr::VRCompositor() )
	{
		printf( "Compositor initialization failed." );
		return false;
	}
	
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