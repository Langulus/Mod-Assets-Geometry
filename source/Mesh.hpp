///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright (c) 2016 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Export.hpp"
#include <Langulus/Primitives/TBox.hpp>
#include <Langulus/Primitives/TTriangle.hpp>
#include <Langulus/Primitives/TLine.hpp>
#include <Langulus/Primitives/TCylinder.hpp>
#include <Langulus/Math/Mapping.hpp>
#include <Langulus/Math/Color.hpp>
#include <Langulus/Math/LOD.hpp>
/*#include <Langulus/CppAPI/Material.hpp>
#include <Langulus/CppAPI/Mesh.hpp>*/

using namespace Langulus;
using namespace Math;

//TODO Unfortunately, due to compiler bugs in MSVC and Clang, we can't
// generalize these generators yet. Some day we will...
//template<CT::Primitive, CT::Topology = A::Triangle, bool = false>
//struct Generator;

struct MeshLibrary;

///                                                                           
///   A mesh asset                                                            
///                                                                           
struct Mesh final : Things::Mesh {
   using CTTI_Abstract  = No;
   using CTTI_Producer  = MeshLibrary;
   using CTTI_Files     = Yes<"obj">;
   using CTTI_Bases     = Things::Mesh;
   using CTTI_Ability   = Verbs::Create;

public:
   Mesh(MeshLibrary*, Many const&);

   void Create(Verb&);

   void Refresh() override;
   bool Generate(TMeta, size_t = 0) override;

   auto GetLOD(LOD const&) const -> Ref<Things::Mesh> override;
   auto GetLibrary() const -> MeshLibrary*;
   static bool AutocompleteDescriptor(Recipe&);

private:
   template<template<typename...> class GENERATOR, class PRIMITIVE>
   static bool AutocompleteInner(Recipe&, DMeta, DMeta = {});

   bool FromDescriptor(Many const&);
   bool FromFile(Many const&);

   template<template<typename...> class GENERATOR, class PRIMITIVE>
   bool FillGenerators(DMeta);
   template<class GENERATOR>
   void FillGeneratorsInner();

   bool ReadOBJ(Things::File const&);
   //bool WriteOBJ(Things::File const&) const;

   // Generator functions for each supported type of data               
   using FGenerator = void(*)(Mesh*);
   TMapUnsorted<TMeta, FGenerator> mGenerators;

   // LOD generator function                                            
   using FLOD = Recipe(*)(Mesh const*, LOD const&);
   FLOD mLODgenerator {};
};
