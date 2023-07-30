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
///   A mesh asset                                                            
///                                                                           
struct Mesh final : A::Mesh {
   LANGULUS(ABSTRACT) false;
   LANGULUS(PRODUCER) MeshLibrary;
   LANGULUS_BASES(A::Mesh);
   LANGULUS_VERBS(Verbs::Create);

public:
   Mesh(MeshLibrary*, const Descriptor&);
   ~Mesh();

   void Create(Verb&);
   void Refresh();

   NOD() Ref<A::Mesh> GetLOD(const Math::LOD&) const;
};

#include "generators/Box.inl"