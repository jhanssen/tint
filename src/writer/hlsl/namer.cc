// Copyright 2020 The Tint Authors.
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

#include "src/writer/hlsl/namer.h"

#include <algorithm>

namespace tint {
namespace writer {
namespace hlsl {
namespace {

// This list is used for a binary search and must be kept in sorted order.
const char* kNames[] = {"AddressU",
                        "AddressV",
                        "AddressW",
                        "AllMemoryBarrier",
                        "AllMemoryBarrierWithGroupSync",
                        "AppendStructuredBuffer",
                        "BINORMAL",
                        "BLENDINDICES",
                        "BLENDWEIGHT",
                        "BlendState",
                        "BorderColor",
                        "Buffer",
                        "ByteAddressBuffer",
                        "COLOR",
                        "CheckAccessFullyMapped",
                        "ComparisonFunc",
                        "CompileShader",
                        "ComputeShader",
                        "ConsumeStructuredBuffer",
                        "D3DCOLORtoUBYTE4",
                        "DEPTH",
                        "DepthStencilState",
                        "DepthStencilView",
                        "DeviceMemoryBarrier",
                        "DeviceMemroyBarrierWithGroupSync",
                        "DomainShader",
                        "EvaluateAttributeAtCentroid",
                        "EvaluateAttributeAtSample",
                        "EvaluateAttributeSnapped",
                        "FOG",
                        "Filter",
                        "GeometryShader",
                        "GetRenderTargetSampleCount",
                        "GetRenderTargetSamplePosition",
                        "GroupMemoryBarrier",
                        "GroupMemroyBarrierWithGroupSync",
                        "Hullshader",
                        "InputPatch",
                        "InterlockedAdd",
                        "InterlockedAnd",
                        "InterlockedCompareExchange",
                        "InterlockedCompareStore",
                        "InterlockedExchange",
                        "InterlockedMax",
                        "InterlockedMin",
                        "InterlockedOr",
                        "InterlockedXor",
                        "LineStream",
                        "MaxAnisotropy",
                        "MaxLOD",
                        "MinLOD",
                        "MipLODBias",
                        "NORMAL",
                        "NULL",
                        "Normal",
                        "OutputPatch",
                        "POSITION",
                        "POSITIONT",
                        "PSIZE",
                        "PixelShader",
                        "PointStream",
                        "Process2DQuadTessFactorsAvg",
                        "Process2DQuadTessFactorsMax",
                        "Process2DQuadTessFactorsMin",
                        "ProcessIsolineTessFactors",
                        "ProcessQuadTessFactorsAvg",
                        "ProcessQuadTessFactorsMax",
                        "ProcessQuadTessFactorsMin",
                        "ProcessTriTessFactorsAvg",
                        "ProcessTriTessFactorsMax",
                        "ProcessTriTessFactorsMin",
                        "RWBuffer",
                        "RWByteAddressBuffer",
                        "RWStructuredBuffer",
                        "RWTexture1D",
                        "RWTexture1DArray",
                        "RWTexture2D",
                        "RWTexture2DArray",
                        "RWTexture3D",
                        "RasterizerState",
                        "RenderTargetView",
                        "SV_ClipDistance",
                        "SV_Coverage",
                        "SV_CullDistance",
                        "SV_Depth",
                        "SV_DepthGreaterEqual",
                        "SV_DepthLessEqual",
                        "SV_DispatchThreadID",
                        "SV_DomainLocation",
                        "SV_GSInstanceID",
                        "SV_GroupID",
                        "SV_GroupIndex",
                        "SV_GroupThreadID",
                        "SV_InnerCoverage",
                        "SV_InsideTessFactor",
                        "SV_InstanceID",
                        "SV_IsFrontFace",
                        "SV_OutputControlPointID",
                        "SV_Position",
                        "SV_PrimitiveID",
                        "SV_RenderTargetArrayIndex",
                        "SV_SampleIndex",
                        "SV_StencilRef",
                        "SV_Target",
                        "SV_TessFactor",
                        "SV_VertexArrayIndex",
                        "SV_VertexID",
                        "Sampler",
                        "Sampler1D",
                        "Sampler2D",
                        "Sampler3D",
                        "SamplerCUBE",
                        "StructuredBuffer",
                        "TANGENT",
                        "TESSFACTOR",
                        "TEXCOORD",
                        "Texcoord",
                        "Texture",
                        "Texture1D",
                        "Texture1DArray",
                        "Texture2D",
                        "Texture2DArray",
                        "Texture2DMS",
                        "Texture2DMSArray",
                        "Texture3D",
                        "TextureCube",
                        "TextureCubeArray",
                        "TriangleStream",
                        "VFACE",
                        "VPOS",
                        "VertexShader",
                        "abort",
                        "abs",
                        "acos",
                        "all",
                        "allow_uav_condition",
                        "any",
                        "asdouble",
                        "asfloat",
                        "asin",
                        "asint",
                        "asm",
                        "asm_fragment",
                        "asuint",
                        "atan",
                        "atan2",
                        "auto",
                        "bool",
                        "bool1",
                        "bool1x1",
                        "bool1x2",
                        "bool1x3",
                        "bool1x4",
                        "bool2",
                        "bool2x1",
                        "bool2x2",
                        "bool2x3",
                        "bool2x4",
                        "bool3",
                        "bool3x1",
                        "bool3x2",
                        "bool3x3",
                        "bool3x4",
                        "bool4",
                        "bool4x1",
                        "bool4x2",
                        "bool4x3",
                        "bool4x4",
                        "branch",
                        "break",
                        "call",
                        "case",
                        "catch",
                        "cbuffer",
                        "ceil",
                        "centroid",
                        "char",
                        "clamp",
                        "class",
                        "clip",
                        "column_major",
                        "compile_fragment",
                        "const",
                        "const_cast",
                        "continue",
                        "cos",
                        "cosh",
                        "countbits",
                        "cross",
                        "ddx",
                        "ddx_coarse",
                        "ddx_fine",
                        "ddy",
                        "ddy_coarse",
                        "ddy_fine",
                        "degrees",
                        "delete",
                        "determinant",
                        "discard",
                        "distance",
                        "do",
                        "dot",
                        "double",
                        "double1",
                        "double1x1",
                        "double1x2",
                        "double1x3",
                        "double1x4",
                        "double2",
                        "double2x1",
                        "double2x2",
                        "double2x3",
                        "double2x4",
                        "double3",
                        "double3x1",
                        "double3x2",
                        "double3x3",
                        "double3x4",
                        "double4",
                        "double4x1",
                        "double4x2",
                        "double4x3",
                        "double4x4",
                        "dst",
                        "dword",
                        "dword1",
                        "dword1x1",
                        "dword1x2",
                        "dword1x3",
                        "dword1x4",
                        "dword2",
                        "dword2x1",
                        "dword2x2",
                        "dword2x3",
                        "dword2x4",
                        "dword3",
                        "dword3x1",
                        "dword3x2",
                        "dword3x3",
                        "dword3x4",
                        "dword4",
                        "dword4x1",
                        "dword4x2",
                        "dword4x3",
                        "dword4x4",
                        "dynamic_cast",
                        "else",
                        "enum",
                        "errorf",
                        "exp",
                        "exp2",
                        "explicit",
                        "export",
                        "extern",
                        "f16to32",
                        "f32tof16",
                        "faceforward",
                        "false",
                        "fastopt",
                        "firstbithigh",
                        "firstbitlow",
                        "flatten",
                        "float",
                        "float1",
                        "float1x1",
                        "float1x2",
                        "float1x3",
                        "float1x4",
                        "float2",
                        "float2x1",
                        "float2x2",
                        "float2x3",
                        "float2x4",
                        "float3",
                        "float3x1",
                        "float3x2",
                        "float3x3",
                        "float3x4",
                        "float4",
                        "float4x1",
                        "float4x2",
                        "float4x3",
                        "float4x4",
                        "floor",
                        "fma",
                        "fmod",
                        "for",
                        "forcecase",
                        "frac",
                        "frexp",
                        "friend",
                        "fwidth",
                        "fxgroup",
                        "goto",
                        "groupshared",
                        "half",
                        "half1",
                        "half1x1",
                        "half1x2",
                        "half1x3",
                        "half1x4",
                        "half2",
                        "half2x1",
                        "half2x2",
                        "half2x3",
                        "half2x4",
                        "half3",
                        "half3x1",
                        "half3x2",
                        "half3x3",
                        "half3x4",
                        "half4",
                        "half4x1",
                        "half4x2",
                        "half4x3",
                        "half4x4",
                        "if",
                        "in",
                        "inline",
                        "inout",
                        "int",
                        "int1",
                        "int1x1",
                        "int1x2",
                        "int1x3",
                        "int1x4",
                        "int2",
                        "int2x1",
                        "int2x2",
                        "int2x3",
                        "int2x4",
                        "int3",
                        "int3x1",
                        "int3x2",
                        "int3x3",
                        "int3x4",
                        "int4",
                        "int4x1",
                        "int4x2",
                        "int4x3",
                        "int4x4",
                        "interface",
                        "isfinite",
                        "isinf",
                        "isnan",
                        "ldexp",
                        "length",
                        "lerp",
                        "lineadj",
                        "linear",
                        "lit",
                        "log",
                        "log10",
                        "log2",
                        "long",
                        "loop",
                        "mad",
                        "matrix",
                        "max",
                        "min",
                        "min10float",
                        "min10float1",
                        "min10float1x1",
                        "min10float1x2",
                        "min10float1x3",
                        "min10float1x4",
                        "min10float2",
                        "min10float2x1",
                        "min10float2x2",
                        "min10float2x3",
                        "min10float2x4",
                        "min10float3",
                        "min10float3x1",
                        "min10float3x2",
                        "min10float3x3",
                        "min10float3x4",
                        "min10float4",
                        "min10float4x1",
                        "min10float4x2",
                        "min10float4x3",
                        "min10float4x4",
                        "min12int",
                        "min12int1",
                        "min12int1x1",
                        "min12int1x2",
                        "min12int1x3",
                        "min12int1x4",
                        "min12int2",
                        "min12int2x1",
                        "min12int2x2",
                        "min12int2x3",
                        "min12int2x4",
                        "min12int3",
                        "min12int3x1",
                        "min12int3x2",
                        "min12int3x3",
                        "min12int3x4",
                        "min12int4",
                        "min12int4x1",
                        "min12int4x2",
                        "min12int4x3",
                        "min12int4x4",
                        "min16float",
                        "min16float1",
                        "min16float1x1",
                        "min16float1x2",
                        "min16float1x3",
                        "min16float1x4",
                        "min16float2",
                        "min16float2x1",
                        "min16float2x2",
                        "min16float2x3",
                        "min16float2x4",
                        "min16float3",
                        "min16float3x1",
                        "min16float3x2",
                        "min16float3x3",
                        "min16float3x4",
                        "min16float4",
                        "min16float4x1",
                        "min16float4x2",
                        "min16float4x3",
                        "min16float4x4",
                        "min16int",
                        "min16int1",
                        "min16int1x1",
                        "min16int1x2",
                        "min16int1x3",
                        "min16int1x4",
                        "min16int2",
                        "min16int2x1",
                        "min16int2x2",
                        "min16int2x3",
                        "min16int2x4",
                        "min16int3",
                        "min16int3x1",
                        "min16int3x2",
                        "min16int3x3",
                        "min16int3x4",
                        "min16int4",
                        "min16int4x1",
                        "min16int4x2",
                        "min16int4x3",
                        "min16int4x4",
                        "min16uint",
                        "min16uint1",
                        "min16uint1x1",
                        "min16uint1x2",
                        "min16uint1x3",
                        "min16uint1x4",
                        "min16uint2",
                        "min16uint2x1",
                        "min16uint2x2",
                        "min16uint2x3",
                        "min16uint2x4",
                        "min16uint3",
                        "min16uint3x1",
                        "min16uint3x2",
                        "min16uint3x3",
                        "min16uint3x4",
                        "min16uint4",
                        "min16uint4x1",
                        "min16uint4x2",
                        "min16uint4x3",
                        "min16uint4x4",
                        "modf",
                        "msad4",
                        "mul",
                        "mutable",
                        "namespace",
                        "new",
                        "nointerpolation",
                        "noise",
                        "noperspective",
                        "normalize",
                        "numthreads",
                        "operator",
                        "out",
                        "packoffset",
                        "pass",
                        "pixelfragment",
                        "pixelshader",
                        "point",
                        "pow",
                        "precise",
                        "printf",
                        "private",
                        "protected",
                        "public",
                        "radians",
                        "rcp",
                        "reflect",
                        "refract",
                        "register",
                        "reinterpret_cast",
                        "return",
                        "reversebits",
                        "round",
                        "row_major",
                        "rsqrt",
                        "sample",
                        "sampler",
                        "sampler1D",
                        "sampler2D",
                        "sampler3D",
                        "samplerCUBE",
                        "sampler_state",
                        "saturate",
                        "shared",
                        "short",
                        "sign",
                        "signed",
                        "sin",
                        "sincos",
                        "sinh",
                        "sizeof",
                        "smoothstep",
                        "snorm",
                        "sqrt",
                        "stateblock",
                        "stateblock_state",
                        "static",
                        "static_cast",
                        "step",
                        "string",
                        "struct",
                        "switch",
                        "tan",
                        "tanh",
                        "tbuffer",
                        "technique",
                        "technique10",
                        "technique11",
                        "template",
                        "tex1D",
                        "tex1Dbias",
                        "tex1Dgrad",
                        "tex1Dlod",
                        "tex1Dproj",
                        "tex2D",
                        "tex2Dbias",
                        "tex2Dgrad",
                        "tex2Dlod",
                        "tex2Dproj",
                        "tex3D",
                        "tex3Dbias",
                        "tex3Dgrad",
                        "tex3Dlod",
                        "tex3Dproj",
                        "texCUBE",
                        "texCUBEbias",
                        "texCUBEgrad",
                        "texCUBElod",
                        "texCUBEproj",
                        "texture",
                        "texture1D",
                        "texture1DArray",
                        "texture2D",
                        "texture2DArray",
                        "texture2DMS",
                        "texture2DMSArray",
                        "texture3D",
                        "textureCube",
                        "textureCubeArray",
                        "this",
                        "throw",
                        "transpose",
                        "triangle",
                        "triangleadj",
                        "true",
                        "trunc",
                        "try",
                        "typedef",
                        "typename",
                        "uint",
                        "uint1",
                        "uint1x1",
                        "uint1x2",
                        "uint1x3",
                        "uint1x4",
                        "uint2",
                        "uint2x1",
                        "uint2x2",
                        "uint2x3",
                        "uint2x4",
                        "uint3",
                        "uint3x1",
                        "uint3x2",
                        "uint3x3",
                        "uint3x4",
                        "uint4",
                        "uint4x1",
                        "uint4x2",
                        "uint4x3",
                        "uint4x4",
                        "uniform",
                        "union",
                        "unorm",
                        "unroll",
                        "unsigned",
                        "using",
                        "vector",
                        "vertexfragment",
                        "vertexshader",
                        "virtual",
                        "void",
                        "volatile",
                        "while"};

}  // namespace

Namer::Namer() = default;

Namer::~Namer() = default;

std::string Namer::NameFor(const std::string& name) {
  // If it's in the name map we can just return it. There are no shadow names
  // in WGSL so this has to be unique in the WGSL names, and we've already
  // checked the name collisions with HLSL.
  auto it = name_map_.find(name);
  if (it != name_map_.end()) {
    return it->second;
  }

  std::string ret_name = name;
  if (std::binary_search(std::begin(kNames), std::end(kNames), ret_name)) {
    uint32_t i = 0;
    // Make sure there wasn't already a tint variable with the new name we've
    // now created.
    while (true) {
      ret_name = name + "_tint_" + std::to_string(i);
      it = name_map_.find(ret_name);
      if (it == name_map_.end()) {
        break;
      }
      i++;
    }
    RegisterRemappedName(ret_name);
  } else {
    uint32_t i = 0;
    // Make sure the ident name wasn't assigned by a remapping.
    while (true) {
      if (!IsRemapped(ret_name)) {
        break;
      }
      ret_name = name + "_" + std::to_string(i);
      i++;
    }
    RegisterRemappedName(ret_name);
  }

  name_map_[name] = ret_name;
  return ret_name;
}

bool Namer::IsMapped(const std::string& name) {
  auto it = name_map_.find(name);
  return it != name_map_.end();
}

bool Namer::IsRemapped(const std::string& name) {
  auto it = remapped_names_.find(name);
  return it != remapped_names_.end();
}

void Namer::RegisterRemappedName(const std::string& name) {
  remapped_names_.insert(name);
}

}  // namespace hlsl
}  // namespace writer
}  // namespace tint