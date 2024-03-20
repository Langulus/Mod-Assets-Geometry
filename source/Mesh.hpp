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

   static bool AutocompleteDescriptor(Construct&);

private:
   template<template<typename...> class GENERATOR, class PRIMITIVE>
   static bool AutocompleteInner(Construct&, DMeta, DMeta = {});

   void FromDescriptor(const Neat&);
   template<template<typename...> class GENERATOR, class PRIMITIVE>
   bool FillGenerators(DMeta);
   template<class GENERATOR>
   void FillGeneratorsInner();

   void LoadFile(const Any&);
   bool ReadOBJ(const A::File&);
   //bool WriteOBJ(const A::File&) const;

   // Generator functions for each supported type of data               
   using FGenerator = void(*)(Mesh*);
   TUnorderedMap<TMeta, FGenerator> mGenerators;

   // LOD generator function                                            
   using FLOD = Construct(*)(const Mesh*, const LOD&);
   FLOD mLODgenerator {};
};

#define HasGenerator(a) ::std::is_member_function_pointer_v<decltype(&a)>


///                                                                           
template<class GENERATOR>
void Mesh::FillGeneratorsInner() {
   if constexpr (HasGenerator(GENERATOR::Indices))
      mGenerators.Insert(MetaOf<Traits::Index>(),      GENERATOR::Indices);
   if constexpr (HasGenerator(GENERATOR::Positions))
      mGenerators.Insert(MetaOf<Traits::Place>(),      GENERATOR::Positions);
   if constexpr (HasGenerator(GENERATOR::Normals))
      mGenerators.Insert(MetaOf<Traits::Aim>(),        GENERATOR::Normals);
   if constexpr (HasGenerator(GENERATOR::TextureCoords))
      mGenerators.Insert(MetaOf<Traits::Sampler>(),    GENERATOR::TextureCoords);
   if constexpr (HasGenerator(GENERATOR::Materials))
      mGenerators.Insert(MetaOf<Traits::Material>(),   GENERATOR::Materials);
   if constexpr (HasGenerator(GENERATOR::Detail))
      mLODgenerator = GENERATOR::Detail;
}

///                                                                           
template<template<typename...> class GENERATOR, class PRIMITIVE>
bool Mesh::FillGenerators(DMeta primitive) {
   if (not primitive->CastsTo<PRIMITIVE>())
      return false;

   LANGULUS_ASSUME(DevAssumes, mView.mTopology, "Topology not set");
   if (mView.mTopology->CastsTo<A::TriangleStrip>())
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
      LANGULUS_OOPS(Mesh, "Unsupported topology: ", mView.mTopology);
   return true;
}

///                                                                           
template<template<typename...> class GENERATOR, class PRIMITIVE>
bool Mesh::AutocompleteInner(Construct& out, DMeta primitive, DMeta topology) {
   if (not primitive->CastsTo<PRIMITIVE>())
      return false;
   
   if (not topology)
      return GENERATOR<PRIMITIVE>::Default(out);
   else if (topology->CastsTo<A::TriangleStrip>())
      return GENERATOR<PRIMITIVE, A::TriangleStrip>::Default(out);
   else if (topology->CastsTo<A::Triangle>())
      return GENERATOR<PRIMITIVE, A::Triangle>::Default(out);
   else if (topology->CastsTo<A::LineStrip>())
      return GENERATOR<PRIMITIVE, A::LineStrip>::Default(out);
   else if (topology->CastsTo<A::Line>())
      return GENERATOR<PRIMITIVE, A::Line>::Default(out);
   else if (topology->CastsTo<A::Point>())
      return GENERATOR<PRIMITIVE, A::Point>::Default(out);
   else
      return false;
}