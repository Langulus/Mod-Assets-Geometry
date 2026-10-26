///                                                                           
/// Langulus::Module::Art::Geometry                                           
/// Copyright (c) 2016 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Mesh.hpp"
#include <Langulus/Primitives/TTriangle.hpp>
#include <Langulus/Primitives/TLine.hpp>
#include <Langulus/Math/Mapping.hpp>
#include <Langulus/Math/Color.hpp>


///                                                                           
///    Triangle mesh generators                                               
///                                                                           
///   @tparam T the primitive to use for point type and dimensions            
///   @tparam TOPOLOGY are we generating triangles/lines/points?              
///                                                                           
///         2                                                                 
/// +Y|     + -->+X                                                           
///   |    / \                                                                
///   v   /   \         Each corner is at distance 0.5 from center by         
///      /     \        default. The vertices are in the XY plane by          
///     /   o   \       default (Z=0)                                         
///    /  origin \                                                            
///   +-----------+                                                           
///   1           0                                                           
///                                                                           
template<CT::Triangle T, CT::Topology TOPOLOGY = Triangle>
struct GenerateTriangle {
   using PointType = typename T::PointType;
   using ScalarType = TypeOf<PointType>;
   static constexpr size_t Dimensions = T::MemberCount;
   static constexpr ScalarType Half = ScalarType {1} / ScalarType {2};

   /// Triangle unique vertices                                               
   static constexpr size_t VertexCount = 3;
   static constexpr PointType TriangleVertices[VertexCount] = {
      PointType { Half, Half, 0},
      PointType {-Half, Half, 0},
      PointType {    0,-Half, 0},
   };

   static bool Default(Recipe&);
   static auto Detail(Mesh const*, LOD const&) -> Recipe;

   static void Indices(Mesh*);
   static void Positions(Mesh*);
   static void Normals(Mesh*);
   static void TextureCoords(Mesh*);
   static void Materials(Mesh*);
   static void Instances(Mesh*);
};

#define GENERATE() template<CT::Triangle T, CT::Topology TOPOLOGY> \
   void GenerateTriangle<T, TOPOLOGY>::


/// Default triangle generation                                               
///   @param d the descriptor to use                                          
///   @return a newly generated descriptor, with missing tags being set to    
///      their defaults                                                       
template<CT::Triangle T, CT::Topology TOPOLOGY>
bool GenerateTriangle<T, TOPOLOGY>::Default(Recipe& d) {
   if constexpr (CT::Triangle<TOPOLOGY>) {
      // A solid triangles                                              
      d.Set<Tags::Place>(MetaOf<TTriangle<PointType>>());
      d.Set<Tags::Sampler>(MetaOf<Sampler2>());

      if constexpr (Dimensions >= 3) {
         d.Set<Tags::Aim>(MetaOf<Normal3>());
      }
   }
   else if constexpr (CT::Line<TOPOLOGY>) {
      // A triangle of lines                                            
      d.Set<Tags::Place>(MetaOf<TLine<PointType>>());
   }
   else if constexpr (CT::Point<TOPOLOGY>) {
      // A triangle of points                                           
      d.Set<Tags::Place>(MetaOf<PointType>());
   }
   else return false;

   d.Set<Tags::Topology>(MetaOf<TOPOLOGY>());
   d.Set<Tags::MapMode>(MapMode::Model);
   d.SetTarget<Things::Mesh>();
   return true;
}

/// Generate box level of detail, giving a LOD state                          
///   @param model the box generator                                          
///   @param lod the LOD state to generate                                    
///   @return a newly generated descriptor, for the LOD model you can use it  
///      to generate the new geometry                                         
template<CT::Triangle T, CT::Topology TOPOLOGY>
auto GenerateTriangle<T, TOPOLOGY>::Detail(Mesh const* model, LOD const&) -> Recipe {
   return Recipe::For<Things::Mesh>(model->GetDescriptor());
}

/// Generate positions for triangle                                           
///   @param model the model to fill                                          
GENERATE() Positions(Mesh* model) {
   TMany<PointType> data = TriangleVertices;
   model->Commit<Tags::Place>(Abandon(data));
}

/// Generate indices for triangle                                             
///   @param model the geometry instance to save data in                      
GENERATE() Indices(Mesh* model) {
   TMany<uint32_t> data {0, 1, 2};
   model->Commit<Tags::Place>(Abandon(data));
}

/// Generate normals for triangle                                             
///   @param model the geometry instance to save data in                      
GENERATE() Normals(Mesh* model) {
   constexpr Normal3 n = Axes::Backward<ScalarType>;
   TMany<Normal3> data;
   data.Reserve(VertexCount);
   for (auto& v : TriangleVertices)
      data << n;
   model->template Commit<Tags::Aim>(Abandon(data));
}

/// Generate texture coordinates for a box                                    
///   @param model the geometry instance to save data in                      
GENERATE() TextureCoords(Mesh* model) {
   TMany<Sampler2> data;
   data.Reserve(VertexCount);
   for (auto& v : TriangleVertices)
      data << v;
   model->template Commit<Tags::Aim>(Abandon(data));
}

GENERATE() Materials(Mesh*) {
   TODO();
}

GENERATE() Instances(Mesh*) {
   TODO();
}

#undef GENERATE