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

#include <Langulus/CppAPI/IO.hpp>


/// Mesh construction                                                         
///   @param producer the producer                                            
///   @param desc mesh descriptor                                             
Mesh::Mesh(MeshLibrary* producer, Many const& desc)
   : Downcastable {this}
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
         LglsAssert(FromDescriptor(desc), "Couldn't create mesh generator");
         LglsAssert(mGenerators, "No generators found in mesh");
      }
   }

   // If this was reached, then mesh was successfully initialized, so   
   // it is ready to be added to the hierarchy of Things                
   Couple(desc);
   VERBOSE_MESHES("Initialized");
}

/// React on environmental change                                             
/*void Mesh::Teardown() {
   mGenerators.Reset();
   mDataListMap.Reset();
}*/

/// React on environmental change                                             
void Mesh::Refresh() {

}

/// Produce geometry data                                                     
///   @param verb creation verb to satisfy                                    
void Mesh::Create(Verb&) {
   TODO();
}

/// Generate data                                                             
///   @param tag the tag to generate                                          
///   @param index tag group to generate                                      
///   @return true if data was generated                                      
bool Mesh::Generate(TMeta tag, size_t index) {
   auto foundData = GetDataListMap().Find(tag);
   if (foundData) {
      if (foundData.GetVal().GetCount() > index)
         return true;
   }

   auto foundGen = mGenerators.Find(tag);
   if (foundGen) {
      (*foundGen.GetVal())(this);
      foundData = GetDataListMap().Find(tag);
      if (foundData) {
         if (foundData.GetVal().GetCount() > index)
            return true;
      }
   }

   return false;
}

/// Get level of detail mesh                                                  
///   @param lod the level of detail state to generate LOD from               
///   @return the new geometry                                                
auto Mesh::GetLOD(LOD const& lod) const -> Ref<Things::Mesh> {
   if (mLODgenerator) {
      // Generate a request, and fulfill it                             
      Verbs::Create creator {mLODgenerator(this, lod)};
      GetLibrary()->Create(creator);
      return creator->template As<Things::Mesh*>();
   }

   return const_cast<Mesh*>(this);
}

/// Analyze a descriptor, fill in any default properties that are missing     
/// by calling the appropriate GENERATOR::Default                             
///   @param desc the descriptor to complete                                  
bool Mesh::AutocompleteDescriptor(Recipe& desc) {
   // The descriptor should have some primitive defined                 
   const auto primitive = desc.FindType<Primitive>();
   if (not primitive)
      return false;

   // The descriptor might or might not have the topology defined       
   DMeta topology;
   desc.ForEachDeep([&](Tags::Topology const& tag) {
      topology = tag.As<DMeta>();
   });

   return AutocompleteInner<GenerateBox,  Box2 >(desc, primitive, topology)
       or AutocompleteInner<GenerateBox,  Box3 >(desc, primitive, topology)
       or AutocompleteInner<GenerateGrid, Grid2>(desc, primitive, topology)
       or AutocompleteInner<GenerateGrid, Grid3>(desc, primitive, topology);
}

/// Populate the mesh view and generator functions, by analyzing descriptor   
///   @param desc the descriptor to parse                                     
bool Mesh::FromDescriptor(Many const& desc) {
   const auto primitive = desc.FindType<Math::Primitive>();
   if (not primitive)
      return false;

   desc.ExtractTag<Tags::Topology >(mView.mTopology);
   desc.ExtractTag<Tags::Bilateral>(mView.mBilateral);
   desc.ExtractTag<Tags::MapMode  >(mView.mTextureMapping);
   
   return FillGenerators<GenerateBox,  Box2 >(primitive)
       or FillGenerators<GenerateBox,  Box3 >(primitive)
       or FillGenerators<GenerateGrid, Grid2>(primitive)
       or FillGenerators<GenerateGrid, Grid3>(primitive);
}

/// Load mesh via filename/file interface                                     
///   @param descriptor the file to load                                      
bool Mesh::FromFile(Many const& desc) {
   Path filename;
   if (not desc.ExtractTag<Tags::Name, Tags::Path>(filename))
      desc.ExtractDataAs(filename);

   if (filename) {
      // Load a filename if such was provided                           
      auto fileInterface = GetProducer()->GetFolder()->RelativeFile(filename);
      if (fileInterface)
         return ReadOBJ(**fileInterface);
   }

   return false;
}

#define HasGenerator(a) ::std::is_invocable_v<decltype(&a), Mesh*>

///                                                                           
template<class GENERATOR>
void Mesh::FillGeneratorsInner() {
   if constexpr (HasGenerator(GENERATOR::Indices))
      mGenerators.Merge(MetaTagOf<Tags::Index>(),      GENERATOR::Indices);
   if constexpr (HasGenerator(GENERATOR::Positions))
      mGenerators.Merge(MetaTagOf<Tags::Place>(),      GENERATOR::Positions);
   if constexpr (HasGenerator(GENERATOR::Normals))
      mGenerators.Merge(MetaTagOf<Tags::Aim>(),        GENERATOR::Normals);
   if constexpr (HasGenerator(GENERATOR::TextureCoords))
      mGenerators.Merge(MetaTagOf<Tags::Sampler>(),    GENERATOR::TextureCoords);
   if constexpr (HasGenerator(GENERATOR::Materials))
      mGenerators.Merge(MetaTagOf<Tags::Material>(),   GENERATOR::Materials);
   if constexpr (HasGenerator(GENERATOR::Detail))
      mLODgenerator = GENERATOR::Detail;
}

///                                                                           
template<template<typename...> class GENERATOR, class PRIMITIVE>
bool Mesh::FillGenerators(DMeta primitive) {
   if (not primitive.CastsTo(MetaDataOf<PRIMITIVE>()))
      return false;

   LglsAssumeDev(mView.mTopology, "Topology not set");
   if (mView.mTopology.CastsTo(MetaDataOf<TriangleStrip>()))
      FillGeneratorsInner<GENERATOR<PRIMITIVE, TriangleStrip>>();
   else if (mView.mTopology.CastsTo(MetaDataOf<Triangle>()))
      FillGeneratorsInner<GENERATOR<PRIMITIVE, Triangle>>();
   else if (mView.mTopology.CastsTo(MetaDataOf<LineStrip>()))
      FillGeneratorsInner<GENERATOR<PRIMITIVE, LineStrip>>();
   else if (mView.mTopology.CastsTo(MetaDataOf<Line>()))
      FillGeneratorsInner<GENERATOR<PRIMITIVE, Line>>();
   else if (mView.mTopology.CastsTo(MetaDataOf<Point>()))
      FillGeneratorsInner<GENERATOR<PRIMITIVE, Point>>();
   else
      LglsError("Unsupported topology: ", mView.mTopology);
   return true;
}

///                                                                           
template<template<typename...> class GENERATOR, class PRIMITIVE>
bool Mesh::AutocompleteInner(Recipe& out, DMeta primitive, DMeta topology) {
   if (not primitive.CastsTo(MetaDataOf<PRIMITIVE>()))
      return false;
   
   if (not topology)
      return GENERATOR<PRIMITIVE>::Default(out);
   else if (topology.CastsTo(MetaDataOf<TriangleStrip>()))
      return GENERATOR<PRIMITIVE, TriangleStrip>::Default(out);
   else if (topology.CastsTo(MetaDataOf<Triangle>()))
      return GENERATOR<PRIMITIVE, Triangle>::Default(out);
   else if (topology.CastsTo(MetaDataOf<LineStrip>()))
      return GENERATOR<PRIMITIVE, LineStrip>::Default(out);
   else if (topology.CastsTo(MetaDataOf<Line>()))
      return GENERATOR<PRIMITIVE, Line>::Default(out);
   else if (topology.CastsTo(MetaDataOf<Point>()))
      return GENERATOR<PRIMITIVE, Point>::Default(out);
   else
      return false;
}

/// Get the mesh library                                                      
///   @return the library                                                     
auto Mesh::GetLibrary() const -> MeshLibrary* {
   return mProducer.template As<MeshLibrary>();
}
