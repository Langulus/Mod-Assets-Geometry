///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright (c) 2016 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
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

   bool FromDescriptor(const Neat&);
   bool FromFile(const Neat&);

   template<template<typename...> class GENERATOR, class PRIMITIVE>
   bool FillGenerators(DMeta);
   template<class GENERATOR>
   void FillGeneratorsInner();

   bool ReadOBJ(const A::File&);
   //bool WriteOBJ(const A::File&) const;

   // Generator functions for each supported type of data               
   using FGenerator = void(*)(const Mesh*);
   TUnorderedMap<TMeta, FGenerator> mGenerators;

   // LOD generator function                                            
   using FLOD = Construct(*)(const Mesh*, const LOD&);
   FLOD mLODgenerator {};
};
