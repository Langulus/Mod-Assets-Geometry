///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "../Model.hpp"
#include <Math/Primitives/TFrustum.hpp>
#include <Math/Primitives/TTriangle.hpp>
#include <Math/Primitives/TLine.hpp>
#include <Math/Mapping.hpp>
#include <Math/Colors.hpp>


///                                                                           
///    Frustum mesh generators                                                
///                                                                           
///   @tparam T - the primitve to use for point type and dimensions           
///   @tparam TOPOLOGY - are we generating triangles/lines/points?            
///                                                                           
template<CT::Frustum T, CT::Topology TOPOLOGY = A::Triangle>
struct Generate {
   using PointType = typename T::PointType;
   static constexpr Count Dimensions = T::MemberCount;

   NOD() static Normalized Default(Descriptor&&);
   NOD() static Normalized Detail(const Model*, const LOD&);

   static void Indices(Model*);
   static void Positions(Model*);
   static void Normals(Model*);
   static void TextureCoords(Model*);
   static void TextureIDs(Model*);
   static void Instances(Model*);
   static void Rotations(Model*);
   static void Colors(Model*);
};

#define GENERATE() template<CT::Frustum T, CT::Topology TOPOLOGY> \
   void Generate<T, TOPOLOGY>::


/// Default frustum generation                                                
///   @param descriptor - the descriptor to use                               
///   @return a newly generated descriptor, with missing traits being set to  
///           their defaults                                                  
template<CT::Frustum T, CT::Topology TOPOLOGY>
Normalized Generate<T, TOPOLOGY>::Default(Descriptor&& descriptor) {
   Normalized d {descriptor};
   d.SetDefaultTrait<Traits::MapMode>(MapMode::Cube);

   if constexpr (CT::Triangle<TOPOLOGY>) {
      // A frustum made out of triangles                                
      d.SetDefaultTrait<Traits::Topology>(
         MetaOf<TOPOLOGY>());
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
      // A frustum made out of lines                                    
      d.SetDefaultTrait<Traits::Topology>(
         MetaOf<TOPOLOGY>());
      d.SetDefaultTrait<Traits::Place>(
         MetaOf<TLine<PointType>>());
   }
   else LANGULUS_ERROR("Unsupported topology for frustum");

   return Abandon(d);
}

/// Generate frustum level of detail, giving a LOD state                      
///   @param model - the frustum generator                                    
///   @param lod - the LOD state to generate                                  
///   @return a newly generated descriptor, for the LOD model you can use it  
///           to generate the new geometry                                    
template<CT::Frustum T, CT::Topology TOPOLOGY>
Normalized Generate<T, TOPOLOGY>::Detail(const Model* model, const LOD&) {
   return model->GetDescriptor();
}

/// Generate positions for a frustum                                          
///   @param model - the model to fill                                        
GENERATE() Positions(Model* model) {
   if constexpr (CT::Triangle<TOPOLOGY>) {
      // A frustum made out of triangles                                
      using E = TTriangle<PointType>;
      TAny<E> data;
      data.Reserve(TriangleCount);
      for (Offset i = 0; i < TriangleCount; ++i)
         data << E {Vertices, Indices[i]};
      model->Commit<Traits::Place>(Abandon(data));
   }
   else if constexpr (CT::Line<TOPOLOGY>) {
      // A frustum made out of lines                                    
      LANGULUS_ERROR("TODO");
   }
   else LANGULUS_ERROR("Unsupported topology for frustum positions");
}

/// Generate normals for a frustum                                            
///   @param model - the geometry instance to save data in                    
GENERATE() Normals(Model* model) {
   static_assert(Dimensions >= 3,
      "Can't generate normals for frustum of this many dimensions");

   if constexpr (CT::Triangle<TOPOLOGY>) {
      constexpr Normal l {Cardinal::Left};
      constexpr Normal r {Cardinal::Right};
      constexpr Normal u {Cardinal::Up};
      constexpr Normal d {Cardinal::Down};
      constexpr Normal f {Cardinal::Forward};
      constexpr Normal b {Cardinal::Backward};

      TAny<Normal> data;
      data.Reserve(IndexCount);
      data << l << l << l << l << l << l;
      data << r << r << r << r << r << r;
      data << u << u << u << u << u << u;
      data << d << d << d << d << d << d;
      data << f << f << f << f << f << f;
      data << b << b << b << b << b << b;

      model->template Commit<Traits::Aim>(Abandon(data));
   }
   else LANGULUS_ERROR("Unsupported topology for frustum normals");
}

/// Generate indices for a frustum                                            
///   @param model - the geometry instance to save data in                    
GENERATE() Indices(Model* model) {
   TAny<uint32_t> data;
   if constexpr (CT::Triangle<TOPOLOGY>) {
      // A frustum made out of triangles                                
      data.Reserve(IndexCount);
      for (Offset i = 0; i < TriangleCount; ++i) {
         data << Indices[i][0];
         data << Indices[i][1];
         data << Indices[i][2];
      }
   }
   else if constexpr (CT::Line<TOPOLOGY>) {
      // A frustum made out of lines                                    
      TODO();
   }
   else LANGULUS_ERROR("Unsupported topology for frustum indices");

   model->template Commit<Traits::Index>(Abandon(data));
}

/// Generate texture coordinates for a frustum                                
///   @param model - the geometry instance to save data in                    
GENERATE() TextureCoords(Model* model) {
   if constexpr (CT::Triangle<TOPOLOGY>) {
      if (model->GetTextureMapper() == MapMode::Model) {
         // Generate model mapping                                      
         TAny<Sampler3> data;
         data.Reserve(IndexCount);
         for (Offset i = 0; i < TriangleCount; ++i) {
            data << Vertices[Indices[i][0]] + Half;
            data << Vertices[Indices[i][1]] + Half;
            data << Vertices[Indices[i][2]] + Half;
         }

         model->template Commit<Traits::Sampler>(Abandon(data));
      }
      else if (model->GetTextureMapper() == MapMode::Face) {
         // Generate face mapping                                       
         TAny<Sampler2> data;
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
   else LANGULUS_ERROR("Unsupported topology for frustum texture coordinates");
}

GENERATE() TextureIDs(Model*) {
   TODO();
}

GENERATE() Instances(Model*) {
   TODO();
}

GENERATE() Rotations(Model*) {
   TODO();
}

GENERATE() Colors(Model* model) {
   TODO();
}

#undef GENERATE