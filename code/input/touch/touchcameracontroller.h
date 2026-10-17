#ifndef TOUCHCAMERACONTROLLER_H_
#define TOUCHCAMERACONTROLLER_H_

//=============================================================================
// TouchCameraController
//
// Converts accumulated touch camera drag into virtual right-stick input.
//
// It does not know about WalkerCam, FollowCam, SuperCam, or gameplay cameras.
// It only writes InputManager::RightStickX / RightStickY through
// TouchInputAdapter.
//=============================================================================

class TouchCameraController
{
public:
    static TouchCameraController& GetInstance();

    void Reset();

    void SetEnabled( bool enabled );
    bool IsEnabled() const;

    void SetSensitivity( float sensitivityX, float sensitivityY );
    void SetSensitivityX( float sensitivityX );
    void SetSensitivityY( float sensitivityY );

    float GetLookX() const;
    float GetLookY() const;

    float GetSensitivityX() const;
    float GetSensitivityY() const;

    void SetInvertY( bool invertY );
    bool GetInvertY() const;

    void SetDeadZone( float deadZone );
    float GetDeadZone() const;

    void Update( unsigned int elapsedMs );

    void RequestCameraToggle();
    bool ConsumeCameraToggleRequest();

private:
    TouchCameraController();
    ~TouchCameraController();

    // Intentionally not implemented.
    TouchCameraController( const TouchCameraController& );
    TouchCameraController& operator=( const TouchCameraController& );

    float ClampInputValue( float value ) const;
    float Abs( float value ) const;
    bool IsNearlyZero( float value ) const;

    void SetLookInput( float lookX, float lookY );
    void ClearLookInput();

    bool mCameraToggleRequested;
private:
    bool mEnabled;

    float mSensitivityX;
    float mSensitivityY;

    bool mInvertY;

    // Applied after sensitivity. Small value to prevent tiny noise.
    float mDeadZone;

    float mLookX;
    float mLookY;
};

inline bool TouchCameraController::IsEnabled() const
{
    return mEnabled;
}

inline float TouchCameraController::GetSensitivityX() const
{
    return mSensitivityX;
}

inline float TouchCameraController::GetSensitivityY() const
{
    return mSensitivityY;
}

inline bool TouchCameraController::GetInvertY() const
{
    return mInvertY;
}

inline float TouchCameraController::GetDeadZone() const
{
    return mDeadZone;
}

inline float TouchCameraController::GetLookX() const
{
    return mLookX;
}

inline float TouchCameraController::GetLookY() const
{
    return mLookY;
}


#endif // TOUCHCAMERACONTROLLER_H_