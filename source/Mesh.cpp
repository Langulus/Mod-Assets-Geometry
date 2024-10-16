///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright (c) 2016 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Mesh.hpp"
#include "MeshLibrary.hpp"

#include "generators/Box.inl"
#include "generators/Cylinder.inl"
#include "generators/Foliage.inl"
#include "generators/Frustum.inl"
#include "generators/Grid.inl"
#include "generators/Label.inl"
#include "generators/Line.inl"
#include "generators/Sphere.inl"
#include "generators/Triangle.inl"
#include "generators/Zode.inl"

#include <Langulus/IO.hpp>


/// Mesh construction                                                         
///   @param producer - the producer                                          
///   @param desc - mesh descriptor                                           
Mesh::Mesh(MeshLibrary* producer, const Many& desc)
   : Resolvable   {this}
   , ProducedFrom {producer, desc} {
   // Get a path from the descriptor                                    
   VERBOSE_MESHES("Initializing...");

   if (not FromFile(desc)) {
      // Mesh isn't file-based, so inspect the descriptor more closely  
      if (desc.ExtractData(mView)) {
         // Upload raw data if any                                      
         Bytes rawData;
         if (desc.ExtractData(rawData))
            TODO();
      }
      else {
         // Configure a generator from descriptor                       
         LANGULUS_ASSERT(FromDescriptor(desc),
            Mesh, "Couldn't create mesh generator");
         LANGULUS_ASSERT(mGenerators,
            Mesh, "No generators found in mesh");
      }
   }

   // If this was reached, then mesh was successfully initialized, so   
   // it is ready to be added to the hierarchy of Things                
   Couple(desc);
   VERBOSE_MESHES("Initialized");
}

/// React on environmental change                                             
void Mesh::Refresh() {

}

/// Produce geometry data                                                     
///   @param verb - creation verb to satisfy                                  
void Mesh::Create(Verb&) {
   TODO();
}

/// Generate data                                                             
///   @param trait - the trait to generate                                    
///   @param index - trait group to generate                                  
///   @return true if data was generated                                      
bool Mesh::Generate(TMeta trait, Offset index) {
   auto foundData = GetDataListMap().FindIt(trait);
   if (foundData) {
      if (foundData.GetValue().GetCount() > index)
         return true;
   }

   auto foundGen = mGenerators.FindIt(trait);
   if (foundGen) {
      foundGen.GetValue()(this);
      foundData = GetDataListMap().FindIt(trait);
      if (foundData) {
         if (foundData.GetValue().GetCount() > index)
            return true;
      }
   }

   return false;
}

/// Get level of detail mesh                                                  
///   @param lod - the level of detail state to generate LOD from             
///   @return the new geometry                                                
auto Mesh::GetLOD(const LOD& lod) const -> Ref<A::Mesh> {
   if (mLODgenerator) {
      // Generate a request, and fulfill it                             
      Verbs::Create creator {mLODgenerator(this, lod)};
      mProducer.As<MeshLibrary>()->Create(creator);
      return creator->template As<A::Mesh*>();
   }

   return const_cast<Mesh*>(this);
}

/// Analyze a descriptor, fill in any default properties that are missing     
/// by calling the appropriate GENERATOR::Default                             
///   @param desc - the descriptor to complete                                
bool Mesh::AutocompleteDescriptor(Construct& desc) {
   // The descriptor should have some primitive defined                 
   const auto primitive = desc->FindType<A::Primitive>();
   if (not primitive)
      return false;

   // The descriptor might or might not have the topology defined       
   DMeta topology;
   desc->ForEachDeep([&](const Traits::Topology& trait) {
      topology = trait.As<DMeta>();
   });

   return AutocompleteInner<GenerateBox,  Box2 >(desc, primitive, topology)
       or AutocompleteInner<GenerateBox,  Box3 >(desc, primitive, topology)
       or AutocompleteInner<GenerateGrid, Grid2>(desc, primitive, topology)
       or AutocompleteInner<GenerateGrid, Grid3>(desc, primitive, topology);
}

/// Populate the mesh view and generator functions, by analyzing descriptor   
///   @param desc - the descriptor to parse                                   
bool Mesh::FromDescriptor(const Many& desc) {
   const auto primitive = desc.FindType<A::Primitive>();
   if (not primitive)
      return false;

   desc.ExtractTrait<Traits::Topology >(mView.mTopology);
   desc.ExtractTrait<Traits::Bilateral>(mView.mBilateral);
   desc.ExtractTrait<Traits::MapMode  >(mView.mTextureMapping);
   
   return FillGenerators<GenerateBox,  Box2 >(primitive)
       or FillGenerators<GenerateBox,  Box3 >(primitive)
       or FillGenerators<GenerateGrid, Grid2>(primitive)
       or FillGenerators<GenerateGrid, Grid3>(primitive);
}

/// Load mesh via filename/file interface                                     
///   @param descriptor - the file to load                                    
bool Mesh::FromFile(const Many& desc) {
   Path filename;
   if (not desc.ExtractTrait<Traits::Name, Traits::Path>(filename))
      desc.ExtractDataAs(filename);

   if (filename) {
      // Load a filename if such was provided                           
      auto fileInterface = GetProducer()->GetFolder()->RelativeFile(filename);
      if (fileInterface)
         return ReadOBJ(*fileInterface);
   }

   return false;
}

#define HasGenerator(a) ::std::is_invocable_v<decltype(&a), Mesh*>

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