///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Model.hpp"
#include <Flow/Verbs/Create.hpp>


///                                                                           
///   Geometry library module                                                 
///                                                                           
struct GeometryLibrary final : A::AssetModule {
   LANGULUS(ABSTRACT) false;
   LANGULUS_BASES(A::AssetModule);
   LANGULUS_VERBS(Verbs::Create);

private:
   // Model library                                                     
   TFactoryUnique<Model> mModels;
   // Data folder, where models will be saved or loaded from            
   Ptr<A::Folder> mFolder;

public:
   GeometryLibrary(Runtime*, const Descriptor&);

   void Update(Time);
   void Create(Verb&);
};

