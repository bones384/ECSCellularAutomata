//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
// StyleAsCode exporter v2.0 - Style data exported as a values array            //
//                                                                              //
// USAGE: On init call: GuiLoadStyleLightwawa();                                   //
//                                                                              //
// more info and bugs-report:  github.com/raysan5/raygui                        //
// feedback and support:       ray[at]raylibtech.com                            //
//                                                                              //
// Copyright (c) 2020-2023 raylib technologies (@raylibtech)                    //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#define LIGHTWAWA_STYLE_PROPS_COUNT  14

// Custom style name: Lightwawa
static const GuiStyleProp lightwawaStyleProps[LIGHTWAWA_STYLE_PROPS_COUNT] = {
    { 0, 0, 0x545454ff },    // DEFAULT_BORDER_COLOR_NORMAL 
    { 0, 1, 0x050505ff },    // DEFAULT_BASE_COLOR_NORMAL 
    { 0, 2, 0xf5f5f5ff },    // DEFAULT_TEXT_COLOR_NORMAL 
    { 0, 3, 0x919191ff },    // DEFAULT_BORDER_COLOR_FOCUSED 
    { 0, 4, 0x141313ff },    // DEFAULT_BASE_COLOR_FOCUSED 
    { 0, 5, 0xffffffff },    // DEFAULT_TEXT_COLOR_FOCUSED 
    { 0, 6, 0x2b2b2bff },    // DEFAULT_BORDER_COLOR_PRESSED 
    { 0, 7, 0x474747ff },    // DEFAULT_BASE_COLOR_PRESSED
    { 0, 8, 0xff0909ff },    // DEFAULT_TEXT_COLOR_PRESSED 
    { 0, 9, 0x474747ff },    // DEFAULT_BORDER_COLOR_DISABLED 
    { 0, 10, 0x030303ff },    // DEFAULT_BASE_COLOR_DISABLED 
    { 0, 11, 0x505050ff },    // DEFAULT_TEXT_COLOR_DISABLED 
    { 0, 18, 0xb4b2b2ff },    // DEFAULT_LINE_COLOR 
    { 0, 19, 0x010101ff },    // DEFAULT_BACKGROUND_COLOR 
};

// WARNING: This style uses a custom font: "font.ttf" (size: 10, spacing: 1)


// Style loading function: Lightwawa
static void GuiLoadStyleLightwawa(void)
{
    // Load style properties provided
    // NOTE: Default properties are propagated
    for (int i = 0; i < LIGHTWAWA_STYLE_PROPS_COUNT; i++)
    {
        GuiSetStyle(lightwawaStyleProps[i].controlId, lightwawaStyleProps[i].propertyId, lightwawaStyleProps[i].propertyValue);
    }

}
