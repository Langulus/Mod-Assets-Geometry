///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright (c) 2016 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Mesh.hpp"
#include <Langulus/Factory.hpp>


///                                                                           
///   Mesh reading, writing, and generation module                            
///                                                                           
struct MeshLibrary final : Things::ArtModule {
   using CTTI_Abstract  = No;
   using CTTI_Bases     = Things::ArtModule;
   using CTTI_Ability   = Verbs::Create;

private:
   // Mesh library                                                      
   TFactoryUnique<Mesh> mMeshes;

public:
   MeshLibrary(Runtime*, Many const&);

   void Create(Verb&);

   void Teardown() override;
   void RequestGarbageCollection() override {}
};

