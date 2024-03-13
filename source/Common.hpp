///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
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

#define VERBOSE_MESHES(...)      Logger::Verbose(Self(), __VA_ARGS__)
#define VERBOSE_MESHES_TAB(...)  const auto tab = Logger::Verbose(Self(), __VA_ARGS__, Logger::Tabs {})
