///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright (c) 2016 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Mesh.hpp"
#include <Langulus/Math/Primitives/Line.hpp>


///                                                                           
///    Line mesh generators                                                   
///                                                                           
///   @tparam T - the primitive to use for point type and dimensions          
///   @tparam TOPOLOGY - are we generating triangles/lines/points?            
///                                                                           
template<CT::Line T, CT::Topology TOPOLOGY = A::Triangle>
struct GenerateLine {
   using PointType = typename T::PointType;
   using ScalarType = TypeOf<PointType>;
   static constexpr Count Dimensions = T::MemberCount;

   static bool Default(Construct&);
   static auto Detail(const Mesh*, const LOD&) -> Construct;

   static void Indices(Mesh*);
   static void Positions(Mesh*);
   static void Normals(Mesh*);
   static void TextureCoords(Mesh*);
   static void Materials(Mesh*);
   static void Instances(Mesh*);
};

#define GENERATE() template<CT::Line T, CT::Topology TOPOLOGY> \
   void GenerateLine<T, TOPOLOGY>::


/// Default line generation                                                   
///   @param descriptor - the descriptor to use                               
///   @return a newly generated descriptor, with missing traits being set to  
///           their defaults                                                  
template<CT::Line T, CT::Topology TOPOLOGY>
bool GenerateLine<T, TOPOLOGY>::Default(Construct& desc) {
   auto& d = desc.GetDescriptor();

   if constexpr (CT::Line<TOPOLOGY>) {
      // A line made out of lines (duh)                                 
      d.SetDefaultTrait<Traits::Place>(MetaOf<TLine<PointType>>());
   }
   else return false;

   d.SetDefaultTrait<Traits::Topology>(MetaOf<TOPOLOGY>());
   desc.SetType<A::Mesh>();
   return true;
}

/// Generate line level of detail, giving a LOD state                         
///   @param model - the line generator                                       
///   @param lod - the LOD state to generate                                  
///   @return a newly generated descriptor, for the LOD model you can use it  
///           to generate the new geometry                                    
template<CT::Line T, CT::Topology TOPOLOGY>
auto GenerateLine<T, TOPOLOGY>::Detail(const Mesh* model, const LOD&) -> Construct {
   return Construct::From<A::Mesh>(model->GetDescriptor());
}

/// Generate positions for a line                                             
///   @param model - the model to fill                                        
GENERATE() Positions(Mesh* model) {
   if constexpr (CT::Line<TOPOLOGY>) {
      // A line made out of lines                                       
      using E = TLine<PointType>;
      TMany<E> data;
      data << E {Axes::Origin<ScalarType>, Axes::Forward<ScalarType>};
      model->Commit<Traits::Place>(Abandon(data));
   }
   else static_assert(false, "Unsupported topology for line positions");
}

/// Generate normals for a line                                               
///   @param model - the geometry instance to save data in                    
GENERATE() Normals(Mesh*) {
   TODO();
}

/// Generate indices for a line                                               
///   @param model - the geometry instance to save data in                    
GENERATE() Indices(Mesh*) {
   TODO();
}

/// Generate texture coordinates for a line                                   
///   @param model - the geometry instance to save data in                    
GENERATE() TextureCoords(Mesh*) {
   TODO();
}

GENERATE() Materials(Mesh*) {
   TODO();
}

GENERATE() Instances(Mesh*) {
   TODO();
}

#undef GENERATE