#ifndef TOUCHINPUTADAPTER_H_
#define TOUCHINPUTADAPTER_H_

#include <input/touch/touchtypes.h>
#include <input/controller.h>
//=============================================================================
// TouchInputAdapter
//
// Converts semantic touch actions into values written into UserController.
//
// Important:
// - Does NOT call CharacterController directly.
// - Does NOT call VehicleController directly.
// - Does NOT know about gameplay.
// - It only writes into UserController virtual input.
//=============================================================================

class UserController;

class TouchInputAdapter
{
public:
    static TouchInputAdapter& GetInstance();

    void Reset();

    void SetEnabled( bool enabled );
    bool IsEnabled() const;

    void SetTargetControllerIndex( unsigned int controllerIndex );
    unsigned int GetTargetControllerIndex() const;

    bool ApplyAction( TouchActionId actionId, float value, bool forceChange = false );
    bool ApplyInputManagerButton( int inputManagerButtonId, float value, bool forceChange = false );

    bool QueueAction( TouchActionId actionId, float value );
    bool QueueInputManagerButton( int inputManagerButtonId, float value );
    bool QueueInputManagerAxis( int inputManagerAxisId, float value );

    void FlushQueuedInputs();
    void ClearQueuedInputs();
    void ClearActiveInputs();

private:
    TouchInputAdapter();
    ~TouchInputAdapter();

    bool mQueuedInputActive[ Input::MaxPhysicalButtons ];
    float mQueuedInputValue[ Input::MaxPhysicalButtons ];
    
    bool mQueuedInputDirty;

    TouchInputAdapter( const TouchInputAdapter& );
    TouchInputAdapter& operator=( const TouchInputAdapter& );

    UserController* GetTargetController() const;
    float ClampInputValue( float value ) const;
    int ResolveInputManagerButtonToControllerIndex( int inputManagerButtonId ) const;

private:
    bool mEnabled;
    unsigned int mTargetControllerIndex;
};

inline bool TouchInputAdapter::IsEnabled() const
{
    return mEnabled;
}

inline unsigned int TouchInputAdapter::GetTargetControllerIndex() const
{
    return mTargetControllerIndex;
}

#endif // TOUCHINPUTADAPTER_H_