///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright (c) 2016 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include <Langulus/Primitives/TBox.hpp>
#include <Langulus/Primitives/TTriangle.hpp>
#include <Langulus/Primitives/TLine.hpp>
#include <Langulus/Primitives/TCylinder.hpp>
#include <Langulus/Mapping.hpp>
#include <Langulus/Colors/TColor.hpp>
#include <Langulus/Material.hpp>
#include <Langulus/Mesh.hpp>

using namespace Langulus;

//TODO Unfortunately, due to compiler bugs in MSVC and Clang, we can't
// generalize these generators yet. Some day we will...
//template<CT::Primitive, CT::Topology = A::Triangle, bool = false>
//struct Generator;


///                                                                           
///   A mesh asset                                                            
///                                                                           
struct Mesh final : Things::Mesh {
   LANGULUS(ABSTRACT) false;
   LANGULUS(PRODUCER) MeshLibrary;
   LANGULUS(FILES) "obj";
   LANGULUS_BASES(A::Mesh);
   LANGULUS_VERBS(Verbs::Create);

public:
   Mesh(MeshLibrary*, const Many&);

   void Refresh();
   void Create(Verb&);
   bool Generate(TMeta, size_t = 0);

   auto GetLOD(const LOD&) const -> Ref<Things::Mesh>;
   auto GetLibrary() const -> MeshLibrary*;
   static bool AutocompleteDescriptor(Construct&);

private:
   template<template<typename...> class GENERATOR, class PRIMITIVE>
   static bool AutocompleteInner(Construct&, DMeta, DMeta = {});

   bool FromDescriptor(const Many&);
   bool FromFile(const Many&);

   template<template<typename...> class GENERATOR, class PRIMITIVE>
   bool FillGenerators(DMeta);
   template<class GENERATOR>
   void FillGeneratorsInner();

   bool ReadOBJ(const A::File&);
   //bool WriteOBJ(const A::File&) const;

   // Generator functions for each supported type of data               
   using FGenerator = void(*)(Mesh*);
   TUnorderedMap<TMeta, FGenerator> mGenerators;

   // LOD generator function                                            
   using FLOD = Construct(*)(const Mesh*, const LOD&);
   FLOD mLODgenerator {};
};
