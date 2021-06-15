function tr1_raptor_init(id)
    basic_init(id);
    
    setEntityAnim(id, ANIM_TYPE_BASE, 0, 0);
    setEntityAnimState(id, ANIM_TYPE_BASE, 1);
    setCharacterStateControlFunctions(id, STATE_FUNCTIONS_RAPTOR);
    setCharacterKeyAnim(id, ANIM_TYPE_BASE, ANIMATION_KEY_INIT);
    setCharacterAIParams(id, 0xFF, ZONE_TYPE_1);
    setCharacterBones(id, 22, 0, 0, 0, 0, 0);  --head, torso, l_hand_first, l_hand_last, r_hand_first, r_hand_last
    setCharacterMoveSizes(id, 512.0, 256.0, 256.0, 256, 256);
    
    setCharacterParam(id, PARAM_HEALTH, TR1_RAPTOR, TR1_RAPTOR);
    setEntityGhostCollisionShape(id,  21,  COLLISION_SHAPE_BOX, nil, nil, nil, nil, nil, nil);
    setEntityGhostCollisionShape(id,  22,  COLLISION_SHAPE_BOX, nil, nil, nil, nil, nil, nil);
    setEntityGhostCollisionShape(id,  24,  COLLISION_SHAPE_BOX, nil, nil, nil, nil, nil, nil);
    
    if(getEntityTypeFlag(id, ENTITY_TYPE_SPAWNED) ~= 0) then
        entity_funcs[id].onSave = function()
            return "tr1_raptor_init(" .. id .. ");\n";
        end;
    end;
    
    entity_funcs[id].onAttack = function(object_id, activator_id)
        local bone = entity_funcs[object_id].col_part_from;
        if(bone >= 9 and bone <= 23) then
            setCharacterParam(object_id, PARAM_HIT_DAMAGE, TR1_RAPTOR_DMG * frame_time, TR1_RAPTOR_DMG * frame_time);
            if(entity_funcs[activator_id].onHit ~= nil) then
                entity_funcs[activator_id].onHit(activator_id, object_id);
            end;
        end;
    end;
    
    entity_funcs[id].onHit = function(object_id, activator_id)
        local damage = getCharacterParam(activator_id, PARAM_HIT_DAMAGE);
        changeCharacterParam(object_id, PARAM_HEALTH, -damage);
        if(getCharacterParam(object_id, PARAM_HEALTH) == 0) then
            setEntityCollision(object_id, false);
            setCharacterTarget(activator_id, nil);
        end;
    end;
    
    entity_funcs[id].onLoop = nil;
end;