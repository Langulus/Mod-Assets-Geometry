///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright (c) 2016 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "../Mesh.hpp"
#include <Math/Primitives/Line.hpp>


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

   NOD() static bool Default(Construct&);
   NOD() static Construct Detail(const Mesh*, const LOD&);

   static void Indices(const Mesh*);
   static void Positions(const Mesh*);
   static void Normals(const Mesh*);
   static void TextureCoords(const Mesh*);
   static void Materials(const Mesh*);
   static void Instances(const Mesh*);
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
      d.SetDefaultTrait<Traits::Place>(
         MetaOf<TLine<PointType>>());
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
Construct GenerateLine<T, TOPOLOGY>::Detail(const Mesh* model, const LOD&) {
   return Construct::From<A::Mesh>(model->GetNeat());
}

/// Generate positions for a line                                             
///   @param model - the model to fill                                        
GENERATE() Positions(const Mesh* model) {
   if constexpr (CT::Line<TOPOLOGY>) {
      // A line made out of lines                                       
      using E = TLine<PointType>;
      TAny<E> data;
      data << E {Axes::Origin<ScalarType>, Axes::Forward<ScalarType>};
      model->Commit<Traits::Place>(Abandon(data));
   }
   else LANGULUS_ERROR("Unsupported topology for line positions");
}

/// Generate normals for a line                                               
///   @param model - the geometry instance to save data in                    
GENERATE() Normals(const Mesh*) {
   TODO();
}

/// Generate indices for a line                                               
///   @param model - the geometry instance to save data in                    
GENERATE() Indices(const Mesh*) {
   TODO();
}

/// Generate texture coordinates for a line                                   
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