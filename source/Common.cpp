///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright (c) 2016 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Common.hpp"
#include <Math/Primitives.hpp>
#include "generators/Grid.hpp"


/// Register meshes                                                           
void RegisterMeshes() {
   Math::RegisterPrimitives();

   (void) MetaOf<Grid2>();
   (void) MetaOf<Grid3>();
}