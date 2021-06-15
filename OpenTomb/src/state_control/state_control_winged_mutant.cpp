
#include <stdlib.h>
#include <stdio.h>

#include "../core/system.h"
#include "../core/console.h"
#include "../core/vmath.h"

#include "../render/camera.h"
#include "../script/script.h"
#include "../vt/tr_versions.h"
#include "../engine.h"
#include "../controls.h"
#include "../room.h"
#include "../world.h"
#include "../skeletal_model.h"
#include "../entity.h"
#include "../character_controller.h"
#include "state_control_winged_mutant.h"
#include "state_control.h"


void StateControl_WingedMutantSetKeyAnim(struct entity_s *ent, struct ss_animation_s *ss_anim, int key_anim)
{
    switch(key_anim)
    {
        case ANIMATION_KEY_INIT:
            if(ent->move_type == MOVE_FLY)
            {
                Anim_SetAnimation(ss_anim, TR_ANIMATION_WINGED_MUTANT_FLY, 0);
            }
            else
            {
                Anim_SetAnimation(ss_anim, TR_ANIMATION_WINGED_MUTANT_STAY_INIT, 0);
            }
            break;

        case ANIMATION_KEY_DEAD:
            break;
    }
}


int StateControl_WingedMutant(struct entity_s *ent, struct ss_animation_s *ss_anim)
{
    character_command_p cmd = &ent->character->cmd;
    character_state_p state = &ent->character->state;
    uint16_t current_state = Anim_GetCurrentState(ss_anim);
    height_info_p hi = &ent->character->height_info;
    float *pos = ent->transform.M4x4 + 12;

    ent->character->rotate_speed_mult = 1.0f;
    ss_anim->anim_frame_flags = ANIM_NORMAL_CONTROL;

    state->sprint = 0x00;
    state->attack = 0x00;

    switch(current_state)
    {
        case TR_STATE_WINGED_MUTANT_JUMP_ATTACK:
        case TR_STATE_WINGED_MUTANT_RUN_ATTACK:
        case TR_STATE_WINGED_MUTANT_STAY_ATTACK:
            state->attack = 0x01;
            break;

        case TR_STATE_WINGED_MUTANT_STAY_INIT:
            if(cmd->action || (cmd->move[0] > 0))
            {
                ss_anim->target_state = TR_STATE_WINGED_MUTANT_STAY;
            }
            else
            {
                ss_anim->target_state = TR_STATE_WINGED_MUTANT_STAY_INIT;
            }
            break;

        case TR_STATE_WINGED_MUTANT_STAY: // -> 3' -> 4' -> 6' -> 8' -> 9' -> 10' -> 13'
            if(cmd->action)
            {
                ss_anim->target_state = (cmd->shift) ? (TR_STATE_WINGED_MUTANT_STAY_AIM) : (TR_STATE_WINGED_MUTANT_STAY_FAST_SHOOT);
            }
            else if(cmd->jump)
            {
                ss_anim->target_state = (cmd->shift) ? (TR_STATE_WINGED_MUTANT_STAY_ATTACK) : (TR_STATE_WINGED_MUTANT_JUMP_ATTACK);
            }
            else if(cmd->move[0] < 0)
            {
                ent->move_type = MOVE_FLY;
                ss_anim->target_state = TR_STATE_WINGED_MUTANT_FLY;
            }
            else if(cmd->move[0] > 0)
            {
                ss_anim->target_state = (cmd->shift) ? (TR_STATE_WINGED_MUTANT_STAY_ALERT) : (TR_STATE_WINGED_MUTANT_RUN);
            }
            else
            {
                ss_anim->target_state = TR_STATE_WINGED_MUTANT_STAY;
            }
            break;

        case TR_STATE_WINGED_MUTANT_STAY_ALERT: // -> 1 -> 2
            if(cmd->shift)
            {
                ss_anim->target_state = (cmd->move[0] > 0) ? (TR_STATE_WINGED_MUTANT_WALK) : (TR_STATE_WINGED_MUTANT_STAY_ALERT);
            }
            else
            {
                ss_anim->target_state = TR_STATE_WINGED_MUTANT_STAY;
            }
            break;

        case TR_STATE_WINGED_MUTANT_RUN: // -> 1 -> 7
            ent->dir_flag = ENT_MOVE_FORWARD;
            if(cmd->action)
            {
                ss_anim->target_state = TR_STATE_WINGED_MUTANT_RUN_ATTACK;
            }
            else if(!cmd->shift && (cmd->move[0] > 0))
            {
                ss_anim->target_state = TR_STATE_WINGED_MUTANT_RUN;
            }
            else
            {
                ss_anim->target_state = TR_STATE_WINGED_MUTANT_STAY;
            }
            break;

        case TR_STATE_WINGED_MUTANT_WALK: // -> 1 -> 6
            ent->dir_flag = ENT_MOVE_FORWARD;
            if(cmd->shift && (cmd->move[0] > 0))
            {
                ss_anim->target_state = TR_STATE_WINGED_MUTANT_WALK;
            }
            else
            {
                ss_anim->target_state = (cmd->shift) ? (TR_STATE_WINGED_MUTANT_STAY_ALERT) : (TR_STATE_WINGED_MUTANT_STAY);
            }
            break;

        case TR_STATE_WINGED_MUTANT_STAY_FAST_SHOOT: // -> 1 -> 11
        case TR_STATE_WINGED_MUTANT_STAY_AIM: // -> 1 -> 11
            if(cmd->action)
            {
                ss_anim->target_state = TR_STATE_WINGED_MUTANT_STAY_SHOOT;
                entity_p target = World_GetEntityByID(ent->character->target_id);
                if((ss_anim->frame_changing_state >= 0x02) && (!target || Character_IsTargetAccessible(ent, target)))
                {
                    Script_ExecEntity(engine_lua, ENTITY_CALLBACK_SHOOT, ent->id, target->id);
                }
            }
            else
            {
                ss_anim->target_state = TR_STATE_WINGED_MUTANT_STAY;
            }
            break;

        case TR_STATE_WINGED_MUTANT_FLY:
            if((ent->move_type != MOVE_FLY) || (cmd->crouch && hi->floor_hit.hit && (pos[2] < hi->floor_hit.point[2] + 256.0f)))
            {
                ent->move_type = MOVE_ON_FLOOR;
                ss_anim->target_state = TR_STATE_WINGED_MUTANT_STAY;
            }
            else
            {
                ss_anim->target_state = TR_STATE_WINGED_MUTANT_FLY;
            }
            break;
    };

    if(ent->character->state.slide == CHARACTER_SLIDE_BACK)
    {
        ent->dir_flag = ENT_MOVE_BACKWARD;
        ent->anim_linear_speed = 64;
    }
    else if(ent->character->state.slide == CHARACTER_SLIDE_FRONT)
    {
        ent->dir_flag = ENT_MOVE_FORWARD;
        ent->anim_linear_speed = 64;
    }

    return 0;
}
