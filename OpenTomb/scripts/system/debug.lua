-- OPENTOMB DEBUG SCRIPT
-- by Lwmte, June 2015

--------------------------------------------------------------------------------
-- Here we'll place any debug routines probably needed for testing in game.
--------------------------------------------------------------------------------

debug_game = GAME_1;
debug_level = 1;

if (debug_level == 1) then
	print("debug->loaded !");
else
	print("debug->desactivated !");
end;

function goToEntity(id)
    if(player ~= nil) then
        setEntityPos(player, getEntityPos(id));
    end;
end

function gtp(id)
    setCharacterPathTarget(id, player);
end

function checkDebugKeys()
    if(checkKey(KEY_RETURN, true)) then
        setGame(debug_game, debug_level);
        debug_level = debug_level + 1;
        if(debug_game == GAME_1) then
            if(debug_level == 20) then
                debug_game = GAME_1_5;
                debug_level = 1;
            end;
        elseif(debug_game == GAME_1_5) then
            if(debug_level == 5) then
                debug_game = GAME_2;
                debug_level = 1;
            end;
        elseif(debug_game == GAME_2) then
            if(debug_level == 19) then
                debug_game = GAME_2_5;
                debug_level = 1;
            end;
        elseif(debug_game == GAME_2_5) then
            if(debug_level == 6) then
                debug_game = GAME_3;
                debug_level = 1;
            end;
        elseif(debug_game == GAME_3) then
            if(debug_level == 21) then
                debug_game = GAME_3_5;
                debug_level = 1;
            end;
        elseif(debug_game == GAME_3_5) then
            if(debug_level == 7) then
                debug_game = GAME_4;
                debug_level = 1;
            end;
        elseif(debug_game == GAME_4) then
            if(debug_level == 39) then
                debug_game = GAME_5;
                debug_level = 1;
            end;
        elseif(debug_game == GAME_5) then
            if(debug_level == 15) then
                debug_game = GAME_1;
                debug_level = 1;
            end;
        end;
    end;

    if(checkKey(KEY_R, true)) then
        print("Ragdoll activated!");
        setCharacterRagdollActivity(player, true);
    end;

    if(checkKey(KEY_T, true)) then
        print("Ragdoll deactivated!");
        setCharacterRagdollActivity(player, false);
    end;

    if(checkKey(KEY_H, true)) then
        setCharacterParam(player, PARAM_HEALTH, PARAM_ABSOLUTE_MAX);
        playSound(SOUND_MEDIPACK, player);
    end;

    if(checkKey(KEY_Z, true)) then
        setEntityCollision(player, true);
        setCharacterRagdollActivity(player, false);
        setCharacterState(player, CHARACTER_STATE_DEAD, 0);
        setCharacterParam(player, PARAM_HEALTH, PARAM_ABSOLUTE_MAX);
        setEntityAnimFlag(player, ANIM_TYPE_BASE, ANIM_NORMAL_CONTROL);
        setCharacterKeyAnim(player, ANIM_TYPE_BASE, ANIMATION_KEY_INIT);
        setEntityActivity(player, true);
    end;

    return true;
end;