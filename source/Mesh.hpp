///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Common.hpp"

// Unfortunately, due to compiler bugs in MSVC and Clang, we can't      
// generalize these generators yet. Some day...                         
//template<CT::Primitive, CT::Topology = A::Triangle, bool = false>
//struct Generator;


///                                                                           
///   A mesh asset                                                            
///                                                                           
struct Mesh final : A::Mesh {
   LANGULUS(ABSTRACT) false;
   LANGULUS(PRODUCER) MeshLibrary;
   LANGULUS(FILES) "obj";
   LANGULUS_BASES(A::Mesh);
   LANGULUS_VERBS(Verbs::Create);

public:
   Mesh(MeshLibrary*, const Descriptor&);

   void Refresh();
   void Create(Verb&);

   NOD() Ref<A::Mesh> GetLOD(const LOD&) const;

private:
   void FromPrimitive(const Block&);
   template<template<typename...> class GENERATOR, class PRIMITIVE>
   bool FillGenerators(const Block&);
   template<class GENERATOR>
   void FillGeneratorsInner();
   void LoadFile(const Any&);

   // Signature for a generator function                                
   using FGenerate = void (*) (Mesh*);

   // Generator functions for each supported type of data               
   TUnorderedMap<TMeta, FGenerate> mGenerators;
};


///                                                                           
template<class GENERATOR>
void Mesh::FillGeneratorsInner() {
   mGenerators[Traits::Index::GetTrait()] = GENERATOR::Indices;
   mGenerators[Traits::Place::GetTrait()] = GENERATOR::Positions;
   mGenerators[Traits::Aim::GetTrait()] = GENERATOR::Normals;
   mGenerators[Traits::Sampler::GetTrait()] = GENERATOR::TextureCoords;
   mGenerators[Traits::Material::GetTrait()] = GENERATOR::Materials;
   mGenerators[Traits::Transform::GetTrait()] = GENERATOR::Instances;
}

///                                                                           
template<template<typename...> class GENERATOR, class PRIMITIVE>
bool Mesh::FillGenerators(const Block& data) {
   if (data.IsExact<PRIMITIVE>()) {
      if (!mView.mTopology)
         FillGeneratorsInner<GENERATOR<PRIMITIVE>>();
      else if (mView.mTopology->CastsTo<A::TriangleStrip>())
         FillGeneratorsInner<GENERATOR<PRIMITIVE, A::TriangleStrip>>();
      else if (mView.mTopology->CastsTo<A::Triangle>())
         FillGeneratorsInner<GENERATOR<PRIMITIVE, A::Triangle>>();
      else if (mView.mTopology->CastsTo<A::LineStrip>())
         FillGeneratorsInner<GENERATOR<PRIMITIVE, A::LineStrip>>();
      else if (mView.mTopology->CastsTo<A::Line>())
         FillGeneratorsInner<GENERATOR<PRIMITIVE, A::Line>>();
      else if (mView.mTopology->CastsTo<A::Point>())
         FillGeneratorsInner<GENERATOR<PRIMITIVE, A::Point>>();
      else
         LANGULUS_THROW(Mesh, "Unsupported topology");
      return true;
   }

   return false;
}