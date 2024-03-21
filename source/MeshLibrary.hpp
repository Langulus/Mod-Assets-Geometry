///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Mesh.hpp"
#include <Flow/Verbs/Create.hpp>


///                                                                           
///   Mesh reading, writing, and generation module                            
///                                                                           
struct MeshLibrary final : A::AssetModule {
   LANGULUS(ABSTRACT) false;
   LANGULUS_BASES(A::AssetModule);
   LANGULUS_VERBS(Verbs::Create);

private:
   // Mesh library                                                      
   TFactoryUnique<Mesh> mMeshes;

public:
   MeshLibrary(Runtime*, const Neat&);

   void Create(Verb&);
};

