///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright (c) 2016 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Mesh.hpp"
#include <Langulus/Math/Primitives/Cylinder.hpp>
#include <Langulus/Math/Primitives/Triangle.hpp>
#include <Langulus/Math/Primitives/Line.hpp>
#include <Langulus/Math/Mapping.hpp>
#include <Langulus/Math/Color.hpp>


///                                                                           
///    Cylinder mesh generators                                               
///                                                                           
///   @tparam T - the primitive to use for point type and dimensions          
///   @tparam TOPOLOGY - are we generating triangles/lines/points?            
///                                                                           
template<CT::Cylinder T, CT::Topology TOPOLOGY = A::Triangle>
struct GenerateCylinder {
   using PointType = typename T::PointType;
   using ScalarType = TypeOf<PointType>;
   static constexpr Count Dimensions = T::MemberCount;
   static constexpr ScalarType Half = ScalarType {1} / ScalarType {2};

   static_assert(Dimensions >= 3, "Cylinder should be at least 3D");

   static constexpr Count VertexCount = 8;
   static constexpr Count TriangleCount = 12;
   static constexpr Count IndexCount = TriangleCount * 3;
   static constexpr Count FaceCount = TriangleCount / 2;

   /// 3D cylinder unique vertices                                            
   static constexpr PointType Vertices[VertexCount] = {
      // Left face (-X)                                                 
      PointType {-Half, -Half,  Half},
      PointType {-Half, -Half, -Half},
      PointType {-Half,  Half,  Half},
      PointType {-Half,  Half, -Half},
      // Right face (+X)                                                
      PointType { Half, -Half, -Half},
      PointType { Half, -Half,  Half},
      PointType { Half,  Half,  Half},
      PointType { Half,  Half, -Half}
   };

   /// Face mapping                                                           
   static constexpr Sampler2 FaceMapping[FaceCount] = {
      Sampler2 {0, 0}, Sampler2 {0, 1}, Sampler2 {1, 0},
      Sampler2 {1, 0}, Sampler2 {0, 1}, Sampler2 {1, 1}
   };

   /// Indices for the 12 box triangles                                       
   static constexpr uint32_t TriangleIndices[TriangleCount][3] = {
      // Left face                                                      
      {0,1,2},  {2,1,3},
      // Right face                                                     
      {4,5,6},  {4,6,7},
      // Top face                                                       
      {2,3,6},  {6,3,7},
      // Bottom face                                                    
      {1,0,5},  {1,5,4},
      // Forward face                                                   
      {2,6,5},  {2,5,0},
      // Backward face                                                  
      {7,3,4},  {4,3,1}
   };

   static bool Default(Construct&);
   static auto Detail(const Mesh*, const LOD&) -> Construct;

   static void Indices(Mesh*);
   static void Positions(Mesh*);
   static void Normals(Mesh*);
   static void TextureCoords(Mesh*);
   static void Materials(Mesh*);
};

#define GENERATE() template<CT::Cylinder T, CT::Topology TOPOLOGY> \
   void GenerateCylinder<T, TOPOLOGY>::


/// Default cylinder generation                                               
///   @param descriptor - the descriptor to use                               
///   @return a newly generated descriptor, with missing traits being set to  
///           their defaults                                                  
template<CT::Cylinder T, CT::Topology TOPOLOGY>
bool GenerateCylinder<T, TOPOLOGY>::Default(Construct& desc) {
   auto& d = desc.GetDescriptor();

   if constexpr (CT::Triangle<TOPOLOGY>) {
      // A cylinder made out of triangles                               
      d.SetDefaultTrait<Traits::MapMode>(
         MapMode::Cube);
      d.SetDefaultTrait<Traits::Place>(
         MetaOf<TTriangle<PointType>>());
      d.SetDefaultTrait<Traits::Sampler>(
         MetaOf<Sampler2>());
      d.SetDefaultTrait<Traits::Aim>(
         MetaOf<Normal>());
   }
   else if constexpr (CT::Line<TOPOLOGY>) {
      // A cylinder made out of lines                                   
      d.SetDefaultTrait<Traits::Place>(
         MetaOf<TLine<PointType>>());
   }
   else return false;

   d.SetDefaultTrait<Traits::Topology>(MetaOf<TOPOLOGY>());
   desc.SetType<A::Mesh>();
   return true;
}

/// Generate cylinder level of detail, giving a LOD state                     
///   @param model - the cylinder generator                                   
///   @param lod - the LOD state to generate                                  
///   @return a newly generated descriptor, for the LOD model you can use it  
///           to generate the new geometry                                    
template<CT::Cylinder T, CT::Topology TOPOLOGY>
auto GenerateCylinder<T, TOPOLOGY>::Detail(const Mesh* model, const LOD&) -> Construct {
   return Construct::From<A::Mesh>(model->GetDescriptor());
}

/// Generate positions for a cylinder                                         
///   @param model - the model to fill                                        
GENERATE() Positions(Mesh* model) {
   if constexpr (CT::Triangle<TOPOLOGY>) {
      // A cylinder made out of triangles                               
      using E = TTriangle<PointType>;
      TMany<E> data;
      data.Reserve(TriangleCount);
      for (Offset i = 0; i < TriangleCount; ++i)
         data << E {Vertices, TriangleIndices[i]};
      model->Commit<Traits::Place>(Abandon(data));
   }
   else if constexpr (CT::Line<TOPOLOGY>) {
      // A cylinder made out of lines                                   
      static_assert(false, "TODO");
   }
   else static_assert(false, "Unsupported topology for cylinder positions");
}

/// Generate normals for a cylinder                                           
///   @param model - the geometry instance to save data in                    
GENERATE() Normals(Mesh* model) {
   static_assert(Dimensions >= 3,
      "Can't generate normals for cylinder of this many dimensions");

   if constexpr (CT::Triangle<TOPOLOGY>) {
      constexpr Normal l {Axes::Left<ScalarType>};
      constexpr Normal r {Axes::Right<ScalarType>};
      constexpr Normal u {Axes::Up<ScalarType>};
      constexpr Normal d {Axes::Down<ScalarType>};
      constexpr Normal f {Axes::Forward<ScalarType>};
      constexpr Normal b {Axes::Backward<ScalarType>};

      TMany<Normal> data;
      data.Reserve(IndexCount);
      data << l << l << l << l << l << l;
      data << r << r << r << r << r << r;
      data << u << u << u << u << u << u;
      data << d << d << d << d << d << d;
      data << f << f << f << f << f << f;
      data << b << b << b << b << b << b;

      model->template Commit<Traits::Aim>(Abandon(data));
   }
   else static_assert(false, "Unsupported topology for cylinder normals");
}

/// Generate indices for a cylinder                                           
///   @param model - the geometry instance to save data in                    
GENERATE() Indices(Mesh* model) {
   TMany<uint32_t> data;
   if constexpr (CT::Triangle<TOPOLOGY>) {
      // A cylinder made out of triangles                               
      data.Reserve(IndexCount);
      for (Offset i = 0; i < TriangleCount; ++i) {
         data << TriangleIndices[i][0];
         data << TriangleIndices[i][1];
         data << TriangleIndices[i][2];
      }
   }
   else if constexpr (CT::Line<TOPOLOGY>) {
      // A cylinder made out of lines                                   
      TODO();
   }
   else static_assert(false, "Unsupported topology for cylinder indices");

   model->template Commit<Traits::Index>(Abandon(data));
}

/// Generate texture coordinates for a cylinder                               
///   @param model - the geometry instance to save data in                    
GENERATE() TextureCoords(Mesh* model) {
   if constexpr (CT::Triangle<TOPOLOGY>) {
      if (model->GetTextureMapper() == MapMode::Model) {
         // Generate model mapping                                      
         TMany<Sampler3> data;
         data.Reserve(IndexCount);
         for (Offset i = 0; i < TriangleCount; ++i) {
            data << Vertices[TriangleIndices[i][0]] + Half;
            data << Vertices[TriangleIndices[i][1]] + Half;
            data << Vertices[TriangleIndices[i][2]] + Half;
         }

         model->template Commit<Traits::Sampler>(Abandon(data));
      }
      else if (model->GetTextureMapper() == MapMode::Face) {
         // Generate face mapping                                       
         TMany<Sampler2> data;
         data.Reserve(IndexCount);
         for (Offset i = 0; i < IndexCount; ++i)
            data << FaceMapping[i % (IndexCount / FaceCount)];

         model->template Commit<Traits::Sampler>(Abandon(data));
      }
      else TODO();
   }
   else if constexpr (CT::Line<TOPOLOGY>) {
      TODO();
   }
   else static_assert(false, "Unsupported topology for cylinder texture coordinates");
}

GENERATE() Materials(Mesh*) {
   TODO();
}

#undef GENERATE