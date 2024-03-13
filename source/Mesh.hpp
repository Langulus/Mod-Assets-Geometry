///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Common.hpp"
#include <Math/Primitives/Box.hpp>
#include <Math/Primitives/Triangle.hpp>
#include <Math/Primitives/Line.hpp>
#include <Math/Primitives/Cylinder.hpp>
#include <Math/Mapping.hpp>
#include <Math/Color.hpp>
#include <Langulus/Material.hpp>

//TODO Unfortunately, due to compiler bugs in MSVC and Clang, we can't
// generalize these generators yet. Some day we will...
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
   Mesh(MeshLibrary*, const Neat&);

   void Refresh();
   void Create(Verb&);

   bool Generate(TMeta, Offset = 0);

   NOD() Ref<A::Mesh> GetLOD(const LOD&) const;

private:
   void FromPrimitive(const Block&);
   template<template<typename...> class GENERATOR, class PRIMITIVE>
   bool FillGenerators(const Block&);
   template<class GENERATOR>
   void FillGeneratorsInner();

   void LoadFile(const Any&);
   bool ReadOBJ(const A::File&);
   bool WriteOBJ(const A::File&) const;

   // Generator functions for each supported type of data               
   using FGenerator = void(*)(Mesh*);
   TUnorderedMap<TMeta, FGenerator> mGenerators;

   // LOD generator function                                            
   using FLOD = Construct(*)(const Mesh*, const LOD&);
   FLOD mLODgenerator {};
};


///                                                                           
template<class GENERATOR>
void Mesh::FillGeneratorsInner() {
   mGenerators.Insert(MetaOf<Traits::Index>(),      GENERATOR::Indices);
   mGenerators.Insert(MetaOf<Traits::Place>(),      GENERATOR::Positions);
   mGenerators.Insert(MetaOf<Traits::Aim>(),        GENERATOR::Normals);
   mGenerators.Insert(MetaOf<Traits::Sampler>(),    GENERATOR::TextureCoords);
   mGenerators.Insert(MetaOf<Traits::Material>(),   GENERATOR::Materials);
   mGenerators.Insert(MetaOf<Traits::Transform>(),  GENERATOR::Instances);
   mLODgenerator = GENERATOR::Detail;
}

///                                                                           
template<template<typename...> class GENERATOR, class PRIMITIVE>
bool Mesh::FillGenerators(const Block& data) {
   if (data.IsExact<Any, PRIMITIVE>()) {
      if (not mView.mTopology)
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
         LANGULUS_OOPS(Mesh, "Unsupported topology");
      return true;
   }

   return false;
}