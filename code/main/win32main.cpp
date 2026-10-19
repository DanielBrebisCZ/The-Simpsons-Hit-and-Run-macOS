//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        win32main.cpp
//
// Description: This file contains the main enrty point to the game.
//
// History:     + Based on xbox main and winmain from squidney
//
//=============================================================================

//========================================
// System Includes
//========================================
// Standard Library
#include <string.h>
// Foundation Tech
#include <raddebug.hpp>
#include <radobject.hpp>
// file reading before radtech
#include <stdio.h>
#include <SDL_main.h>


#ifdef RAD_ANDROID
#include <android/log.h>
#include <unistd.h>
#endif

#ifdef __SWITCH__
#include <switch.h>
#endif

#ifdef RAD_VITA
#include <unistd.h>
#include <psp2/kernel/clib.h>
unsigned int sceLibcHeapSize = 16 * 1024 * 1024;
#endif



//========================================
// Logging helper (cross-platform)
//========================================
#if defined(RAD_ANDROID)
    #define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "SimpsonsHitAndRun", __VA_ARGS__)
#elif defined(RAD_VITA)
    #define LOGI(...) sceClibPrintf(__VA_ARGS__), sceClibPrintf("\n")
#else
    #define LOGI(...) do { printf(__VA_ARGS__); printf("\n"); fflush(stdout); } while(0)
#endif

//========================================
// Project Includes
//========================================
#include <main/game.h>
#include <main/win32platform.h>
#include <main/singletons.h>
#include <main/commandlineoptions.h>
#include <memory/memoryutilities.h>
#include <memory/srrmemory.h>
#include <p3d/entity.hpp>

//========================================
// Forward Declarations
//========================================
static void ProcessCommandLineArguments( int argc, char *argv[] );

static void ProcessCommandLineArgumentsFromFile();

static void LogOutputFunction( void *userdata, int category, SDL_LogPriority priority, const char *message )
{
#ifdef RAD_VITA
    sceClibPrintf( "%s\n", message );
	
#elif defined(RAD_ANDROID)
	LOGI("%s", message);

#else
    printf( "%s\n", message );
    fflush( stdout );
#endif
}

//=============================================================================
// Function:    WinMain
//=============================================================================
//
// Description: Main Windows entry point.
// 
// Parameters:  win32 parameters
//
// Returns:     win32 return.
//
//=============================================================================
#include <signal.h>
#include <execinfo.h>
#include <unistd.h>

static void CrashHandler(int sig)
{
    void *array[32];
    size_t size = backtrace(array, 32);
    fprintf(stderr, "\n=== CRASH DETECTED: Signal %d ===\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    fprintf(stderr, "=================================\n");
    fflush(stderr);
    _exit(1);
}

#if defined(RAD_MACOSX) || defined(__APPLE__)
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

static void SetupMacOSAssetDirectory()
{
    SDL_SetHint("SDL_MAC_PRESS_AND_HOLD", "0");

    const char *home = getenv("HOME");
    if (!home || home[0] == '\0') {
        return;
    }

    // List of candidate external directories to check in ~/Library/Application Support/
    const char *candidateSubdirs[] = {
        "Library/Application Support/The Simpsons Hit and Run",
        "Library/Application Support/The Simpsons Hit & Run",
        "Library/Application Support/SimpsonsHitAndRun",
        NULL
    };

    // Ensure the primary Application Support directory exists
    char primaryDir[1024];
    snprintf(primaryDir, sizeof(primaryDir), "%s/%s", home, candidateSubdirs[0]);
    mkdir(primaryDir, 0755);

    bool foundExternal = false;
    for (int i = 0; candidateSubdirs[i] != NULL; ++i) {
        char dirPath[1024];
        snprintf(dirPath, sizeof(dirPath), "%s/%s", home, candidateSubdirs[i]);

        // Check if this directory contains valid game data
        const char *checkFiles[] = {
            "scripts", "art", "sound", "movies",
            "dialog.rcf", "scripts.rcf", "art.rcf", "sound.rcf",
            "simpsons.ini", NULL
        };

        for (int j = 0; checkFiles[j] != NULL; ++j) {
            char filePath[1024];
            snprintf(filePath, sizeof(filePath), "%s/%s", dirPath, checkFiles[j]);
            if (access(filePath, F_OK) == 0) {
                printf("[macOS] Found game data in external directory: %s\n", dirPath);
                if (chdir(dirPath) == 0) {
                    foundExternal = true;
                    break;
                }
            }
        }
        if (foundExternal) {
            break;
        }
    }

    if (!foundExternal) {
        printf("[macOS] No external game assets found in ~/Library/Application Support/.\n");
        printf("[macOS] Using bundle/current working directory, or place assets into:\n  %s\n", primaryDir);
    }
}
#endif

extern "C" int main( int argc, char *argv[] )
{
    signal(SIGSEGV, CrashHandler);
    signal(SIGBUS, CrashHandler);
#ifdef __SWITCH__
#ifdef RAD_DEBUG
    socketInitializeDefault();
    nxlinkStdio();
#endif
    romfsInit();
#endif
#ifdef RAD_VITA
	chdir( "ux0:data/simpsons" );
#endif

#ifdef RAD_ANDROID
	//LOGI( "Android Port Initialized.");
    #ifdef SDL_HINT_ANDROID_SEPARATE_MOUSE_AND_TOUCH
    SDL_SetHint(SDL_HINT_ANDROID_SEPARATE_MOUSE_AND_TOUCH, "0");//only use touchpad
#else
    SDL_SetHint("SDL_ANDROID_SEPARATE_MOUSE_AND_TOUCH", "0");
#endif

	//LOGI( "Touchpad initialized, mouse disabled.");
#endif

#if defined(RAD_MACOSX) || defined(__APPLE__)
    SetupMacOSAssetDirectory();
#endif

	//
    // Pick out and store command line settings.
    //
	//LOGI("Initializing command line options...");
    CommandLineOptions::InitDefaults();
    ProcessCommandLineArguments( argc, argv );
    ProcessCommandLineArgumentsFromFile();

    //
    // Initialize SDL subsystems
    //
//LOGI("Initializing SDL subsystems...");	
#if SDL_MAJOR_VERSION < 3
    SDL_Init( SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER );

    SDL_LogSetOutputFunction( LogOutputFunction, NULL );
#else
    SDL_Init( SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_GAMEPAD );

    SDL_SetLogOutputFunction( LogOutputFunction, NULL );
#endif



// --------------------------------------------------
// Log SDL version (runtime vs compile-time)
// --------------------------------------------------
#if defined(RAD_ANDROID) && defined(RAD_DEBUG)
#if SDL_MAJOR_VERSION < 3
    SDL_version compiled;
    SDL_version linked;

    SDL_VERSION(&compiled);      // headers used at compile time
    SDL_GetVersion(&linked);     // library used at runtime

    LOGI("SDL (compiled): %d.%d.%d", compiled.major, compiled.minor, compiled.patch);
    LOGI("SDL (runtime):  %d.%d.%d", linked.major, linked.minor, linked.patch);

    // Extra: which backend driver got picked (useful for ports)
    LOGI("SDL Video driver: %s", SDL_GetCurrentVideoDriver() ? SDL_GetCurrentVideoDriver() : "(null)");
    LOGI("SDL Audio driver: %s", SDL_GetCurrentAudioDriver() ? SDL_GetCurrentAudioDriver() : "(null)");
#else
    int major, minor, micro;
    SDL_GetVersion(&major, &minor, &micro);

    LOGI("SDL (runtime):  %d.%d.%d", major, minor, micro);

    // Extra: drivers (SDL3 mantiene funciones similares, pero por si acaso)
    const char* v = SDL_GetCurrentVideoDriver();
    const char* a = SDL_GetCurrentAudioDriver();
    LOGI("SDL Video driver: %s", v ? v : "(null)");
    LOGI("SDL Audio driver: %s", a ? a : "(null)");
#endif
#endif
    //
    // Have to get FTech setup first so that we can use all the memory services.
    // The initialize window call will fail if another Simpsons window exists. In
    // this case, we exit.
    //
	
    if( !Win32Platform::InitializeWindow() )
    {
        return 0;
    }
    Win32Platform::InitializeFoundation();

    srand (Game::GetRandomSeed ());


    // Now disable the default heap
    //
#ifndef RAD_RELEASE
    tName::SetAllocator (GMA_DEBUG);

    //g_HeapActivityTracker.EnableHeapAllocs (GMA_DEFAULT, false);
    //g_HeapActivityTracker.EnableHeapFrees (GMA_DEFAULT, false);
#endif

    HeapMgr()->PushHeap (GMA_PERSISTENT);

    //
    // Instantiate all the singletons before doing anything else.
    //
	//LOGI( "Creating game singletons...");
    CreateSingletons();

    //
    // Construct the platform object.
    //
    Win32Platform* pPlatform = Win32Platform::CreateInstance();
    rAssert( pPlatform != NULL );

    //
    // Create the game object.
    //
    Game* pGame = Game::CreateInstance( pPlatform );
    rAssert( pGame != NULL );


    //
    // Initialize the game.
    //
    pGame->Initialize();

    HeapMgr()->PopHeap (GMA_PERSISTENT);

    //
    // Run it!  Control will not return from here until the game is stopped.
    //
    pGame->Run();

    //
    // Terminate the game (this frees all resources allocated by the game).
    //
    pGame->Terminate();

    //
    // Dump all the singletons.
    //
    DestroySingletons();

    //
    // Destroy the game object.
    //
    Game::DestroyInstance();

    //
    // Shutdown the platform.
    //
    pPlatform->ShutdownPlatform();

    //
    // Destroy the game and platform (do it in this order in case the game's 
    // destructor references the platform.
    //
    Win32Platform::DestroyInstance();

    // As a last thing, shut down the memory.
    Win32Platform::ShutdownMemory();

    // Re-enable the default heap
    //
#ifndef RAD_RELEASE
    tName::SetAllocator (RADMEMORY_ALLOC_DEFAULT);
#endif

    //
    // Shutdown SDL subsystems
    //
    SDL_Quit();

    //
    // Pass any error codes back to the operating system.
    //
    return 0;
}


//=============================================================================
// Function:    ProcessCommandLineArguments
//=============================================================================
//
// Description: Pick out the command line options and store them.
// 
// Parameters:  None.
//
// Returns:     None.
//
//=============================================================================
void ProcessCommandLineArguments(int argc, char* argv[])
{
    rDebugPrintf( "*************************************************************************\n" );
    rDebugPrintf( "Command Line Args:\n" );

    //
    // Pick out all the command line options and store them in GameDB.
    // Also dump them to the output for handy dandy viewing.
    //
    for (int i = 1; i < argc; i++ )
    {
        rDebugPrintf( "arg%d: %s\n", i, argv[i] );

        CommandLineOptions::HandleOption( argv[i] );
    }

    if( !CommandLineOptions::Get( CLO_ART_STATS ) )
    {
        //CommandLineOptions::HandleOption( "noheaps" );
    }

    rDebugPrintf( "*************************************************************************\n" );
}



void ProcessCommandLineArgumentsFromFile()
{
#ifndef FINAL

    //Chuck: looking for additional command line args being passed in from a file
    //its for QA testing etc.

    FILE* pfile = fopen( "command.txt", "r" );

    if (pfile != NULL)
    {
        int ret = fseek( pfile, 0, SEEK_END ); 
        rAssert( ret == 0 );

        int len = ftell( pfile );
        rAssertMsg( len < 256, "Command line file too large to process." );

        rewind( pfile );

        if( len > 0 && len < 256 )
        {
            char commandlinestring[256] = {0};

            fgets( commandlinestring, 256, pfile );
                    
            char* argument = strtok(commandlinestring," ");
            while (argument != NULL)
            {
                CommandLineOptions::HandleOption(argument);
                argument=strtok(NULL," ");
            }
        }

        fclose( pfile );
    }
#endif //FINAL
} //end of Function
