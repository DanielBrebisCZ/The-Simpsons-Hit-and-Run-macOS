#include <input/touch/touchcameracontroller.h>
#include <input/touch/touchhudsystem.h>
#include <input/touch/touchinputmodemanager.h>



//=============================================================================
// Singleton
//=============================================================================

TouchCameraController& TouchCameraController::GetInstance()
{
    static TouchCameraController sInstance;
    return sInstance;
}

//=============================================================================
// Construction
//=============================================================================

TouchCameraController::TouchCameraController()
:
mEnabled( true ),
mSensitivityX( 200.0f ),
mSensitivityY( 25.0f ),
mInvertY( true ),
mDeadZone( 0.001f ),
mLookX(0.0f),
mLookY(0.0f),
mCameraToggleRequested( false )
{
}

TouchCameraController::~TouchCameraController()
{
}

//=============================================================================
// Public API
//=============================================================================

void TouchCameraController::Reset()
{
    ClearLookInput();

    mEnabled = true;

    mSensitivityX = 8.0f;
    mSensitivityY = 8.0f;

    mInvertY = false;
    mDeadZone = 0.001f;
    mCameraToggleRequested = false;
}

void TouchCameraController::SetEnabled( bool enabled )
{
    if ( mEnabled == enabled )
    {
        return;
    }

    mEnabled = enabled;

    if ( !mEnabled )
    {
        ClearLookInput();
        mCameraToggleRequested = false;
        // Drop any pending drag so it cannot be applied when re-enabled.
        TouchHudSystem::GetInstance().ConsumeCameraDragDelta();
    }
}

void TouchCameraController::SetSensitivity( float sensitivityX, float sensitivityY )
{
    SetSensitivityX( sensitivityX );
    SetSensitivityY( sensitivityY );
}

void TouchCameraController::SetSensitivityX( float sensitivityX )
{
    if ( sensitivityX < 0.0f )
    {
        sensitivityX = 0.0f;
    }

    mSensitivityX = sensitivityX;
}

void TouchCameraController::SetSensitivityY( float sensitivityY )
{
    if ( sensitivityY < 0.0f )
    {
        sensitivityY = 0.0f;
    }

    mSensitivityY = sensitivityY;
}

void TouchCameraController::SetInvertY( bool invertY )
{
    mInvertY = invertY;
}

void TouchCameraController::SetDeadZone( float deadZone )
{
    if ( deadZone < 0.0f )
    {
        deadZone = 0.0f;
    }

    if ( deadZone > 1.0f )
    {
        deadZone = 1.0f;
    }

    mDeadZone = deadZone;
}

void TouchCameraController::Update( unsigned int elapsedMs )
{
    (void)elapsedMs;

#if defined(RAD_ANDROID)
    if ( !TouchInputModeManager::GetInstance().ShouldShowTouchHud() )
    {
        TouchHudSystem::GetInstance().ConsumeCameraDragDelta();
        ClearLookInput();
        return;
    }
#endif

    if ( !mEnabled )
    {
        TouchHudSystem::GetInstance().ConsumeCameraDragDelta();
        ClearLookInput();
        return;
    }

    TouchVector2 delta =
        TouchHudSystem::GetInstance().ConsumeCameraDragDelta();

    float lookX = delta.x * mSensitivityX;
    float lookY = delta.y * mSensitivityY;

    if ( mInvertY )
    {
        lookY = -lookY;
    }

    lookX = ClampInputValue( lookX );
    lookY = ClampInputValue( lookY );

    if ( IsNearlyZero( lookX ) )
    {
        lookX = 0.0f;
    }

    if ( IsNearlyZero( lookY ) )
    {
        lookY = 0.0f;
    }

    if ( lookX == 0.0f && lookY == 0.0f )
    {
        ClearLookInput();
        return;
    }

    SetLookInput( lookX, lookY );
}

//=============================================================================
// Private helpers
//=============================================================================

float TouchCameraController::ClampInputValue( float value ) const
{
    if ( value > 1.0f )
    {
        return 1.0f;
    }

    if ( value < -1.0f )
    {
        return -1.0f;
    }

    return value;
}

float TouchCameraController::Abs( float value ) const
{
    return value < 0.0f ? -value : value;
}

bool TouchCameraController::IsNearlyZero( float value ) const
{
    return Abs( value ) <= mDeadZone;
}

void TouchCameraController::SetLookInput( float lookX, float lookY )
{
    mLookX = lookX;
    mLookY = lookY;
}

void TouchCameraController::ClearLookInput()
{
    mLookX = 0.0f;
    mLookY = 0.0f;
}

void TouchCameraController::RequestCameraToggle()
{
    if ( !mEnabled )
    {
        return;
    }

    mCameraToggleRequested = true;
}

bool TouchCameraController::ConsumeCameraToggleRequest()
{
    if ( !mEnabled )
    {
        mCameraToggleRequested = false;
        return false;
    }

    bool requested = mCameraToggleRequested;
    mCameraToggleRequested = false;

    return requested;
}