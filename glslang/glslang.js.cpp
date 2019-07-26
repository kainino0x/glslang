// Copyright 2019 The Shaderc Authors. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <cstdio>
#include <cstdint>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif  // __EMSCRIPTEN__
#include <memory>

#include "../SPIRV/GlslangToSpv.h"
#include "../SPIRV/doc.h"
#include "./../glslang/Public/ShaderLang.h"

#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif

const TBuiltInResource DefaultTBuiltInResource = {
    /* .MaxLights = */ 32,
    /* .MaxClipPlanes = */ 6,
    /* .MaxTextureUnits = */ 32,
    /* .MaxTextureCoords = */ 32,
    /* .MaxVertexAttribs = */ 64,
    /* .MaxVertexUniformComponents = */ 4096,
    /* .MaxVaryingFloats = */ 64,
    /* .MaxVertexTextureImageUnits = */ 32,
    /* .MaxCombinedTextureImageUnits = */ 80,
    /* .MaxTextureImageUnits = */ 32,
    /* .MaxFragmentUniformComponents = */ 4096,
    /* .MaxDrawBuffers = */ 32,
    /* .MaxVertexUniformVectors = */ 128,
    /* .MaxVaryingVectors = */ 8,
    /* .MaxFragmentUniformVectors = */ 16,
    /* .MaxVertexOutputVectors = */ 16,
    /* .MaxFragmentInputVectors = */ 15,
    /* .MinProgramTexelOffset = */ -8,
    /* .MaxProgramTexelOffset = */ 7,
    /* .MaxClipDistances = */ 8,
    /* .MaxComputeWorkGroupCountX = */ 65535,
    /* .MaxComputeWorkGroupCountY = */ 65535,
    /* .MaxComputeWorkGroupCountZ = */ 65535,
    /* .MaxComputeWorkGroupSizeX = */ 1024,
    /* .MaxComputeWorkGroupSizeY = */ 1024,
    /* .MaxComputeWorkGroupSizeZ = */ 64,
    /* .MaxComputeUniformComponents = */ 1024,
    /* .MaxComputeTextureImageUnits = */ 16,
    /* .MaxComputeImageUniforms = */ 8,
    /* .MaxComputeAtomicCounters = */ 8,
    /* .MaxComputeAtomicCounterBuffers = */ 1,
    /* .MaxVaryingComponents = */ 60,
    /* .MaxVertexOutputComponents = */ 64,
    /* .MaxGeometryInputComponents = */ 64,
    /* .MaxGeometryOutputComponents = */ 128,
    /* .MaxFragmentInputComponents = */ 128,
    /* .MaxImageUnits = */ 8,
    /* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
    /* .MaxCombinedShaderOutputResources = */ 8,
    /* .MaxImageSamples = */ 0,
    /* .MaxVertexImageUniforms = */ 0,
    /* .MaxTessControlImageUniforms = */ 0,
    /* .MaxTessEvaluationImageUniforms = */ 0,
    /* .MaxGeometryImageUniforms = */ 0,
    /* .MaxFragmentImageUniforms = */ 8,
    /* .MaxCombinedImageUniforms = */ 8,
    /* .MaxGeometryTextureImageUnits = */ 16,
    /* .MaxGeometryOutputVertices = */ 256,
    /* .MaxGeometryTotalOutputComponents = */ 1024,
    /* .MaxGeometryUniformComponents = */ 1024,
    /* .MaxGeometryVaryingComponents = */ 64,
    /* .MaxTessControlInputComponents = */ 128,
    /* .MaxTessControlOutputComponents = */ 128,
    /* .MaxTessControlTextureImageUnits = */ 16,
    /* .MaxTessControlUniformComponents = */ 1024,
    /* .MaxTessControlTotalOutputComponents = */ 4096,
    /* .MaxTessEvaluationInputComponents = */ 128,
    /* .MaxTessEvaluationOutputComponents = */ 128,
    /* .MaxTessEvaluationTextureImageUnits = */ 16,
    /* .MaxTessEvaluationUniformComponents = */ 1024,
    /* .MaxTessPatchComponents = */ 120,
    /* .MaxPatchVertices = */ 32,
    /* .MaxTessGenLevel = */ 64,
    /* .MaxViewports = */ 16,
    /* .MaxVertexAtomicCounters = */ 0,
    /* .MaxTessControlAtomicCounters = */ 0,
    /* .MaxTessEvaluationAtomicCounters = */ 0,
    /* .MaxGeometryAtomicCounters = */ 0,
    /* .MaxFragmentAtomicCounters = */ 8,
    /* .MaxCombinedAtomicCounters = */ 8,
    /* .MaxAtomicCounterBindings = */ 1,
    /* .MaxVertexAtomicCounterBuffers = */ 0,
    /* .MaxTessControlAtomicCounterBuffers = */ 0,
    /* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
    /* .MaxGeometryAtomicCounterBuffers = */ 0,
    /* .MaxFragmentAtomicCounterBuffers = */ 1,
    /* .MaxCombinedAtomicCounterBuffers = */ 1,
    /* .MaxAtomicCounterBufferSize = */ 16384,
    /* .MaxTransformFeedbackBuffers = */ 4,
    /* .MaxTransformFeedbackInterleavedComponents = */ 64,
    /* .MaxCullDistances = */ 8,
    /* .MaxCombinedClipAndCullDistances = */ 8,
    /* .MaxSamples = */ 4,
    /* .maxMeshOutputVerticesNV = */ 256,
    /* .maxMeshOutputPrimitivesNV = */ 512,
    /* .maxMeshWorkGroupSizeX_NV = */ 32,
    /* .maxMeshWorkGroupSizeY_NV = */ 1,
    /* .maxMeshWorkGroupSizeZ_NV = */ 1,
    /* .maxTaskWorkGroupSizeX_NV = */ 32,
    /* .maxTaskWorkGroupSizeY_NV = */ 1,
    /* .maxTaskWorkGroupSizeZ_NV = */ 1,
    /* .maxMeshViewCountNV = */ 4,

    /* .limits = */ {
        /* .nonInductiveForLoops = */ 1,
        /* .whileLoops = */ 1,
        /* .doWhileLoops = */ 1,
        /* .generalUniformIndexing = */ 1,
        /* .generalAttributeMatrixVectorIndexing = */ 1,
        /* .generalVaryingIndexing = */ 1,
        /* .generalSamplerIndexing = */ 1,
        /* .generalVariableIndexing = */ 1,
        /* .generalConstantMatrixVectorIndexing = */ 1,
    }};

extern "C" {

/*
 * Takes in a GLSL shader as a string and converts it to SPIR-V in binary form.
 *
 * |glsl|          Char array created using create_input_buffer and populated
 *                 with the shader to be converted.
 *                 This buffer must be destroyed using destroy_input_buffer.
 * |shader_type| - Magic number indicating the type of shader being processed.
 *                 Legal values are as follows:
 *                   Vertex = 0
 *                   Fragment = 4
 *                   Compute = 5
 * |spirv|         Pointer to an output buffer that will be updated with the
 *                 resulting SPIR-V shader.
 *                 This buffer must be destroyed using destroy_output_buffer.
 *
 * |spirv_len|     Length of the output binary buffer.
 * |gen_debug|     Flag to indicate if debug information should be generated.
 *
 * Return 0 on success, non-0 on failure.
 */
EMSCRIPTEN_KEEPALIVE
int convert_glsl_to_spirv(const char* glsl, int shader_type, uint32_t** spirv, size_t* spirv_len, bool gen_debug)
{
    int ret_val = 0;
    if (glsl == nullptr || spirv == nullptr) {
        return 1;
    }
    *spirv = nullptr;

    if (shader_type != 0 && shader_type != 4 && shader_type != 5) {
        return 2;
    }

    EShLanguage shader_stage = static_cast<EShLanguage>(shader_type);

    glslang::InitializeProcess();
    {
        glslang::TShader shader(shader_stage);
        shader.setStrings(&glsl, 1);
        shader.setEnvInput(glslang::EShSourceGlsl, shader_stage, glslang::EShClientVulkan, 100);
        shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_1);
        shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_3);
        if (!shader.parse(&DefaultTBuiltInResource, 100, true, EShMsgDefault)) {
            fprintf(stderr, "Parse failed\n");
            fprintf(stderr, "%s\n", shader.getInfoLog());
            fprintf(stderr, "%s\n", shader.getInfoDebugLog());
        }

        glslang::TProgram program;
        program.addShader(&shader);
        if (!program.link(EShMsgDefault)) {
            fprintf(stderr, "Link failed\n");
            fprintf(stderr, "%s\n", program.getInfoLog());
            fprintf(stderr, "%s\n", program.getInfoDebugLog());
        }

        std::vector<uint32_t> output;
        glslang::SpvOptions spvOptions;
        spvOptions.generateDebugInfo = gen_debug;
        spvOptions.disableOptimizer = false;
        spvOptions.optimizeSize = false;
        spvOptions.disassemble = false;
        spvOptions.validate = false;

        glslang::GlslangToSpv(*program.getIntermediate(shader_stage), output, nullptr, &spvOptions);

        *spirv_len = output.size();
        *spirv = static_cast<uint32_t*>(malloc(*spirv_len * sizeof(uint32_t)));
        if (*spirv != nullptr) {
            memcpy(*spirv, output.data(), *spirv_len * sizeof(uint32_t));
        } else {
            ret_val = 3;
        }
    }
    glslang::FinalizeProcess();
    return ret_val;
}

/*
 * Created a valid input buffer.
 *
 * Must be destroyed later using destroy_input_buffer.
 */
EMSCRIPTEN_KEEPALIVE
char* create_input_buffer(int count) { return static_cast<char*>(malloc(count * sizeof(char))); }

/*
 * Destroys a buffer created by create_input_buffer
 */
EMSCRIPTEN_KEEPALIVE
void destroy_input_buffer(char* p)
{
    if (p != nullptr)
        free(p);
}

/*
 * Destroys a buffer created by convert_glsl_to_spirv
 */
EMSCRIPTEN_KEEPALIVE
void destroy_output_buffer(uint32_t* p)
{
    if (p != nullptr)
        free(p);
}

}

#ifndef __EMSCRIPTEN__
int main() {
    const char* input_text = R"(#version 330

void main()
{
    gl_FragColor = vec4(0.4, 0.4, 0.8, 1.0);
})";

    char* input;
    uint32_t* output;
    size_t output_len;

    input = create_input_buffer(sizeof(input_text));
    assert(input != nullptr);
    memcpy(input, input_text, sizeof(input_text));

    convert_glsl_to_spirv(input, 4, &output, &output_len, false);
    destroy_output_buffer(output);
    destroy_input_buffer(input);
    return 0;
}
#endif  // !__EMSCRIPTEN__
