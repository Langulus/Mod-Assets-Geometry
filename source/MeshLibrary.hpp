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
   // Where meshes are read and written to                              
   Path mMeshFolder;
   // Mesh library                                                      
   TFactoryUnique<::Mesh> mMeshes;
   // Data folder, where models will be saved or loaded from            
   Ref<A::Folder> mFolder;

public:
   MeshLibrary(Runtime*, const Neat&);

   void Create(Verb&);

   const A::Folder* GetFolder() const noexcept;
};

