///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright (c) 2016 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include <Langulus/Mesh.hpp>

using namespace Langulus;
using namespace Math;

struct MeshLibrary;
struct Mesh;

LANGULUS_DEFINE_TRAIT(Tesselation, "Tesselation level, usually an integer");

#if 0
   #define VERBOSE_MESHES(...)      Logger::Verbose(Self(), __VA_ARGS__)
   #define VERBOSE_MESHES_TAB(...)  const auto tab = Logger::VerboseTab(Self(), __VA_ARGS__)
#else
   #define VERBOSE_MESHES(...)      LANGULUS(NOOP)
   #define VERBOSE_MESHES_TAB(...)  LANGULUS(NOOP)
#endif

void RegisterMeshes();