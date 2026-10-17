#ifndef TOUCHASSETEXTRACTOR_H_
#define TOUCHASSETEXTRACTOR_H_

//=============================================================================
// TouchAssetExtractor
//
// Copies bundled APK assets to a real writable/readable app directory so
// Pure3D can later load PNG files using its normal file APIs.
//
// Phase 6A.2:
// - Copy a small known manifest of PNG files.
// - Do it only once unless version changes or files are missing.
// - Do not render anything.
//=============================================================================

class TouchAssetExtractor
{
public:
    static TouchAssetExtractor& GetInstance();

    bool EnsureAssetsExtracted();

    const char* GetOutputRoot() const;
    const char* GetLastError() const;

private:
    TouchAssetExtractor();
    ~TouchAssetExtractor();

    // Not implemented.
    TouchAssetExtractor( const TouchAssetExtractor& );
    TouchAssetExtractor& operator=( const TouchAssetExtractor& );

    bool BuildOutputRoot();
    bool NeedsExtraction() const;

    bool CopyKnownAssets();
    bool CopyAssetFile( const char* relativePath );

    bool EnsureDirectory( const char* path ) const;
    bool EnsureParentDirectoryForFile( const char* path ) const;
    bool FileExists( const char* path ) const;

    bool WriteVersionFile() const;
    bool IsVersionFileValid() const;

    void SetLastError( const char* error );

private:
    enum
    {
        MAX_TOUCH_PATH = 512,
        MAX_TOUCH_ERROR = 256
    };

    char mOutputRoot[ MAX_TOUCH_PATH ];
    char mLastError[ MAX_TOUCH_ERROR ];
    bool mHasRun;
};

#endif // TOUCHASSETEXTRACTOR_H_