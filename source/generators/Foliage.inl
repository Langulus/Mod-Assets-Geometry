///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright (c) 2016 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Mesh.hpp"
#include <Math/Primitives/Triangle.hpp>
#include <Math/Mapping.hpp>


namespace Langulus::A
{
   /// An abstract foliage                                                    
   struct Foliage {
      LANGULUS(ABSTRACT) true;
   };
}

namespace Langulus::CT
{
   /// Concept for distinguishing box primitives                              
   template<class...T>
   concept Foliage = (DerivedFrom<T, A::Foliage> and ...);
}


///                                                                           
///    Foliage mesh generators                                                
///                                                                           
///   @tparam T - the primitive to use for point type and dimensions          
///   @tparam TOPOLOGY - are we generating triangles/lines/points?            
///                                                                           
template<CT::Foliage T, CT::Topology TOPOLOGY = A::Triangle>
struct GenerateFoliage {
   using PointType = typename T::PointType;
   static constexpr Count Dimensions = T::MemberCount;

   NOD() static bool Default(Construct&);
   NOD() static Construct Detail(const Mesh*, const LOD&);

   static void Indices(const Mesh*);
   static void Positions(const Mesh*);
   static void Normals(const Mesh*);
   static void TextureCoords(const Mesh*);
   static void Materials(const Mesh*);
   static void Instances(const Mesh*);
};

#define GENERATE() template<CT::Foliage T, CT::Topology TOPOLOGY> \
   void GenerateFoliage<T, TOPOLOGY>::


/// Default foliage generation                                                
///   @param descriptor - the descriptor to use                               
///   @return a newly generated descriptor, with missing traits being set to  
///           their defaults                                                  
template<CT::Foliage T, CT::Topology TOPOLOGY>
bool GenerateFoliage<T, TOPOLOGY>::Default(Construct& desc) {
   auto& d = desc.GetDescriptor();

   if constexpr (CT::Triangle<TOPOLOGY>) {
      // Foliage made out of triangles                                  
      d.SetDefaultTrait<Traits::Place>(
         MetaOf<TTriangle<PointType>>());
   }
   else return false;

   d.SetDefaultTrait<Traits::Topology>(MetaOf<TOPOLOGY>());
   desc.SetType<A::Mesh>();
   return true;
}

/// Generate foliage level of detail, giving a LOD state                      
///   @param model - the foliage generator                                    
///   @param lod - the LOD state to generate                                  
///   @return a newly generated descriptor, for the LOD model you can use it  
///           to generate the new geometry                                    
template<CT::Foliage T, CT::Topology TOPOLOGY>
Construct GenerateFoliage<T, TOPOLOGY>::Detail(const Mesh* model, const LOD&) {
   return Construct::From<A::Mesh>(model->GetNeat());
}

/// Generate positions for foliage                                            
///   @param model - the model to fill                                        
GENERATE() Positions(const Mesh*) {
   TODO();
}

/// Generate normals for foliage                                              
///   @param model - the geometry instance to save data in                    
GENERATE() Normals(const Mesh*) {
   TODO();
}

/// Generate indices for foliage                                              
///   @param model - the geometry instance to save data in                    
GENERATE() Indices(const Mesh*) {
   TODO();
}

/// Generate texture coordinates for foliage                                  
///   @param model - the geometry instance to save data in                    
GENERATE() TextureCoords(const Mesh*) {
   TODO();
}

GENERATE() Materials(const Mesh*) {
   TODO();
}

GENERATE() Instances(const Mesh*) {
   TODO();
}

#undef GENERATE