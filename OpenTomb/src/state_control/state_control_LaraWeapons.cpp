#include "../weapons.h"

#include "../core/system.h"
#include "../core/vmath.h"
#include "../core/obb.h"

#include "../script/script.h"
#include "../audio/audio.h"
#include "../inventory.h"
#include "../entity.h"
#include "../room.h"
#include "../world.h"
#include "../weapons.h"

#include "../character_controller.h"
#include "state_control_Lara.h"


// Mesh for Two Hand and One Hand
#define HAND_RIGHT 10
#define HAND_LEFT 13
#define HOLSTER_LEFT 1
#define HOLSTER_RIGHT 4
#define TR1_BACK_WEAPON 7
#define TR2_BACK_WEAPON 14       // tr2 to tr5
#define TR4_REVOLVER_LASERSIGHT_MESH 4
#define TR4_CROSSBOW_LASERSIGHT_MESH 1

// Two Hand Frame Start and End
#define TW_FRAME 13
#define TW_FRAME_GRENADEGUN_IS_END 14
#define TW_FRAME_IS_END 23



/// Custom TR Define for State Control (sound, reload, fire, etc..)
/// it's for custom level (TR4) to add all weapon in same game :)
#define CTDS_PISTOL             3
#define CTDS_SHOTGUN            4
#define CTDS_AUTOPISTOL         5
#define CTDS_UZI                6
#define CTDS_DESERT_EAGLE       7
#define CTDS_M16                8
#define CTDS_MP5                9
#define CTDS_GRENADE_LAUNCHER   10
#define CTDS_ROCKET_LAUNCHER    11
#define CTDS_HARPOON_LAUNCHER   12

#define ANIM_IDLE_TO_FIRING 0
#define ANIM_HIDE_TO_IDLE 1
#define ANIM_IDLE_AND_HIDE 2
#define ANIM_FIRING 3

#define SHOTGUN_IDLE_TO_FIRING 0
#define SHOTGUN_HIDE_TO_IDLE 1
#define SHOTGUN_FIRING 2
#define SHOTGUN_IDLE_TO_HIDE 3
#define SHOTGUN_FIRING_TO_IDLE 4

#define HARPOON_LAND_IDLE_TO_FIRING 0
#define HARPOON_LAND_HIDE_TO_IDLE 1
#define HARPOON_LAND_FIRING 2
#define HARPOON_LAND_IDLE_TO_HIDE 3
#define HARPOON_LAND_FIRING_TO_IDLE 4
#define HARPOON_RELOAD 5
#define HARPOON_WATER_IDLE_TO_FIRING 6
#define HARPOON_WATER_FIRING_TO_IDLE 7
#define HARPOON_WATER_FIRING 8
#define HARPOON_WATER_IDLE_TO_HIDE 9
#define HARPOON_WATER_ALT_IDLE_TO_HIDE 10   // not used !

#define M16_IDLE_TO_FIRING 0
#define M16_HIDE_TO_IDLE 1
#define M16_FIRING 2
#define M16_IDLE_TO_HIDE 3
#define M16_FIRING_TO_IDLE 4
#define M16_MOVE_IDLE_TO_AIM 5
#define M16_MOVE_FIRING_TO_IDLE 6
#define M16_MOVE_FIRING 7

#define MP5_IDLE_TO_FIRING 0
#define MP5_HIDE_TO_IDLE 1
#define MP5_FIRING 2
#define MP5_IDLE_TO_HIDE 3
#define MP5_FIRING_TO_IDLE 4
#define MP5_MOVE_IDLE_TO_AIM 5
#define MP5_MOVE_FIRING_TO_IDLE 6
#define MP5_MOVE_FIRING 7

#define GRENADEGUN_HIDE_TO_IDLE 0
#define GRENADEGUN_IDLE_TO_FIRING 1     // -> firing (not reload ?)
#define GRENADEGUN_FIRING_RELOAD 2      // -> firing
#define GRENADEGUN_FIRING 3             // -> reload
#define GRENADEGUN_FIRING_TO_IDLE 4
#define GRENADEGUN_IDLE_TO_HIDE 5

#define ROCKETGUN_IDLE_TO_FIRING 0
#define ROCKETGUN_HIDE_TO_IDLE 1
#define ROCKETGUN_FIRING 2
#define ROCKETGUN_IDLE_TO_HIDE 3
#define ROCKETGUN_FIRING_TO_IDLE 4

#define CROSSBOW_IDLE_TO_FIRING 0
#define CROSSBOW_HIDE_TO_IDLE 1
#define CROSSBOW_FIRING 2
#define CROSSBOW_IDLE_TO_HIDE 3
#define CROSSBOW_FIRING_TO_IDLE 4

/// this weapon can only found at last level of tr5
#define GRAPPLING_IDLE_TO_FIRING 0
#define GRAPPLING_HIDE_TO_IDLE 1
#define GRAPPLING_FIRING 2             // no frame
#define GRAPPLING_IDLE_TO_HIDE 3
#define GRAPPLING_FIRING_TO_IDLE 4


// if you define (AMMO_UNLIMITED) the item have unlimited ammo
bool checkCanShoot(struct weapons_s *weapon)
{
    entity_s* player = World_GetPlayer();

    if(weapon->current_ammo == AMMO_UNLIMITED)
    {
        return true;
    }
    // ammo are empty when is 0 or go to negative ?
    else if(Inventory_GetItemsCount(player->inventory, weapon->current_ammo) > AMMO_EMPTY)
    {
        return true;
    }

    return false;
}

bool consumeAmmo(struct weapons_s *weapon)
{
    entity_p player = World_GetPlayer();

    // checking if ammo exists before consume !
    if(Inventory_GetItemsCount(player->inventory, weapon->current_ammo) > AMMO_EMPTY)
    {
        // checking if the current ammo is not empty or unlimited !
        if(weapon->current_ammo != AMMO_EMPTY && weapon->current_ammo != AMMO_UNLIMITED)
        {
            // consume item
            Inventory_RemoveItem(&player->inventory, weapon->current_ammo, 1);
            return true;
        }
    }

    // item not found else if not equal to current ammo
    return false;
}

int CurrentWeaponModelToItemID(struct ss_animation_s *ss_anim)
{
    int32_t ver = World_GetVersion();
    if(ss_anim->model->id == TR_MODEL_PISTOL)
    {
        return ITEM_PISTOL;
    }
    else if((ver >= TR_II) && (ss_anim->model->id == TR_MODEL_SHOTGUN))
    {
        return ITEM_SHOTGUN;
    }

    if(IS_TR_I(ver))
    {
        if(ss_anim->model->id == TR1_MODEL_SHOTGUN)
        {
            return ITEM_SHOTGUN;
        }
        else if(ss_anim->model->id == TR1_MODEL_MAGNUM)
        {
            return ITEM_MAGNUMS;
        }
        else if(ss_anim->model->id == TR1_MODEL_UZI)
        {
            return ITEM_UZIS;
        }
    }
    else if(IS_TR_II(ver))
    {
        if(ss_anim->model->id == TR2_MODEL_AUTOMAGS)
        {
            return ITEM_AUTOMAGS;
        }
        else if(ss_anim->model->id == TR2_MODEL_UZI)
        {
            return ITEM_UZIS;
        }
        else if(ss_anim->model->id == TR2_MODEL_M16)
        {
            return ITEM_M16;
        }
        else if(ss_anim->model->id == TR2_MODEL_GRENADEGUN)
        {
            return ITEM_GRENADEGUN;
        }
        else if(ss_anim->model->id == TR2_MODEL_HARPOONGUN)
        {
            return ITEM_HARPOONGUN;
        }
    }
    else if(IS_TR_III(ver))
    {
        if(ss_anim->model->id == TR3_MODEL_DESERTEAGLE)
        {
            return ITEM_DESERTEAGLE;
        }
        else if(ss_anim->model->id == TR3_MODEL_UZI)
        {
            return ITEM_UZIS;
        }
        else if(ss_anim->model->id == TR3_MODEL_MP5)
        {
            return ITEM_MP5;
        }
        else if(ss_anim->model->id == TR3_MODEL_ROCKETGUN)
        {
            return ITEM_ROCKETGUN;
        }
        else if(ss_anim->model->id == TR3_MODEL_GRENADEGUN)
        {
            return ITEM_GRENADEGUN;
        }
        else if(ss_anim->model->id == TR3_MODEL_HARPOONGUN)
        {
            return ITEM_HARPOONGUN;
        }
    }
    else if(IS_TR_IV(ver))
    {
        if(ss_anim->model->id == TR4C_MODEL_UZI)
        {
            return ITEM_UZIS;
        }
        else if(ss_anim->model->id == TR4C_MODEL_CROSSBOW)
        {
            return ITEM_CROSSBOW;
        }
        else if(ss_anim->model->id == TR4C_MODEL_GRENADEGUN)
        {
            return ITEM_GRENADEGUN;
        }
        else if(ss_anim->model->id == TR4C_MODEL_REVOLVER)
        {
            return ITEM_REVOLVER;
        }
    }
    else if(IS_TR_V(ver))
    {
        if(ss_anim->model->id == TR4C_MODEL_UZI)
        {
            return ITEM_UZIS;
        }
        else if(ss_anim->model->id == TR4C_MODEL_CROSSBOW)
        {
            return ITEM_GRAPPLEGUN;  // it's grappling gun !
        }
        else if(ss_anim->model->id == TR4C_MODEL_GRENADEGUN)
        {
            return ITEM_MP5;         // it's h&k gun !
        }
        else if(ss_anim->model->id == TR4C_MODEL_REVOLVER)
        {
            return ITEM_REVOLVER;
        }
    }

    // no id found
    return 0;
}

// if ammo is empty and you try to firing.
void AutoSelect(int model_id, struct ss_animation_s *ss_anim, struct entity_s *ent, float time)
{
    // needed
    ent->character->state.weapon_ready = 0;

    // animation only for one hand !
    if(ss_anim->model->animation_count == 4)
    {
        Anim_SetAnimation(ss_anim, 2, -1);  // hide and draw
        
        // when animation is finished, change weapon to model_id
        if(Anim_IncTime(ss_anim, time))
        {
            Character_ChangeWeapon(ent, model_id);
        }
    }
    else if(ss_anim->model->animation_count > 4)
    {
        Anim_SetAnimation(ss_anim, 3, 0);   // hide (back)
    }
}

void SetCurrentWeaponAnimation(struct entity_s *ent, struct ss_animation_s *ss_anim, float time, struct weapons_s *weapon, struct ss_bone_tag_s *b_tag, struct entity_s *target, float *target_pos, bool do_aim, uint16_t targeted_bone_start, uint16_t targeted_bone_end)
{
    b_tag->is_targeted = (target) ? (0x01) : (0x00);

    switch (ss_anim->current_animation)
    {
        case ANIM_IDLE_TO_FIRING:
            OneHand_IdleToFiring(ent, ss_anim, weapon, b_tag, target, do_aim, time);
            break;

        case ANIM_HIDE_TO_IDLE:
            OneHand_HideToIdle(ent, ss_anim, weapon, time);
            b_tag->is_targeted = 0x00;
            break;

        case ANIM_IDLE_AND_HIDE:
            OneHand_HideAndIdle(ent, ss_anim, weapon, time);
            b_tag->is_targeted = 0x00;
            break;

        case ANIM_FIRING:
            if(checkCanShoot(weapon))
            {
                OneHand_Firing(ent, ss_anim, b_tag, target, target_pos, weapon, time, targeted_bone_start, targeted_bone_end, ANIM_FIRING, ANIM_IDLE_TO_FIRING);
            }
            else
            {
                AutoSelect(TR_MODEL_PISTOL, ss_anim, ent, time);
            }
            break;
    }
}

void ShotgunAnim(struct entity_s *ent,  ss_animation_s *ss_anim, float time, struct weapons_s *weapon, struct ss_bone_tag_s *b_tag, struct entity_s *target, float *target_pos, int inc_state, bool do_aim)
{
    switch (ss_anim->current_animation)
    {
        case SHOTGUN_IDLE_TO_FIRING:
            inc_state = Anim_IncTime(ss_anim, (ent->character->state.weapon_ready && do_aim) ? (time) : (-time));
            TwoHand_IdleToFiring(ent, ss_anim, weapon, b_tag, target, do_aim, time, inc_state, SHOTGUN_IDLE_TO_HIDE, SHOTGUN_FIRING);
            break;

        case SHOTGUN_HIDE_TO_IDLE:
            TwoHand_HideToIdle(ent, ss_anim, weapon, time, SHOTGUN_IDLE_TO_FIRING);
            break;

        case SHOTGUN_FIRING:
            if(checkCanShoot(weapon))
            {
                TwoHand_Firing(ent, ss_anim, b_tag, target, target_pos, weapon, time, SHOTGUN_FIRING, SHOTGUN_FIRING_TO_IDLE, SHOTGUN_IDLE_TO_FIRING);
            }
            else
            {
                AutoSelect(TR_MODEL_PISTOL, ss_anim, ent, time);
            }
            break;

        case SHOTGUN_IDLE_TO_HIDE:
            TwoHand_IdleToHide(ent, ss_anim, weapon, time, TW_FRAME_IS_END);
            break;

        case SHOTGUN_FIRING_TO_IDLE:
            TwoHand_FiringToIdle(ent, ss_anim, weapon, time, SHOTGUN_IDLE_TO_FIRING);
            break;
    }
}

void GrenadeGunAnim(struct entity_s *ent, struct ss_animation_s *ss_anim, float time, struct weapons_s *weapon, struct ss_bone_tag_s *b_tag, struct entity_s *target, float *target_pos, int inc_state, bool do_aim)
{
    int32_t ver = World_GetVersion();
    switch (ss_anim->current_animation)
    {
        case GRENADEGUN_IDLE_TO_FIRING:
            inc_state = Anim_IncTime(ss_anim, (ent->character->state.weapon_ready && do_aim) ? (time) : (-time));
            TwoHand_IdleToFiring(ent, ss_anim, weapon, b_tag, target, do_aim, time, inc_state, GRENADEGUN_IDLE_TO_HIDE, GRENADEGUN_FIRING);
            break;

        case GRENADEGUN_HIDE_TO_IDLE:
            TwoHand_HideToIdle(ent, ss_anim, weapon, time, GRENADEGUN_IDLE_TO_FIRING);
            break;

        case GRENADEGUN_FIRING_RELOAD:
            switch (ss_anim->current_frame)
            {
                case 26:
                    if((ver >= TR_II) && (ver < TR_IV))
                    {
                        Audio_Send(TR_AUDIO_SOUND_TR2_3_RELOADGRENADEGUN, TR_AUDIO_EMITTER_ENTITY, ent->id);
                    }
                    else if(ver >= TR_IV)
                    {
                        Audio_Send(TR_AUDIO_SOUND_TR4C_RELOADGRENADEGUN, TR_AUDIO_EMITTER_ENTITY, ent->id);
                    }
                    break;

                case 33:
                    if((ver >= TR_II) && (ver < TR_IV))
                    {
                        Audio_Send(TR_AUDIO_SOUND_TR2_3_RELOADGRENADEGUN_LOCK, TR_AUDIO_EMITTER_ENTITY, ent->id);
                    }
                    else if(ver >= TR_IV)
                    {
                        Audio_Send(TR_AUDIO_SOUND_TR4C_RELOADGRENADEGUN_LOCK, TR_AUDIO_EMITTER_ENTITY, ent->id);
                    }
                    break;
            }
            
            if(Anim_IncTime(ss_anim, time))
            {
                if(ent->character->cmd.action)
                {
                    Anim_SetAnimation(ss_anim, GRENADEGUN_FIRING, 0);
                }
                else
                {
                    Anim_SetAnimation(ss_anim, GRENADEGUN_FIRING_TO_IDLE, 0);
                }
            }
            break;

        case GRENADEGUN_FIRING:
            if(checkCanShoot(weapon))
            {
                TwoHand_Firing(ent, ss_anim, b_tag, target, target_pos, weapon, time, GRENADEGUN_FIRING_RELOAD, GRENADEGUN_FIRING_TO_IDLE, GRENADEGUN_IDLE_TO_FIRING);
            }
            else
            {
                AutoSelect(TR_MODEL_PISTOL, ss_anim, ent, time);
            }
            break;

        case GRENADEGUN_FIRING_TO_IDLE:
            TwoHand_FiringToIdle(ent, ss_anim, weapon, time, GRENADEGUN_IDLE_TO_FIRING);
            break;

        case GRENADEGUN_IDLE_TO_HIDE:
            TwoHand_IdleToHide(ent, ss_anim, weapon, time, TW_FRAME_GRENADEGUN_IS_END);
            break;
    }
}

void HarpoonAnim(struct entity_s *ent, struct ss_animation_s *ss_anim, float time, struct weapons_s *weapon, struct ss_bone_tag_s *b_tag, struct entity_s *target, float *target_pos, int inc_state, bool do_aim)
{
    switch (ss_anim->current_animation)
    {
        case HARPOON_LAND_IDLE_TO_FIRING:
            if(ent->move_type == MOVE_UNDERWATER)
            {
                Anim_SetAnimation(ss_anim, HARPOON_WATER_IDLE_TO_FIRING, 0);
            }
            else
            {
                inc_state = Anim_IncTime(ss_anim, (ent->character->state.weapon_ready && do_aim) ? (time) : (-time));
                TwoHand_IdleToFiring(ent, ss_anim, weapon, b_tag, target, do_aim, time, inc_state, HARPOON_LAND_IDLE_TO_HIDE, HARPOON_LAND_FIRING);
            }
            break;

        case HARPOON_LAND_HIDE_TO_IDLE:
            if(ent->move_type == MOVE_UNDERWATER)
            {
                TwoHand_HideToIdle(ent, ss_anim, weapon, time, HARPOON_WATER_IDLE_TO_FIRING);
            }
            else
            {
                TwoHand_HideToIdle(ent, ss_anim, weapon, time, HARPOON_LAND_IDLE_TO_FIRING);
            }
            break;

        case HARPOON_LAND_FIRING:
            if(checkCanShoot(weapon))
            {
                TwoHand_Firing(ent, ss_anim, b_tag, target, target_pos, weapon, time, HARPOON_LAND_FIRING, HARPOON_LAND_FIRING_TO_IDLE, HARPOON_LAND_IDLE_TO_FIRING);
            }
            else
            {
                AutoSelect(TR_MODEL_PISTOL, ss_anim, ent, time);
            }
            break;

        case HARPOON_LAND_IDLE_TO_HIDE:
            TwoHand_IdleToHide(ent, ss_anim, weapon, time, TW_FRAME_IS_END);
            break;

        case HARPOON_LAND_FIRING_TO_IDLE:
            TwoHand_FiringToIdle(ent, ss_anim, weapon, time, HARPOON_LAND_IDLE_TO_FIRING);
            break;

        case HARPOON_RELOAD:
            if(Anim_IncTime(ss_anim, time))
            {
                if(ent->move_type == MOVE_UNDERWATER)
                {
                    Anim_SetAnimation(ss_anim, HARPOON_WATER_IDLE_TO_FIRING, 0);
                }
                else
                {
                    Anim_SetAnimation(ss_anim, HARPOON_LAND_IDLE_TO_FIRING, 0);
                }
            }
            break;

        case HARPOON_WATER_IDLE_TO_FIRING:
            inc_state = Anim_IncTime(ss_anim, (ent->character->state.weapon_ready && do_aim) ? (time) : (-time));
            TwoHand_IdleToFiring(ent, ss_anim, weapon, b_tag, target, do_aim, time, inc_state, HARPOON_WATER_IDLE_TO_HIDE, HARPOON_WATER_FIRING);
            break;

        case HARPOON_WATER_FIRING_TO_IDLE:
            TwoHand_FiringToIdle(ent, ss_anim, weapon, time, HARPOON_WATER_IDLE_TO_FIRING);
            break;

        case HARPOON_WATER_FIRING:
            if(checkCanShoot(weapon))
            {
                TwoHand_Firing(ent, ss_anim, b_tag, target, target_pos, weapon, time, HARPOON_WATER_FIRING, HARPOON_WATER_FIRING_TO_IDLE, HARPOON_WATER_IDLE_TO_FIRING);
            }
            else
            {
                AutoSelect(TR_MODEL_PISTOL, ss_anim, ent, time);
            }
            break;

        case HARPOON_WATER_IDLE_TO_HIDE:
            TwoHand_IdleToHide(ent, ss_anim, weapon, time, TW_FRAME_IS_END);
            break;
    }
}

void MP5Anim(struct entity_s *ent, struct ss_animation_s *ss_anim, float time, struct weapons_s *weapon, struct ss_bone_tag_s *b_tag, struct entity_s *target, float *target_pos, int inc_state, bool do_aim)
{
    switch (ss_anim->current_animation)
    {
        case MP5_IDLE_TO_FIRING:
            inc_state = Anim_IncTime(ss_anim, (ent->character->state.weapon_ready && do_aim) ? (time) : (-time));
            TwoHand_IdleToFiring(ent, ss_anim, weapon, b_tag, target, do_aim, time, inc_state, MP5_IDLE_TO_HIDE, MP5_FIRING);
            break;

        case MP5_HIDE_TO_IDLE:
            TwoHand_HideToIdle(ent, ss_anim, weapon, time, MP5_IDLE_TO_FIRING);
            break;

        case MP5_FIRING:
            if(checkCanShoot(weapon))
            {
                TwoHand_Firing(ent, ss_anim, b_tag, target, target_pos, weapon, time, MP5_FIRING, MP5_FIRING_TO_IDLE, MP5_IDLE_TO_FIRING);
            }
            else
            {
                AutoSelect(TR_MODEL_PISTOL, ss_anim, ent, time);
            }
            break;

        case MP5_IDLE_TO_HIDE:
            TwoHand_IdleToHide(ent, ss_anim, weapon, time, TW_FRAME_IS_END);
            break;

        case MP5_FIRING_TO_IDLE:
            TwoHand_FiringToIdle(ent, ss_anim, weapon, time, MP5_IDLE_TO_FIRING);
            break;
    }
}

void M16Anim(struct entity_s *ent, struct ss_animation_s *ss_anim, float time, struct weapons_s *weapon, struct ss_bone_tag_s *b_tag, struct entity_s *target, float *target_pos, int inc_state, bool do_aim)
{
    switch (ss_anim->current_animation)
    {
        case M16_IDLE_TO_FIRING:
            inc_state = Anim_IncTime(ss_anim, (ent->character->state.weapon_ready && do_aim) ? (time) : (-time));
            TwoHand_IdleToFiring(ent, ss_anim, weapon, b_tag, target, do_aim, time, inc_state, M16_IDLE_TO_HIDE, M16_FIRING);
            break;

        case M16_HIDE_TO_IDLE:
            TwoHand_HideToIdle(ent, ss_anim, weapon, time, M16_IDLE_TO_FIRING);
            break;

        case M16_FIRING:
            if(checkCanShoot(weapon))
            {
                TwoHand_Firing(ent, ss_anim, b_tag, target, target_pos, weapon, time, M16_FIRING, M16_FIRING_TO_IDLE, M16_IDLE_TO_FIRING);
            }
            else
            {
                AutoSelect(TR_MODEL_PISTOL, ss_anim, ent, time);
            }
            break;

        case M16_IDLE_TO_HIDE:
            TwoHand_IdleToHide(ent, ss_anim, weapon, time, TW_FRAME_IS_END);
            break;

        case M16_FIRING_TO_IDLE:
            TwoHand_FiringToIdle(ent, ss_anim, weapon, time, M16_IDLE_TO_FIRING);
            break;
    }
}

void RocketGunAnim(struct entity_s *ent, struct ss_animation_s *ss_anim, float time, struct weapons_s *weapon, struct ss_bone_tag_s *b_tag, struct entity_s *target, float *target_pos, int inc_state, bool do_aim)
{
    switch (ss_anim->current_animation)
    {
        case ROCKETGUN_IDLE_TO_FIRING:
            inc_state = Anim_IncTime(ss_anim, (ent->character->state.weapon_ready && do_aim) ? (time) : (-time));
            TwoHand_IdleToFiring(ent, ss_anim, weapon, b_tag, target, do_aim, time, inc_state, ROCKETGUN_IDLE_TO_HIDE, ROCKETGUN_FIRING);
            break;

        case ROCKETGUN_HIDE_TO_IDLE:
            TwoHand_HideToIdle(ent, ss_anim, weapon, time, ROCKETGUN_IDLE_TO_FIRING);
            break;

        case ROCKETGUN_FIRING:
            if(checkCanShoot(weapon))
            {
                TwoHand_Firing(ent, ss_anim, b_tag, target, target_pos, weapon, time, ROCKETGUN_FIRING, ROCKETGUN_FIRING_TO_IDLE, ROCKETGUN_IDLE_TO_FIRING);
            }
            else
            {
                AutoSelect(TR_MODEL_PISTOL, ss_anim, ent, time);
            }
            break;

        case ROCKETGUN_IDLE_TO_HIDE:
            TwoHand_IdleToHide(ent, ss_anim, weapon, time, TW_FRAME_IS_END);
            break;

        case ROCKETGUN_FIRING_TO_IDLE:
            TwoHand_FiringToIdle(ent, ss_anim, weapon, time, ROCKETGUN_IDLE_TO_FIRING);
            break;
    }
}

void CrossbowAnim(struct entity_s *ent, struct ss_animation_s *ss_anim, float time, struct weapons_s *weapon, struct ss_bone_tag_s *b_tag, struct entity_s *target, float *target_pos, int inc_state, bool do_aim)
{
    switch (ss_anim->current_animation)
    {
        case CROSSBOW_IDLE_TO_FIRING:
            inc_state = Anim_IncTime(ss_anim, (ent->character->state.weapon_ready && do_aim) ? (time) : (-time));
            TwoHand_IdleToFiring(ent, ss_anim, weapon, b_tag, target, do_aim, time, inc_state, CROSSBOW_IDLE_TO_HIDE, CROSSBOW_FIRING);
            break;

        case CROSSBOW_HIDE_TO_IDLE:
            TwoHand_HideToIdle(ent, ss_anim, weapon, time, CROSSBOW_IDLE_TO_FIRING);
            break;

        case CROSSBOW_FIRING:
            if(checkCanShoot(weapon))
            {
                TwoHand_Firing(ent, ss_anim, b_tag, target, target_pos, weapon, time, CROSSBOW_FIRING, CROSSBOW_FIRING_TO_IDLE, CROSSBOW_IDLE_TO_FIRING);
            }
            else
            {
                AutoSelect(TR_MODEL_PISTOL, ss_anim, ent, time);
            }
            break;
        case CROSSBOW_IDLE_TO_HIDE:
            TwoHand_IdleToHide(ent, ss_anim, weapon, time, TW_FRAME_IS_END);
            break;

        case CROSSBOW_FIRING_TO_IDLE:
            TwoHand_FiringToIdle(ent, ss_anim, weapon, time, CROSSBOW_IDLE_TO_FIRING);
            break;
    }
}

void GrapplinGunAnim(struct entity_s *ent, struct ss_animation_s *ss_anim, float time, struct weapons_s *weapon, struct ss_bone_tag_s *b_tag, struct entity_s *target, float *target_pos, int inc_state, bool do_aim)
{
    switch (ss_anim->current_animation)
    {
        case GRAPPLING_IDLE_TO_FIRING:
            inc_state = Anim_IncTime(ss_anim, (ent->character->state.weapon_ready && do_aim) ? (time) : (-time));
            TwoHand_IdleToFiring(ent, ss_anim, weapon, b_tag, target, do_aim, time, inc_state, GRAPPLING_IDLE_TO_HIDE, GRAPPLING_FIRING);
            break;

        case GRAPPLING_HIDE_TO_IDLE:
            TwoHand_HideToIdle(ent, ss_anim, weapon, time, GRAPPLING_IDLE_TO_FIRING);
            break;

        case GRAPPLING_FIRING:
            if(checkCanShoot(weapon))
            {
                TwoHand_Firing(ent, ss_anim, b_tag, target, target_pos, weapon, time, GRAPPLING_FIRING, GRAPPLING_FIRING_TO_IDLE, GRAPPLING_IDLE_TO_FIRING);
            }
            else
            {
                AutoSelect(TR_MODEL_PISTOL, ss_anim, ent, time);
            }
            break;

        case GRAPPLING_IDLE_TO_HIDE:
            TwoHand_IdleToHide(ent, ss_anim, weapon, time, TW_FRAME_IS_END);
            break;

        case GRAPPLING_FIRING_TO_IDLE:
            TwoHand_FiringToIdle(ent, ss_anim, weapon, time, GRAPPLING_IDLE_TO_FIRING);
            break;
    }
}

void OneHand_IdleToFiring(struct entity_s *ent, struct ss_animation_s *ss_anim, struct weapons_s *weapon, struct ss_bone_tag_s *b_tag, struct entity_s *target, bool do_aim, float time)
{
    int inc_state = Anim_IncTime(ss_anim, (ent->character->state.weapon_ready && do_aim) ? (time) : (-time));
    int32_t ver = World_GetVersion();
    if((inc_state == ARMED) && ent->character->state.weapon_ready && ent->character->cmd.action)
    {
        ///@FIXME: animation launch one time and do nothing (sound, ammo consume, shoot)
        Anim_SetAnimation(ss_anim, ANIM_FIRING, 0);
    }
    else if(ent->character->state.weapon_ready && !ent->character->cmd.action && (-time))
    {
        if(ss_anim->current_frame == 3)
        {
            Audio_Kill(weapon->shot, TR_AUDIO_EMITTER_ENTITY, ent->id);

            if(ss_anim->anim_frame_flags & ANIM_FRAME_REVERSE)
            {
                if(IS_TR_II(ver) && (ss_anim->model->id == TR2_MODEL_UZI))
                {
                    Audio_Send(weapon->echo, TR_AUDIO_EMITTER_ENTITY, ent->id);
                }
                else if(IS_TR_III(ver) && (ss_anim->model->id == TR3_MODEL_UZI))
                {
                    Audio_Send(weapon->echo, TR_AUDIO_EMITTER_ENTITY, ent->id);
                }
                else if((ver >= TR_IV) && (ss_anim->model->id == TR4C_MODEL_UZI))
                {
                    Audio_Send(weapon->echo, TR_AUDIO_EMITTER_ENTITY, ent->id);
                }
            }
        }
    }
    else if((inc_state == UNARMED) && !ent->character->state.weapon_ready)
    {
        // fix exit fire loop bug (after pressing 
        Audio_Kill(weapon->shot, TR_AUDIO_EMITTER_ENTITY, ent->id);
        Anim_SetAnimation(ss_anim, ANIM_IDLE_AND_HIDE, -1);
    }
}

void TwoHand_IdleToFiring(struct entity_s *ent, struct ss_animation_s *ss_anim, struct weapons_s *weapon, struct ss_bone_tag_s *b_tag, struct entity_s *target, bool do_aim, float time, int inc_state, int anim_hide, int anim_firing)
{
    b_tag->is_targeted = (target) ? (0x01) : (0x00);

    if((inc_state == ARMED) && ent->character->state.weapon_ready && ent->character->cmd.action)
    {
        Anim_SetAnimation(ss_anim, anim_firing, 0);
    }
    else if((inc_state == UNARMED) && !ent->character->state.weapon_ready)
    {
        // fix exit fire loop bug
        Audio_Kill(weapon->shot, TR_AUDIO_EMITTER_ENTITY, ent->id);
        Anim_SetAnimation(ss_anim, anim_hide, 0);
    }
}

void TwoHand_FiringToIdle(struct entity_s *ent, struct ss_animation_s *ss_anim, struct weapons_s *weapon, float time, int anim_idle_to_firing)
{
    int32_t ver = World_GetVersion();
    if(ss_anim->current_frame == 1)
    {
        Audio_Kill(weapon->shot, TR_AUDIO_EMITTER_ENTITY, ent->id);
    }
    else if(ss_anim->current_frame == 3 && (ss_anim->anim_frame_flags == ANIM_FRAME_REVERSE))
    {
        if(IS_TR_II(ver) && ss_anim->model->id == TR2_MODEL_M16)
        {
            Audio_Send(weapon->echo, TR_AUDIO_EMITTER_ENTITY, ent->id);
        }
    }
    
    if(Anim_IncTime(ss_anim, time))
    {
        Anim_SetAnimation(ss_anim, anim_idle_to_firing, 0);
    }
}

void TwoHand_IdleToHide(struct entity_s *ent, struct ss_animation_s *ss_anim, struct weapons_s *weapon, float time, int frame_to_hide)
{
    if(Anim_IncTime(ss_anim, time))
    {
        SSBoneFrame_DisableOverrideAnim(ent->bf, ss_anim);

        // fix animation of two hand if weapon have no ammo (AutoSelect not work with Two Hand after Hide)
        // when animation is finished and if weapon not have ammo, change weapon to model_id
        if(Inventory_GetItemsCount(ent->inventory, weapon->current_ammo) <= 0)
        {
            Character_ChangeWeapon(ent, TR_MODEL_PISTOL);
        }
    }

    if((ss_anim->frame_changing_state >= 0x01) && (ss_anim->prev_frame == frame_to_hide))
    {
        int32_t ver = World_GetVersion();
        StateControl_SetWeaponMeshOn(ent->bf, ss_anim->model, IS_TR_I(ver) ? TR1_BACK_WEAPON : TR2_BACK_WEAPON);
        StateControl_SetWeaponMeshOff(ent->bf, HAND_RIGHT);
        Audio_Send(weapon->hide, TR_AUDIO_EMITTER_ENTITY, ent->id);
    }
}

void OneHand_HideToIdle(struct entity_s *ent, struct ss_animation_s *ss_anim, struct weapons_s *weapon, float time)
{
    int inc_state = Anim_IncTime(ss_anim, (ent->character->state.weapon_ready) ? (time) : (-time));

    if((inc_state == ARMED) && ent->character->state.weapon_ready)
    {
        StateControl_SetWeaponMeshOn(ent->bf, ss_anim->model, HAND_RIGHT);
        StateControl_SetWeaponMeshOn(ent->bf, ss_anim->model, HAND_LEFT);
        StateControl_SetWeaponMeshOff(ent->bf, HOLSTER_RIGHT);
        StateControl_SetWeaponMeshOff(ent->bf, HOLSTER_LEFT);
        Anim_SetAnimation(ss_anim, ANIM_IDLE_AND_HIDE, 0);
        Audio_Send(weapon->draw, TR_AUDIO_EMITTER_ENTITY, ent->id);
    }
    else if((inc_state == UNARMED) && !ent->character->state.weapon_ready)
    {
        ent->character->state.weapon_ready = 0;
        for(ss_animation_p anim = ent->bf->animations.next; anim; anim = anim->next)
        {
            if((anim->type == ANIM_TYPE_WEAPON_LH) || (anim->type == ANIM_TYPE_WEAPON_RH))
            {
                SSBoneFrame_DisableOverrideAnim(ent->bf, anim);
            }
        }
    }
}

void TwoHand_HideToIdle(struct entity_s *ent, struct ss_animation_s *ss_anim, struct weapons_s *weapon, float time, int anim_idle)
{
    if(Anim_IncTime(ss_anim, time))
    {
        Anim_SetAnimation(ss_anim, anim_idle, 0);  // to idle
    }

    if((ss_anim->frame_changing_state >= 0x01) && (ss_anim->prev_frame == TW_FRAME))
    {
        int32_t ver = World_GetVersion();
        StateControl_SetWeaponMeshOn(ent->bf, ss_anim->model, HAND_RIGHT);
        if(IS_TR_I(ver))
        {
            StateControl_SetWeaponMeshOff(ent->bf, TR1_BACK_WEAPON);
        }
        else
        {
            StateControl_SetWeaponMeshOff(ent->bf, TR2_BACK_WEAPON);
        }
        Audio_Send(weapon->draw, TR_AUDIO_EMITTER_ENTITY, ent->id);
    }
}

void OneHand_HideAndIdle(struct entity_s *ent, struct ss_animation_s *ss_anim, struct weapons_s *weapon, float time)
{
    int inc_state = Anim_IncTime(ss_anim, (ent->character->state.weapon_ready) ? (time) : (-time));
    ent->bf->bone_tags[10].is_targeted = 0x00;
    ent->bf->bone_tags[13].is_targeted = 0x00;
    if((inc_state == ARMED) && ent->character->state.weapon_ready)
    {
        Anim_SetAnimation(ss_anim, ANIM_IDLE_TO_FIRING, 0);
    }
    else if((inc_state == UNARMED) && !ent->character->state.weapon_ready)
    {
        StateControl_SetWeaponMeshOn(ent->bf, ss_anim->model, 1);
        StateControl_SetWeaponMeshOn(ent->bf, ss_anim->model, 4);
        StateControl_SetWeaponMeshOff(ent->bf, 10);
        StateControl_SetWeaponMeshOff(ent->bf, 13);
        Anim_SetAnimation(ss_anim, ANIM_HIDE_TO_IDLE, -1);
        Audio_Send(weapon->hide, TR_AUDIO_EMITTER_ENTITY, ent->id);
    }
}

void TwoHand_Firing(struct entity_s *ent, struct ss_animation_s *ss_anim, struct ss_bone_tag_s *b_tag, struct entity_s *target, float *target_pos, struct weapons_s *weapon, float time, int anim_firing, int anim_firing_to_idle, int anim_idle)
{
    if((ss_anim->frame_changing_state >= 4) | Anim_IncTime(ss_anim, time * weapon->firerate))
    {
        if(ent->character->state.weapon_ready && ent->character->cmd.action)
        {
            collision_result_t cs;
            float from[3], to[3], tr[16], dir[3], t;
            ss_bone_tag_p bt = ent->bf->bone_tags + ent->character->bone_r_hand_end;

            Anim_SetAnimation(ss_anim, anim_firing, 0);
            ss_anim->frame_changing_state = 0x01;
            Audio_Send(weapon->shot, TR_AUDIO_EMITTER_ENTITY, ent->id);
            consumeAmmo(weapon);

            Mat4_Mat4_mul(tr, ent->transform.M4x4, bt->current_transform);
            vec3_copy(from, tr + 12);

            if(target && (bt->mod.current_slerp) > 0.99f)
            {
                vec3_sub(dir, target_pos, from);
                vec3_norm(dir, t);
            }
            else
            {
                vec3_copy_inv(dir, tr + 8);
            }

            // adding range
            vec3_add_mul(to, from, dir, weapon->range);

            for (int i = 1; i <= weapon->bullet; ++i)
            {
                //vec3_copy(d_from, from);
                //vec3_copy(d_to, to);
                t = (weapon->range * i) / weapon->bullet;
                vec3_add_mul(to, from, dir, t);
                t = 8.0f * i;

                switch (i % 4)
                {
                    case 0: vec3_add_mul(to, to, tr + 0, t); break;
                    case 1: vec3_add_mul(to, to, tr + 4, t); break;
                    case 2: vec3_add_mul(to, to, tr + 0, -t); break;
                    case 3: vec3_add_mul(to, to, tr + 4, -t); break;
                }

                if(Physics_RayTest(&cs, from, to, ent->self, COLLISION_FILTER_CHARACTER) && cs.obj && (cs.obj->object_type == OBJECT_ENTITY))
                {
                    target = (entity_p)cs.obj->object;
                    Script_ExecEntity(engine_lua, ENTITY_CALLBACK_SHOOT, ent->id, target->id);
                }
            }
        }
        else if(target)
        {
            // if you have a target -> lock the enemie
            Anim_SetAnimation(ss_anim, anim_idle, -1);
        }
        else
        {
            // to idle animation
            Anim_SetAnimation(ss_anim, anim_firing_to_idle, 0);
        }
    }

    if((ss_anim->frame_changing_state == 0x01) && (weapon->reload_1))
    {
        if(ss_anim->prev_frame == 2)
        {
            Audio_Send(weapon->reload_1, TR_AUDIO_EMITTER_ENTITY, ent->id);
        }
    }
}

void OneHand_Firing(struct entity_s *ent, struct ss_animation_s *ss_anim, struct ss_bone_tag_s *b_tag, struct entity_s *target, float *target_pos, struct weapons_s *weapon, float time, uint16_t targeted_bone_start, uint16_t targeted_bone_end, int anim_firing, int anim_firing_to_idle)
{
    if((ss_anim->frame_changing_state >= 4) | Anim_IncTime(ss_anim, time * weapon->firerate))
    {
        if(ent->character->state.weapon_ready && ent->character->cmd.action)
        {
            collision_result_t cs;
            float from[3], to[3], tr[16];
            ss_bone_tag_p bt = ent->bf->bone_tags + targeted_bone_start;

            Anim_SetAnimation(ss_anim, anim_firing, 0);
            ss_anim->frame_changing_state = 0x01;
            Audio_Send(weapon->shot, TR_AUDIO_EMITTER_ENTITY, ent->id);
            consumeAmmo(weapon);

            Mat4_Mat4_mul(tr, ent->transform.M4x4, bt->current_transform);
            Mat4_vec3_mul(from, ent->transform.M4x4, ent->bf->bone_tags[targeted_bone_end].current_transform + 12);

            if(target && (bt->mod.current_slerp > 0.99))
            {
                vec3_copy(to, bt->mod.target_pos);
            }
            else
            {
                vec3_add_mul(to, from, tr + 8, -32768.0f);
            }

            if(Physics_RayTest(&cs, from, to, ent->self, COLLISION_FILTER_CHARACTER) && cs.obj && (cs.obj->object_type == OBJECT_ENTITY))
            {
                target = (entity_p)cs.obj->object;
                Script_ExecEntity(engine_lua, ENTITY_CALLBACK_SHOOT, ent->id, target->id);
            }
        }
        else
        {
            Anim_SetAnimation(ss_anim, anim_firing_to_idle, -1);
        }
    }
}
