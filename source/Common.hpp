///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include <Entity/External.hpp>
#include <Math/LOD.hpp>

using namespace Langulus;
using namespace Langulus::Flow;
using namespace Langulus::Anyness;
using namespace Langulus::Entity;
using namespace Langulus::Math;

struct GeometryLibrary;
struct Model;

template<CT::Real T = Real>
constexpr T Half = T {1} / T {2};

LANGULUS_DEFINE_TRAIT(Tesselation, "Tesselation level, usually an integer");
