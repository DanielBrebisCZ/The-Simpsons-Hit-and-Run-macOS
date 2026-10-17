#ifndef TOUCHHUDRENDERER_H_
#define TOUCHHUDRENDERER_H_
#include <input/touch/touchhudsystem.h>
//=============================================================================
// TouchHudRenderer
//
// Phase 6B.1:
// - Empty renderer shell.
// - Does not draw anything yet.
// - Future responsibility:
//      * draw touch icons using TouchAssetManager sprites
//      * read logical positions from TouchHudSystem
//      * draw dynamic joystick base/knob
//      * apply opacity / pressed opacity
//=============================================================================
class tSprite;

class TouchHudRenderer
{
public:
    static TouchHudRenderer& GetInstance();

    bool Initialize();
    void Shutdown();

    bool IsInitialized() const;

    void SetEnabled( bool enabled );
    bool IsEnabled() const;

    void SetOpacity( float opacity );
    float GetOpacity() const;

    void SetPressedOpacity( float opacity );
    float GetPressedOpacity() const;

    void Render();

    void SetJoystickVisualScale( float scale );
    float GetJoystickVisualScale() const;
    void RenderCinematicSkip();
private:
    TouchHudRenderer();
    ~TouchHudRenderer();

    // Not implemented.
    TouchHudRenderer( const TouchHudRenderer& );
    TouchHudRenderer& operator=( const TouchHudRenderer& );

    float ClampOpacity( float value ) const;
    float mJoystickVisualScale;

    bool BeginTouchHud2D();
    void EndTouchHud2D();

    bool GetRenderDimensions( float& width, float& height ) const;

    void RenderVisibleControls (TouchProfile currentProfile,float renderWidth,float renderHeight);

    void RenderControlDefinition (const TouchHudControlDefinition& control,TouchProfile currentProfile,float renderWidth,float renderHeight);

    void ComputeAspectFitRect (tSprite* sprite,float sourceX,float sourceY,float sourceWidth,float sourceHeight,float& drawX,float& drawY,float& drawWidth,float& drawHeight) const;

   

     void RenderMovementJoystick
    (
        TouchProfile currentProfile,
        float renderWidth,
        float renderHeight
    );

    void RenderSpriteInPixelRect
    (
        tSprite* sprite,
        float rectX,
        float rectY,
        float rectW,
        float rectH,
        float opacity
    );



private:
    bool mInitialized;
    bool mEnabled;

    float mOpacity;
    float mPressedOpacity;
};

#endif // TOUCHHUDRENDERER_H_