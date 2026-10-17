#include <input/touch/touchinteractionresolver.h>
#include <input/touch/touchcontextresolver.h>

#include <worldsim/character/charactermanager.h>
#include <worldsim/character/character.h>

#include <ai/actionbuttonhandler.h>

#if defined(RAD_ANDROID)
#include <android/log.h>

#define TOUCH_INTERACTION_LOG_TAG "SimpsonsHitAndRun"
#define TOUCH_INTERACTION_LOGI(...) __android_log_print(ANDROID_LOG_INFO, TOUCH_INTERACTION_LOG_TAG, __VA_ARGS__)
#else
#define TOUCH_INTERACTION_LOGI(...)
#endif

//=============================================================================
// Singleton
//=============================================================================

TouchInteractionResolver& TouchInteractionResolver::GetInstance()
{
    static TouchInteractionResolver sInstance;
    return sInstance;
}

//=============================================================================
// Construction
//=============================================================================

TouchInteractionResolver::TouchInteractionResolver()
{
}

TouchInteractionResolver::~TouchInteractionResolver()
{
}

void TouchInteractionResolver::Reset()
{
}

//=============================================================================
// Public
//=============================================================================

TouchInteractionType TouchInteractionResolver::ResolveCharacterInteraction() const
{
    /*
     * Solo resolvemos acciones contextuales cuando el perfil activo es
     * personaje a pie.
     */
    if ( !TouchContextResolver::GetInstance().IsCharacter() )
    {
        return TOUCH_INTERACTION_NONE;
    }

    Character* character = GetCharacterManager()->GetCharacter( 0 );

    if ( character == 0 )
    {
        return TOUCH_INTERACTION_NONE;
    }

    /*
     * Evitamos mostrar botón contextual a pie durante estados de coche,
     * entrada o salida.
     */
    if ( character->IsInCarOrGettingInOut() )
    {
        return TOUCH_INTERACTION_NONE;
    }

    /*
     * Esta es la clave de la arquitectura:
     * el juego ya ha resuelto qué acción contextual está activa.
     */
    ActionButton::ButtonHandler* handler =
        character->GetActionButtonHandler();

    if ( handler == 0 )
    {
        return TOUCH_INTERACTION_NONE;
    }

    /*
     * Respetamos el estado interno del handler. El HUD original del juego
     * también comprueba si la instancia está habilitada antes de mostrar
     * el overlay del botón de acción.
     */
    if ( !handler->IsInstanceEnabled() )
    {
        return TOUCH_INTERACTION_NONE;
    }

    return ConvertHandlerToInteraction( handler );
}

TouchInteractionIcon TouchInteractionResolver::ResolveCharacterInteractionIcon() const
{
    return TouchInteractionTypeToIcon(
        ResolveCharacterInteraction()
    );
}

//=============================================================================
// Private
//=============================================================================

TouchInteractionType TouchInteractionResolver::ConvertHandlerToInteraction
(
    ActionButton::ButtonHandler* handler
) const
{
    if ( handler == 0 )
    {
        return TOUCH_INTERACTION_NONE;
    }

    const ActionButton::ButtonHandler::Type type = handler->GetType();

/*
#if defined(RAD_ANDROID)
    TOUCH_INTERACTION_LOGI(
        "[TouchInteraction] handler=%p type=%d",
        handler,
        static_cast<int>( type )
    );
#endif
*/

    switch ( type )
    {
        case ActionButton::ButtonHandler::GET_IN_USER_CAR:
        case ActionButton::ButtonHandler::GET_IN_CAR:
        {
            return TOUCH_INTERACTION_ENTER_CAR;
        }

        case ActionButton::ButtonHandler::SUMMON_PHONE:
        {
            return TOUCH_INTERACTION_PHONE_BOOTH;
        }

        case ActionButton::ButtonHandler::INTERIOR:
        case ActionButton::ButtonHandler::TELEPORT:
        {
            return TOUCH_INTERACTION_INTERIOR_DOOR;
        }

        case ActionButton::ButtonHandler::PURCHASE_CAR:
        {
            return TOUCH_INTERACTION_PURCHASE;
        }

        case ActionButton::ButtonHandler::PURCHASE_SKIN:
        {
            return TOUCH_INTERACTION_CLOTHING;
        }

        case ActionButton::ButtonHandler::MISSION_OBJECTIVE:
        {
            return TOUCH_INTERACTION_MISSION_MAIN;
        }

        case ActionButton::ButtonHandler::GAG:
        {
            /*
             * Decisión de arquitectura:
             * cualquier gag usa exclamación.
             */
            return TOUCH_INTERACTION_MISSION_MAIN;
        }

        case ActionButton::ButtonHandler::COLLECTOR_CARD:
        case ActionButton::ButtonHandler::ALIEN_CAMERA:
        case ActionButton::ButtonHandler::WRENCH_ICON:
        case ActionButton::ButtonHandler::NITRO_ICON:
        case ActionButton::ButtonHandler::OTHER:
        default:
        {
            /*
             * Fallback robusto:
             * Si el juego dice que hay una acción contextual activa pero
             * no entra en una categoría visual específica,  no mostramos NADA 
             * 
             */
            return TOUCH_INTERACTION_MISSION_MAIN;
        }
    }
}