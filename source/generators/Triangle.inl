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
#include <Math/Primitives/Triangle.hpp>
#include <Math/Primitives/Line.hpp>
#include <Math/Mapping.hpp>
#include <Math/Color.hpp>


///                                                                           
///    Triangle mesh generators                                               
///                                                                           
///   @tparam T - the primitive to use for point type and dimensions          
///   @tparam TOPOLOGY - are we generating triangles/lines/points?            
///                                                                           
///         2                                                                 
/// +Y|     + -->+X                                                           
///   |    / \                                                                
///   v   /   \         Each corner is at distance 0.5 from center by         
///      /     \        default. The vertices are in the XY plane by          
///     /   +   \       default (Z=0)                                         
///    /  origin \                                                            
///   +-----------+                                                           
///   1           0                                                           
///                                                                           
template<CT::Triangle T, CT::Topology TOPOLOGY = A::Triangle>
struct GenerateTriangle {
   using PointType = typename T::PointType;
   using ScalarType = TypeOf<PointType>;
   static constexpr Count Dimensions = T::MemberCount;
   static constexpr ScalarType Half = ScalarType {1} / ScalarType {2};

   /// Triangle unique vertices                                               
   static constexpr Count VertexCount = 3;
   static constexpr PointType TriangleVertices[VertexCount] = {
      PointType { Half, Half, 0},
      PointType {-Half, Half, 0},
      PointType {    0,-Half, 0},
   };

   NOD() static bool Default(Construct&);
   NOD() static Construct Detail(const Mesh*, const LOD&);

   static void Indices(const Mesh*);
   static void Positions(const Mesh*);
   static void Normals(const Mesh*);
   static void TextureCoords(const Mesh*);
   static void Materials(const Mesh*);
   static void Instances(const Mesh*);
};

#define GENERATE() template<CT::Triangle T, CT::Topology TOPOLOGY> \
   void GenerateTriangle<T, TOPOLOGY>::


/// Default triangle generation                                               
///   @param descriptor - the descriptor to use                               
///   @return a newly generated descriptor, with missing traits being set to  
///           their defaults                                                  
template<CT::Triangle T, CT::Topology TOPOLOGY>
bool GenerateTriangle<T, TOPOLOGY>::Default(Construct& desc) {
   auto& d = desc.GetDescriptor();

   if constexpr (CT::Triangle<TOPOLOGY>) {
      // A solid triangles                                              
      d.SetDefaultTrait<Traits::Place>(
         MetaOf<TTriangle<PointType>>());
      d.SetDefaultTrait<Traits::Sampler>(
         MetaOf<Sampler2>());

      if constexpr (Dimensions >= 3) {
         d.SetDefaultTrait<Traits::Aim>(
            MetaOf<Normal>());
      }
   }
   else if constexpr (CT::Line<TOPOLOGY>) {
      // A triangle of lines                                            
      d.SetDefaultTrait<Traits::Place>(
         MetaOf<TLine<PointType>>());
   }
   else if constexpr (CT::Point<TOPOLOGY>) {
      // A triangle of points                                           
      d.SetDefaultTrait<Traits::Place>(
         MetaOf<PointType>());
   }
   else return false;

   d.SetDefaultTrait<Traits::Topology>(MetaOf<TOPOLOGY>());
   d.SetDefaultTrait<Traits::MapMode>(MapMode::Model);
   desc.SetType<A::Mesh>();
   return true;
}

/// Generate box level of detail, giving a LOD state                          
///   @param model - the box generator                                        
///   @param lod - the LOD state to generate                                  
///   @return a newly generated descriptor, for the LOD model you can use it  
///           to generate the new geometry                                    
template<CT::Triangle T, CT::Topology TOPOLOGY>
Construct GenerateTriangle<T, TOPOLOGY>::Detail(const Mesh* model, const LOD&) {
   return Construct::From<A::Mesh>(model->GetNeat());
}

/// Generate positions for triangle                                           
///   @param model - the model to fill                                        
GENERATE() Positions(const Mesh* model) {
   TMany<PointType> data = TriangleVertices;
   model->Commit<Traits::Place>(Abandon(data));
}

/// Generate indices for triangle                                             
///   @param model - the geometry instance to save data in                    
GENERATE() Indices(const Mesh* model) {
   TMany<uint32_t> data {0, 1, 2};
   model->Commit<Traits::Place>(Abandon(data));
}

/// Generate normals for triangle                                             
///   @param model - the geometry instance to save data in                    
GENERATE() Normals(const Mesh* model) {
   constexpr Normal n = Axes::Backward<ScalarType>;
   TMany<Normal> data;
   data.Reserve(VertexCount);
   for (auto& v : TriangleVertices)
      data << n;
   model->template Commit<Traits::Aim>(Abandon(data));
}

/// Generate texture coordinates for a box                                    
///   @param model - the geometry instance to save data in                    
GENERATE() TextureCoords(const Mesh* model) {
   TMany<Sampler2> data;
   data.Reserve(VertexCount);
   for (auto& v : TriangleVertices)
      data << v;
   model->template Commit<Traits::Aim>(Abandon(data));
}

GENERATE() Materials(const Mesh*) {
   TODO();
}

GENERATE() Instances(const Mesh*) {
   TODO();
}

#undef GENERATE