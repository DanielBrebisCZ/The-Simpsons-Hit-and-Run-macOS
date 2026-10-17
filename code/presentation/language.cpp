//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        language.cpp
//
// Description: contains functions for dealing with language
//
// History:     21/1/2002 + Created -- Ian Gipson
//
//=============================================================================

//========================================
// System Includes
//========================================
#ifdef RAD_PS2
    #include <libscf.h>
#endif

#ifdef RAD_GAMECUBE
    #include <dolphin/os.h>
#endif

#ifdef RAD_XBOX
    #include <xtl.h>
#endif

#ifdef __SWITCH__
    #include <switch.h>
#endif

#ifdef RAD_VITA
    #include <psp2/apputil.h>
    #include <psp2/system_param.h>
#endif

#ifdef RAD_ANDROID
    #include <SDL.h>   // SDL_GetPreferredLocales
    #include <ctype.h>
    #include <string.h>
#endif

#if defined(RAD_ANDROID)
  #include <android/log.h>
  #define LOG_TAG "SimpsonsHitAndRun"
  #define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
  #define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#elif defined(RAD_VITA)
  #include <psp2/kernel/clib.h>
  #define LOGI(...) do { sceClibPrintf(__VA_ARGS__); sceClibPrintf("\n"); } while(0)
  #define LOGE(...) do { sceClibPrintf(__VA_ARGS__); sceClibPrintf("\n"); } while(0)

#else
  #include <cstdio>
  #define LOGI(...) do { std::printf(__VA_ARGS__); std::printf("\n"); std::fflush(stdout); } while(0)
  #define LOGE(...) do { std::printf(__VA_ARGS__); std::printf("\n"); std::fflush(stdout); } while(0)
#endif



//========================================
// Project Includes
//========================================

#include <presentation/language.h>

//*****************************************************************************
//
// Global Data, Local Data, Local Classes
//
//*****************************************************************************

//*****************************************************************************
//
// Public Member Functions
//
//*****************************************************************************
namespace Language{

//=============================================================================
// Language::GetHardwareLanguage()
//=============================================================================
// Description: returns the currently set language for the console.
//
// Parameters: None.
//
// Return:      Language enum specifying the current language
//
//=============================================================================
Language GetHardwareLanguage()
{
    #ifdef RAD_PS2
    ////////////////////////////////////////////////////////////
    // PS2
    ////////////////////////////////////////////////////////////
    switch ( sceScfGetLanguage() )
    {
        case SCE_DUTCH_LANGUAGE : 
        {
            return DUTCH;
        }
        case SCE_ENGLISH_LANGUAGE : 
        {
            return ENGLISH;
        }
        case SCE_FRENCH_LANGUAGE : 
        {
            return FRENCH;
        }
        case SCE_GERMAN_LANGUAGE : 
        {
            return GERMAN;
        }
        case SCE_ITALIAN_LANGUAGE : 
        {
            return ITALIAN;
        }
        case SCE_JAPANESE_LANGUAGE : 
        {
            return JAPANESE;
        }
        case SCE_PORTUGUESE_LANGUAGE : 
        {
            return PORTUGUESE;
        }
        case SCE_SPANISH_LANGUAGE : 
        {
            return SPANISH;
        }
        default : 
        {
            return UNKNOWN;
        }
    }
    #endif

    #ifdef RAD_XBOX
    ////////////////////////////////////////////////////////////
    // XBOX
    ////////////////////////////////////////////////////////////
    switch ( XGetLanguage() )
    {
        case XC_LANGUAGE_ENGLISH : 
        {
            return ENGLISH;
        }
        case XC_LANGUAGE_FRENCH : 
        {
            return FRENCH;
        }
        case XC_LANGUAGE_GERMAN : 
        {
            return GERMAN;
        }
        case XC_LANGUAGE_ITALIAN : 
        {
            return ITALIAN;
        }
        case XC_LANGUAGE_JAPANESE : 
        {
            return JAPANESE;
        }
        case XC_LANGUAGE_SPANISH : 
        {
            return SPANISH;
        }
        default : 
        {
            return UNKNOWN;
        }
    }
    #endif

    #ifdef RAD_GAMECUBE
    switch ( OSGetLanguage() )
    {
        case OS_LANG_ENGLISH:
        {
            return ENGLISH;
        }
        case OS_LANG_GERMAN:
        {
            return GERMAN;
        }
        case OS_LANG_FRENCH:
        {
            return FRENCH;
        }
        case OS_LANG_SPANISH:
        {
            return SPANISH;
        }
        case OS_LANG_ITALIAN:
        {
            return ITALIAN;
        }
        case OS_LANG_DUTCH:
        {
            return DUTCH;
        }
        default:
        {
            return UNKNOWN;
        }
    }
    #endif

    #ifdef __SWITCH__
    if( R_FAILED( setInitialize() ) )
        return UNKNOWN;

    u64 code;
    SetLanguage lang;
    Result rc = setGetSystemLanguage( &code );
    if( R_SUCCEEDED( rc ) )
        rc = setMakeLanguage( code, &lang );
    setExit();

    if( R_FAILED( rc ) )
        return UNKNOWN;

    switch ( lang )
    {
        case SetLanguage_JA:
        {
            return JAPANESE;
        }
        case SetLanguage_ENUS:
        case SetLanguage_ENGB:
        {
            return ENGLISH;
        }
        case SetLanguage_FR:
        case SetLanguage_FRCA:
        {
            return FRENCH;
        }
        case SetLanguage_DE:
        {
            return GERMAN;
        }
        case SetLanguage_IT:
        {
            return ITALIAN;
        }
        case SetLanguage_ES:
        case SetLanguage_ES419:
        {
            return SPANISH;
        }
        case SetLanguage_NL:
        {
            return DUTCH;
        }
        case SetLanguage_PT:
        case SetLanguage_PTBR:
        {
            return PORTUGUESE;
        }
        default:
        {
            return UNKNOWN;
        }
    }
    #elif RAD_VITA
    SceAppUtilInitParam init = { 0 };
    SceAppUtilBootParam boot = { 0 };
    if( sceAppUtilInit( &init, &boot ) < 0)
        return UNKNOWN;

    SceSystemParamLang lang;
    if( sceAppUtilSystemParamGetInt( SCE_SYSTEM_PARAM_ID_LANG, reinterpret_cast<int*>( &lang ) ) < 0 )
        return UNKNOWN;

    switch ( lang )
    {
        case SCE_SYSTEM_PARAM_LANG_JAPANESE:
        {
            return JAPANESE;
        }
        case SCE_SYSTEM_PARAM_LANG_ENGLISH_US:
        case SCE_SYSTEM_PARAM_LANG_ENGLISH_GB:
        {
            return ENGLISH;
        }
        case SCE_SYSTEM_PARAM_LANG_FRENCH:
        {
            return FRENCH;
        }
        case SCE_SYSTEM_PARAM_LANG_SPANISH:
        {
            return SPANISH;
        }
        case SCE_SYSTEM_PARAM_LANG_GERMAN:
        {
            return GERMAN;
        }
        case SCE_SYSTEM_PARAM_LANG_ITALIAN:
        {
            return ITALIAN;
        }
        case SCE_SYSTEM_PARAM_LANG_DUTCH:
        {
            return DUTCH;
        }
        case SCE_SYSTEM_PARAM_LANG_PORTUGUESE_PT:
        case SCE_SYSTEM_PARAM_LANG_PORTUGUESE_BR:
        {
            return PORTUGUESE;
        }
        default:
        {
            return UNKNOWN;
        }
    }


    #elif defined(RAD_ANDROID)
////////////////////////////////////////////////////////////
// ANDROID (via SDL preferred locales)
////////////////////////////////////////////////////////////
    {
        //LOGI("LANG: GetHardwareLanguage() -> ANDROID block");

        const SDL_Locale* locales = SDL_GetPreferredLocales();
        if (!locales)
        {
            //LOGI("LANG: SDL_GetPreferredLocales() returned NULL -> UNKNOWN");
            return UNKNOWN;
        }

        Language detectedLanguage = UNKNOWN;

        for (int i = 0; locales[i].language != NULL; ++i)
        {
            const char* lang = locales[i].language;

            //LOGI("LANG: locale[%d] language=%s country=%s",
              //  i,
                //locales[i].language ? locales[i].language : "(null)",
                //locales[i].country  ? locales[i].country  : "(null)");

            if (!lang || lang[0] == '\0')
            {
                continue;
            }

            // Protección básica por si SDL devolviera un código raro de 1 solo carácter.
            if (lang[1] == '\0')
            {
                //LOGI("LANG: locale[%d] language '%s' too short -> skipped", i, lang);
                continue;
            }

            char l0 = (char)tolower((unsigned char)lang[0]);
            char l1 = (char)tolower((unsigned char)lang[1]);

            if (l0 == 'e' && l1 == 'n')
            {
                detectedLanguage = ENGLISH;
                break;
            }
            else if (l0 == 'e' && l1 == 's')
            {
                detectedLanguage = SPANISH;
                break;
            }
            else if (l0 == 'f' && l1 == 'r')
            {
                detectedLanguage = FRENCH;
                break;
            }
            else if (l0 == 'd' && l1 == 'e')
            {
                detectedLanguage = GERMAN;
                break;
            }
            else if (l0 == 'i' && l1 == 't')
            {
                detectedLanguage = ITALIAN;
                break;
            }
            else if (l0 == 'n' && l1 == 'l')
            {
                detectedLanguage = DUTCH;
                break;
            }
            else if (l0 == 'p' && l1 == 't')
            {
                detectedLanguage = PORTUGUESE;
                break;
            }
            else if (l0 == 'j' && l1 == 'a')
            {
                detectedLanguage = JAPANESE;
                break;
            }

           // LOGI("LANG: locale[%d] language '%s' not supported -> trying next", i, lang);
        }

        SDL_free((void*)locales);

        if (detectedLanguage != UNKNOWN)
        {
            //LOGI("LANG: mapped -> %d", detectedLanguage);
            return detectedLanguage;
        }

        //LOGI("LANG: no supported locale found -> UNKNOWN");
        return UNKNOWN;
    }

    #elif RAD_WIN32
    ////////////////////////////////////////////////////////////
    // WIN32
    ////////////////////////////////////////////////////////////
    return UNKNOWN;     // to be implemented.
    #endif
}

} //namespace Language