#ifndef TOUCHACTIONREGISTRY_H_
#define TOUCHACTIONREGISTRY_H_

#include <input/touch/touchtypes.h>

//=============================================================================
// TouchActionDefinition
//
// inputManagerButtonId is the value from InputManager::eButtonMap.
// It is stored as int so this registry remains simple and does not expose
// InputManager enum details to every caller.
//=============================================================================

struct TouchActionDefinition
{
    TouchActionId id;
    const char* name;

    int inputManagerButtonId;

    TouchActionCategory category;
    TouchProfile defaultProfile;
    TouchControlKind defaultControl;

    bool visibleByDefault;
};

class TouchActionRegistry
{
public:
    static unsigned int GetActionCount();

    static const TouchActionDefinition* GetActionByIndex( unsigned int index );
    static const TouchActionDefinition* GetActionById( TouchActionId id );
    static const TouchActionDefinition* GetActionByName( const char* name );
    static const TouchActionDefinition* GetActionByInputManagerButtonId( int inputManagerButtonId );

    static bool IsValidAction( TouchActionId id );

    static const char* GetActionName( TouchActionId id );
    static int GetInputManagerButtonId( TouchActionId id );

    static TouchActionCategory GetActionCategory( TouchActionId id );
    static TouchProfile GetDefaultProfile( TouchActionId id );
    static TouchControlKind GetDefaultControlKind( TouchActionId id );
    static bool IsVisibleByDefault( TouchActionId id );

private:
    TouchActionRegistry();
    ~TouchActionRegistry();
};

#endif // TOUCHACTIONREGISTRY_H_