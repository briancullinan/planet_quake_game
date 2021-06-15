#include <cassert>
#include <sstream>

#include "shader_manager.h"

shader_manager::shader_manager()
{
    //Color mult prog
    static_mesh_shader = new unlit_tinted_shader_description(shader_stage(GL_VERTEX_SHADER_ARB, "shaders/static_mesh.vsh"), shader_stage(GL_FRAGMENT_SHADER_ARB, "shaders/static_mesh.fsh"));

    //Room prog
    shader_stage roomFragmentShader(GL_FRAGMENT_SHADER_ARB, "shaders/room.fsh");
    for (int isWater = 0; isWater < 2; isWater++)
    {
        for (int isFlicker = 0; isFlicker < 2; isFlicker++)
        {
            std::ostringstream stream;
            stream << "#define IS_WATER " << isWater << std::endl;
            stream << "#define IS_FLICKER " << isFlicker << std::endl;

            room_shaders[isWater][isFlicker] = new unlit_tinted_shader_description(shader_stage(GL_VERTEX_SHADER_ARB, "shaders/room.vsh", stream.str().c_str()), roomFragmentShader);
        }
    }

    // Entity prog
    shader_stage entityVertexShader(GL_VERTEX_SHADER_ARB, "shaders/entity.vsh");
    for (int i = 0; i <= MAX_NUM_LIGHTS; i++) {
        std::ostringstream stream;
        stream << "#define NUMBER_OF_LIGHTS " << i << std::endl;

        entity_shader[i] = new lit_shader_description(entityVertexShader, shader_stage(GL_FRAGMENT_SHADER_ARB, "shaders/entity.fsh", stream.str().c_str()));
    }

    text = new text_shader_description(shader_stage(GL_VERTEX_SHADER_ARB, "shaders/text.vsh"), shader_stage(GL_FRAGMENT_SHADER_ARB, "shaders/text.fsh"));
}

shader_manager::~shader_manager()
{
    // Do nothing. All shaders are released by OpenGL anyway.
}

const lit_shader_description *shader_manager::getEntityShader(unsigned numberOfLights) const {
    assert(numberOfLights <= MAX_NUM_LIGHTS);

    return entity_shader[numberOfLights];
}

const unlit_tinted_shader_description *shader_manager::getRoomShader(bool isFlickering, bool isWater) const
{
    return room_shaders[isWater ? 1 : 0][isFlickering ? 1 : 0];
}
