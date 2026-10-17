#include <input/touch/touchinputadapter.h>
#include <input/touch/touchactionregistry.h>
#include <input/touch/touchinputmodemanager.h>

#include <input/inputmanager.h>



TouchInputAdapter& TouchInputAdapter::GetInstance()
{
    static TouchInputAdapter sInstance;
    return sInstance;
}

TouchInputAdapter::TouchInputAdapter()
:
mEnabled( false ),
mTargetControllerIndex( 0 ),
mQueuedInputDirty(false)
{
    for ( unsigned int i = 0; i < Input::MaxPhysicalButtons; ++i )
    {
        mQueuedInputActive[ i ] = false;
        mQueuedInputValue[ i ] = 0.0f;
    }
}

TouchInputAdapter::~TouchInputAdapter()
{
}

void TouchInputAdapter::Reset()
{
    ClearActiveInputs();
      for ( unsigned int i = 0; i < Input::MaxPhysicalButtons; ++i )
    {
        mQueuedInputActive[ i ] = false;
        mQueuedInputValue[ i ] = 0.0f;
    }
    mEnabled = false;
    mTargetControllerIndex = 0;
    mQueuedInputDirty = false;
}

static bool IsTouchInputBlockedByConfirmedGamepad()
{
    return TouchInputModeManager::GetInstance().IsTouchInputBlockedByGamepad();
}

void TouchInputAdapter::SetEnabled( bool enabled )
{
    if ( mEnabled == enabled )
    {
        return;
    }

    mEnabled = enabled;

    UserController* controller = GetTargetController();
    if ( controller )
    {
        controller->SetVirtualInputAvailable( mEnabled );

        if ( !mEnabled )
        {
            ClearQueuedInputs();
            controller->ClearVirtualInputs();
        }
    }
}

void TouchInputAdapter::SetTargetControllerIndex( unsigned int controllerIndex )
{
    if ( mTargetControllerIndex == controllerIndex )
    {
        return;
    }

    ClearActiveInputs();
    ClearQueuedInputs();

    UserController* oldController = GetTargetController();
    if ( oldController )
    {
        oldController->SetVirtualInputAvailable( false );
    }

    mTargetControllerIndex = controllerIndex;

    UserController* newController = GetTargetController();
    if ( newController )
    {
        newController->SetVirtualInputAvailable( mEnabled );
    }
}

bool TouchInputAdapter::ApplyAction( TouchActionId actionId, float value, bool forceChange )
{
    if ( !mEnabled )
    {
        return false;
    }

    const TouchActionDefinition* action =
        TouchActionRegistry::GetActionById( actionId );

    if ( action == 0 )
    {
        return false;
    }

    return ApplyInputManagerButton(
        action->inputManagerButtonId,
        value,
        forceChange
    );
}

bool TouchInputAdapter::ApplyInputManagerButton( int inputManagerButtonId, float value, bool forceChange )
{
    if ( IsTouchInputBlockedByConfirmedGamepad() )
    {
        return false;
    }

    if ( !mEnabled )
    {
        return false;
    }

    if ( inputManagerButtonId < 0 )
    {
        return false;
    }

    UserController* controller = GetTargetController();
    if ( controller == 0 )
    {
        return false;
    }

    /*
     * inputManagerButtonId no siempre coincide con el índice real del
     * input point dentro del controller.
     *
     * Ejemplo:
     *   InputManager::LeftStickX puede ser 22,
     *   pero el input point "LeftStickX" puede estar en el índice 16.
     *
     * Por eso resolvemos el índice real antes de escribir en mButtonArray.
     */
    int resolvedIndex = ResolveInputManagerButtonToControllerIndex( inputManagerButtonId );

    if ( resolvedIndex < 0 )
    {
        return false;
    }

    if ( static_cast<unsigned int>( resolvedIndex ) >= Input::MaxPhysicalButtons )
    {
        return false;
    }

    controller->SetVirtualInputAvailable( true );

    controller->SetVirtualInputValue(
        static_cast<unsigned int>( resolvedIndex ),
        ClampInputValue( value ),
        forceChange
    );

    return true;
}

void TouchInputAdapter::ClearActiveInputs()
{
    UserController* controller = GetTargetController();
    if ( controller )
    {
        controller->ClearVirtualInputs();
    }
}

UserController* TouchInputAdapter::GetTargetController() const
{
    if ( mTargetControllerIndex >= Input::MaxControllers )
    {
        return 0;
    }

    return InputManager::GetInstance()->GetController( mTargetControllerIndex );
}

float TouchInputAdapter::ClampInputValue( float value ) const
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
bool TouchInputAdapter::QueueAction( TouchActionId actionId, float value )
{
    if ( !mEnabled )
    {
        return false;
    }

    const TouchActionDefinition* action =
        TouchActionRegistry::GetActionById( actionId );

    if ( action == 0 )
    {
        return false;
    }

    return QueueInputManagerButton( action->inputManagerButtonId, value );
}


bool TouchInputAdapter::QueueInputManagerButton( int inputManagerButtonId, float value )
{
    if ( IsTouchInputBlockedByConfirmedGamepad() )
    {
        return false;
    }
    if ( !mEnabled )
    {
        return false;
    }

    if ( inputManagerButtonId < 0 )
    {
        return false;
    }

    int resolvedIndex = ResolveInputManagerButtonToControllerIndex( inputManagerButtonId );

    if ( resolvedIndex < 0 )
    {
        return false;
    }

    if ( static_cast<unsigned int>( resolvedIndex ) >= Input::MaxPhysicalButtons )
    {
        return false;
    }

    unsigned int index = static_cast<unsigned int>( resolvedIndex );

    mQueuedInputValue[ index ] = ClampInputValue( value );
    mQueuedInputActive[ index ] = true;

    mQueuedInputDirty = true;

    return true;
}

bool TouchInputAdapter::QueueInputManagerAxis( int inputManagerAxisId, float value )
{
    return QueueInputManagerButton( inputManagerAxisId, value );
}

void TouchInputAdapter::FlushQueuedInputs()
{
     if ( IsTouchInputBlockedByConfirmedGamepad() )
    {
        ClearQueuedInputs();
        return;
    }

    if ( !mEnabled )
    {
        ClearQueuedInputs();
        return;
    }

    if(!mQueuedInputDirty)
    {
        return;
    }

    UserController* controller = GetTargetController();
    if ( controller == 0 )
    {
        ClearQueuedInputs();
        return;
    }

    controller->SetVirtualInputAvailable( true );

    for ( unsigned int i = 0; i < Input::MaxPhysicalButtons; ++i )
    {
        if ( mQueuedInputActive[ i ] )
        {
            controller->SetVirtualInputValue(
                i,
                mQueuedInputValue[ i ],
                true
            );

            mQueuedInputActive[ i ] = false;
        }
    }
    mQueuedInputDirty=false;
}

int TouchInputAdapter::ResolveInputManagerButtonToControllerIndex( int inputManagerButtonId ) const
{
    UserController* controller = GetTargetController();

    if ( controller == 0 )
    {
        return -1;
    }

    const char* inputName = 0;

    switch ( inputManagerButtonId )
    {
        case InputManager::DPadUp:
            inputName = "DPadUp";
            break;

        case InputManager::DPadDown:
            inputName = "DPadDown";
            break;

        case InputManager::DPadLeft:
            inputName = "DPadLeft";
            break;

        case InputManager::DPadRight:
            inputName = "DPadRight";
            break;

        case InputManager::Start:
            inputName = "Start";
            break;

        case InputManager::Back:
            inputName = "Back";
            break;

        case InputManager::LeftThumb:
            inputName = "LeftThumb";
            break;

        case InputManager::RightThumb:
            inputName = "RightThumb";
            break;

        case InputManager::A:
            inputName = "A";
            break;

        case InputManager::B:
            inputName = "B";
            break;

        case InputManager::Square:
            inputName = "X";
            break;

        case InputManager::Y:
            inputName = "Y";
            break;

        case InputManager::Black:
            inputName = "Black";
            break;

        case InputManager::White:
            inputName = "White";
            break;

        case InputManager::LeftTrigger:
            inputName = "LeftTrigger";
            break;

        case InputManager::RightTrigger:
            inputName = "RightTrigger";
            break;

        case InputManager::LeftStickX:
            inputName = "LeftStickX";
            break;

        case InputManager::LeftStickY:
            inputName = "LeftStickY";
            break;

        case InputManager::RightStickX:
            inputName = "RightStickX";
            break;

        case InputManager::RightStickY:
            inputName = "RightStickY";
            break;

        default:
            break;
    }

    if ( inputName == 0 )
    {
        return inputManagerButtonId;
    }

    return controller->GetIdByName( inputName );
}

void TouchInputAdapter::ClearQueuedInputs()
{
    if ( !mQueuedInputDirty )
    {
        return;
    }

    for ( unsigned int i = 0; i < Input::MaxPhysicalButtons; ++i )
    {
        mQueuedInputActive[ i ] = false;
        mQueuedInputValue[ i ] = 0.0f;
    }
     mQueuedInputDirty = false;
}