#pragma once

#include <vector>

#include "VulkanRegistrar.h"


namespace aa
{


    // wrapper over VKShaderModule
    class VKShader
    {

    public:
        VKShader(const std::vector <char>& spirvCode);
        ~VKShader();

        const VkShaderModule vkShaderModule;

        //  delete all implicit constructors 
        VKShader(const VKShader&)   = delete;
        VKShader(VKShader&&)        = delete;

        VKShader& operator = (const VKShader&)  = delete;
        VKShader& operator = (VKShader&&)       = delete;


    private:
        VkShaderModule createShaderModule(const std::vector <char>& spirvCode);

    };

}
