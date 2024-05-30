///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright (c) 2016 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include <Langulus.hpp>
#include <Langulus/Mesh.hpp>

using namespace Langulus;
using namespace Math;

struct MeshLibrary;
struct Mesh;

LANGULUS_DEFINE_TRAIT(Tesselation, "Tesselation level, usually an integer");

#if 0
   #define VERBOSE_MESHES(...)      Logger::Verbose(Self(), __VA_ARGS__)
   #define VERBOSE_MESHES_TAB(...)  const auto tab = Logger::Verbose(Self(), __VA_ARGS__, Logger::Tabs {})
#else
   #define VERBOSE_MESHES(...)      LANGULUS(NOOP)
   #define VERBOSE_MESHES_TAB(...)  LANGULUS(NOOP)
#endif
