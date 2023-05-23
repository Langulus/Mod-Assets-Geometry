///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Common.hpp"


///                                                                           
///   A model asset                                                           
///                                                                           
struct Model final : A::Geometry {
   LANGULUS(ABSTRACT) false;
   LANGULUS(PRODUCER) GeometryLibrary;
   LANGULUS_BASES(A::Geometry);
   LANGULUS_VERBS(Verbs::Create);

public:
   Model(GeometryLibrary*, const Descriptor&);
   ~Model();

   void Create(Verb&);
   void Refresh();

   NOD() const Geometry* GetLOD(const Math::LOD&) const;
};

#include "generators/Box.inl"