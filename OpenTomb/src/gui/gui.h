
#ifndef ENGINE_GUI_H
#define ENGINE_GUI_H

#include <stdint.h>

// Anchoring is needed to link specific GUI element to specific screen position,
// independent of screen resolution and aspect ratio. Vertical and horizontal
// anchorings are seperated, so you can link element at any place - top, bottom,
// center, left or right.

#define GUI_ANCHOR_VERT_TOP         0
#define GUI_ANCHOR_VERT_BOTTOM      1
#define GUI_ANCHOR_VERT_CENTER      2

#define GUI_ANCHOR_HOR_LEFT         0
#define GUI_ANCHOR_HOR_RIGHT        1
#define GUI_ANCHOR_HOR_CENTER       2

// Immutable bars enumeration.
// These are the bars that are always exist in GUI.
// Scripted bars could be created and drawn separately later.

enum Bars
{
    BAR_HEALTH,     // TR 1-5
    BAR_AIR,        // TR 1-5, alternate state - gas (TR5)
    BAR_STAMINA,    // TR 3-5
    BAR_WARMTH,     // TR 3 only
    BAR_LOADING,
    BAR_LASTINDEX
};

// Bar color types.
// Each bar part basically has two colours - main and fade.

enum BarColorType
{
    BASE_MAIN,
    BASE_FADE,
    ALT_MAIN,
    ALT_FADE,
    BACK_MAIN,
    BACK_FADE,
    BORDER_MAIN,
    BORDER_FADE
};

// Main bar class.
class gui_ProgressBar
{
public:
    gui_ProgressBar();  // Bar constructor.

    void Show(float value);    // Main show bar procedure.
    void Resize();

    void SetColor(BarColorType colType, uint8_t R, uint8_t G, uint8_t B, uint8_t alpha);
    void SetSize(float width, float height, float borderSize);
    void SetPosition(int8_t anchor_X, float offset_X, int8_t anchor_Y, float offset_Y);
    void SetValues(float maxValue, float warnValue);
    void SetBlink(int interval);
    void SetExtrude(bool enabled, uint8_t depth);
    void SetAutoshow(bool enabled, int delay, bool fade, int fadeDelay);

    bool          Forced;               // Forced flag is set when bar is strictly drawn.
    bool          Visible;              // Is it visible or not.
    bool          Alternate;            // Alternate state, in which bar changes color to AltColor.

    bool          Invert;               // Invert decrease direction flag.
    bool          Vertical;             // Change bar style to vertical.

private:
    void          RecalculateSize();    // Recalculate size and position.
    void          RecalculatePosition();

    float         mX;                   // Horizontal position.
    float         mY;                   // Vertical position.
    float         mWidth;               // Real width.
    float         mHeight;              // Real height.
    float         mBorderWidth;         // Real border size (horizontal).
    float         mBorderHeight;        // Real border size (vertical).

    int8_t        mXanchor;             // Horizontal anchoring: left, right or center.
    int8_t        mYanchor;             // Vertical anchoring: top, bottom or center.
    float         mAbsXoffset;          // Absolute (resolution-independent) X offset.
    float         mAbsYoffset;          // Absolute Y offset.
    float         mAbsWidth;            // Absolute width.
    float         mAbsHeight;           // Absolute height.
    float         mAbsBorderSize;       // Absolute border size (horizontal).

    float         mBaseMainColor[5];    // Color at the min. of bar.
    float         mBaseFadeColor[5];    // Color at the max. of bar.
    float         mAltMainColor[5];     // Alternate main color.
    float         mAltFadeColor[5];     // Alternate fade color.
    float         mBackMainColor[5];    // Background main color.
    float         mBackFadeColor[5];    // Background fade color.
    float         mBorderMainColor[5];  // Border main color.
    float         mBorderFadeColor[5];  // Border fade color.

    int8_t        mBaseBlendingMode;    // Blending modes for all bar parts.
    int8_t        mBackBlendingMode;    // Note there is no alt. blending mode, cause
    int8_t        mBorderBlendingMode;  // base and alt are actually the same part.

    bool          mExtrude;             // Extrude effect.
    float         mExtrudeDepth[5];     // Extrude effect depth.

    float         mMaxValue;            // Maximum possible value.
    float         mWarnValue;           // Warning value, at which bar begins to blink.
    float         mLastValue;           // Last value back-up for autoshow on change event.

    bool          mBlink;               // Warning state (blink) flag.
    float         mBlinkInterval;       // Blink interval (speed).
    float         mBlinkCnt;            // Blink counter.

    bool          mAutoShow;            // Autoshow on change flag.
    float         mAutoShowDelay;       // How long bar will stay on-screen in AutoShow mode.
    float         mAutoShowCnt;         // Auto-show counter.
    bool          mAutoShowFade;        // Fade flag.
    float         mAutoShowFadeDelay;   // Fade length.
    float         mAutoShowFadeCnt;     // Fade progress counter.

    float         mRangeUnit;           // Range unit used to set base bar size.
    float         mBaseSize;            // Base bar size.
    float         mBaseRatio;           // Max. / actual value ratio.
};

void Gui_Init();
void Gui_InitBars();
void Gui_Destroy();

void Gui_ConScroll(int value);
void Gui_ConShow(int value);
int  Gui_ConIsShown();

/**
 * Helper method to setup OpenGL state for console drawing.
 *
 * Either changes to 2D matrix state (is_gui = 1) or away from it (is_gui = 0). Does not do any drawing.
 */
void Gui_SwitchGLMode(char is_gui);

/**
 * Draws wireframe of this frustum.
 *
 * Expected state:
 *  - Vertex array is enabled, color, tex coord, normal disabled
 *  - No vertex buffer object is bound
 *  - Texturing is disabled
 *  - Alpha test is disabled
 *  - Blending is enabled
 *  - Lighting is disabled
 * Ignored state:
 *  - Currently bound texture.
 *  - Currently bound element buffer.
 *  - Depth test enabled (disables it, then restores)
 *  - Vertex pointer (changes it)
 *  - Matrices (changes them, restores)
 *  - Line width (changes it, then restores)
 *  - Current color (changes it)
 * Changed state:
 *  - Current position will be arbitrary.
 *  - Vertex pointer will be arbitray.
 *  - Current color will be arbitray (set by console)
 *  - Blend mode will be SRC_ALPHA, ONE_MINUS_SRC_ALPHA (set by console)
 */
void Gui_Render();

/**
 *  Draw simple rectangle.
 *  Only state it changes is the blend mode, according to blendMode value.
 */
void Gui_DrawRect(const GLfloat &x, const GLfloat &y,
                  const GLfloat &width, const GLfloat &height,
                  const GLfloat colorUpperLeft[], const GLfloat colorUpperRight[],
                  const GLfloat colorLowerLeft[], const GLfloat colorLowerRight[],
                  const int &blendMode,
                  const GLuint texture = 0);

/**
 * General GUI drawing routines.
 */
void Gui_SetCurrentMenu(struct gui_object_s *menu);
struct gui_object_s *Gui_GetCurrentMenu();

void Gui_DrawCrosshair();
void Gui_DrawBars();
void Gui_DrawLoadScreen(int value);
bool Gui_SetScreenTexture(void *data, int w, int h, int bpp);
bool Gui_LoadScreenAssignPic(const char* pic_name);

/**
 * General GUI update routines.
 */
void Gui_UpdateResize();  // Called every resize event.

#endif
