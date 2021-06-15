-- LUA config file
screen =
{
    x = 148;
    y = 155;
    width = 800;
    height = 600;
    fov = 75.0;
    debug_view_state = 0;
    fullscreen = 0;
    crosshair = 0;
}

audio =
{
    sound_volume = 0.80;
    music_volume = 0.90;
    use_effects = 1;
    listener_is_player = 0;
}

render =
{
    mipmap_mode = 3;
    mipmaps = 3;
    lod_bias = 0.000;
    anisotropy = 4;
    antialias = 1;
    antialias_samples = 4;
    z_depth = 24;
    texture_border = 16;
    fog_color = {r = 255, g = 255, b = 255};
    show_fps = 1;
}

controls =
{
    mouse_sensitivity_x = 0.50;
    mouse_sensitivity_y = 0.50;

    use_joy = 0;
    joy_number = 0;
    joy_rumble = 0;

    joy_move_axis_x = 0;
    joy_move_axis_y = 1;
    joy_move_invert_x = 0;
    joy_move_invert_y = 0;
    joy_move_sensitivity = 1.50;
    joy_move_deadzone = 1500;

    joy_look_axis_x = 2;
    joy_look_axis_y = 3;
    joy_look_invert_x = 0;
    joy_look_invert_y = 1;
    joy_look_sensitivity = 1.50;
    joy_look_deadzone = 1500;
}

console =
{
    background_color = {r = 0, g = 0, b = 0, a = 200};
    commands_count = 128;
    lines_count = 18;
    height = 320;
    spacing = 1.50;
}

-- Keys binding
-- Please note that on XInput game controllers (XBOX360 and such), triggers are NOT
-- coded as joystick buttons. Instead, they have unique names: JOY_TRIGGERLEFT and
-- JOY_TRIGGERRIGHT.

dofile(base_path .. "scripts/config/control_constants.lua");

bind(ACT_UP, KEY_W);
bind(ACT_DOWN, KEY_S);
bind(ACT_LEFT, KEY_A);
bind(ACT_RIGHT, KEY_D);
bind(ACT_ACTION, KEY_LCTRL, JOY_1);
bind(ACT_JUMP, KEY_SPACE, JOY_3);
bind(ACT_ROLL, KEY_X, JOY_2);
bind(ACT_DRAWWEAPON, KEY_F);
bind(ACT_LOOK, KEY_O);
bind(ACT_WALK, KEY_LSHIFT, JOY_11);
bind(ACT_SPRINT, KEY_CAPSLOCK, JOY_TRIGGERRIGHT);
bind(ACT_CROUCH, KEY_V, JOY_TRIGGERLEFT);
bind(ACT_STEPLEFT, KEY_H);
bind(ACT_STEPRIGHT, KEY_J);
bind(ACT_LOOKUP, KEY_UP, JOY_POVUP);
bind(ACT_LOOKDOWN, KEY_DOWN, JOY_POVDOWN);
bind(ACT_LOOKLEFT, KEY_LEFT, JOY_POVLEFT);
bind(ACT_LOOKRIGHT, KEY_RIGHT, JOY_POVRIGHT);
bind(ACT_BIGMEDI, KEY_0);
bind(ACT_SMALLMEDI, KEY_9);
bind(ACT_WEAPON1, KEY_1);
bind(ACT_WEAPON2, KEY_2);
bind(ACT_WEAPON3, KEY_3);
bind(ACT_WEAPON4, KEY_4);
bind(ACT_WEAPON5, KEY_5);
bind(ACT_WEAPON6, KEY_6);
bind(ACT_WEAPON7, KEY_7);
bind(ACT_WEAPON8, KEY_8);
bind(ACT_WEAPON9, KEY_9);
bind(ACT_WEAPON10, KEY_0);
bind(ACT_BINOCULARS, KEY_MINUS);
bind(ACT_INVENTORY, KEY_ESCAPE);
bind(ACT_LOADGAME, KEY_F6);
bind(ACT_SAVEGAME, KEY_F5);
bind(ACT_CONSOLE, KEY_BACKQUOTE);
bind(ACT_SCREENSHOT, KEY_PRINTSCREEN);
