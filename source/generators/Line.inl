///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
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

   NOD() static Construct Default(Neat&&);
   NOD() static Construct Detail(const Mesh*, const LOD&);

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
Construct GenerateLine<T, TOPOLOGY>::Default(Neat&& descriptor) {
   auto d = Forward<Neat>(descriptor);

   if constexpr (CT::Line<TOPOLOGY>) {
      // A line made out of lines (duh)                                 
      d.SetDefaultTrait<Traits::Topology>(
         MetaOf<TOPOLOGY>());
      d.SetDefaultTrait<Traits::Place>(
         MetaOf<TLine<PointType>>());
   }
   else LANGULUS_ERROR("Unsupported topology for line");

   return Construct::From<A::Mesh>(Abandon(d));
}

/// Generate line level of detail, giving a LOD state                         
///   @param model - the line generator                                       
///   @param lod - the LOD state to generate                                  
///   @return a newly generated descriptor, for the LOD model you can use it  
///           to generate the new geometry                                    
template<CT::Line T, CT::Topology TOPOLOGY>
Construct GenerateLine<T, TOPOLOGY>::Detail(const Mesh* model, const LOD&) {
   return Construct {model->GetNeat()};
}

/// Generate positions for a line                                             
///   @param model - the model to fill                                        
GENERATE() Positions(Mesh* model) {
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
GENERATE() Normals(Mesh* model) {
   TODO();
}

/// Generate indices for a line                                               
///   @param model - the geometry instance to save data in                    
GENERATE() Indices(Mesh* model) {
   TODO();
}

/// Generate texture coordinates for a line                                   
///   @param model - the geometry instance to save data in                    
GENERATE() TextureCoords(Mesh* model) {
   TODO();
}

GENERATE() Materials(Mesh*) {
   TODO();
}

GENERATE() Instances(Mesh*) {
   TODO();
}

#undef GENERATE