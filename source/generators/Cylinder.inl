///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "../Mesh.hpp"
#include <Math/Primitives/TCylinder.hpp>
#include <Math/Primitives/TTriangle.hpp>
#include <Math/Primitives/TLine.hpp>
#include <Math/Mapping.hpp>
#include <Math/Colors.hpp>


///                                                                           
///    Cylinder mesh generators                                               
///                                                                           
///   @tparam T - the primitve to use for point type and dimensions           
///   @tparam TOPOLOGY - are we generating triangles/lines/points?            
///                                                                           
template<CT::Cylinder T, CT::Topology TOPOLOGY = A::Triangle>
struct Generate {
   using PointType = typename T::PointType;
   static constexpr Count Dimensions = T::MemberCount;
   static_assert(Dimensions >= 3, "Cylinder should be at least 3D");

   NOD() static Normalized Default(Descriptor&&);
   NOD() static Normalized Detail(const Mesh*, const LOD&);

   static void Indices(Mesh*);
   static void Positions(Mesh*);
   static void Normals(Mesh*);
   static void TextureCoords(Mesh*);
   static void TextureIDs(Mesh*);
   static void Instances(Mesh*);
   static void Rotations(Mesh*);
   static void Colors(Mesh*);
};

#define GENERATE() template<CT::Cylinder T, CT::Topology TOPOLOGY> \
   void Generate<T, TOPOLOGY>::


/// Default cylinder generation                                               
///   @param descriptor - the descriptor to use                               
///   @return a newly generated descriptor, with missing traits being set to  
///           their defaults                                                  
template<CT::Cylinder T, CT::Topology TOPOLOGY>
Normalized Generate<T, TOPOLOGY>::Default(Descriptor&& descriptor) {
   Normalized d {descriptor};

   if constexpr (CT::Triangle<TOPOLOGY>) {
      // A cylinder made out of triangles                               
      d.SetDefaultTrait<Traits::MapMode>(
         MapMode::Cube);
      d.SetDefaultTrait<Traits::Topology>(
         MetaOf<TOPOLOGY>());
      d.SetDefaultTrait<Traits::Place>(
         MetaOf<TTriangle<PointType>>());
      d.SetDefaultTrait<Traits::Sampler>(
         MetaOf<Sampler2>());
      d.SetDefaultTrait<Traits::Aim>(
         MetaOf<Normal>());
   }
   else if constexpr (CT::Line<TOPOLOGY>) {
      // A cylinder made out of lines                                   
      d.SetDefaultTrait<Traits::Topology>(
         MetaOf<TOPOLOGY>());
      d.SetDefaultTrait<Traits::Place>(
         MetaOf<TLine<PointType>>());
   }
   else LANGULUS_ERROR("Unsupported topology for cylinder");

   return Abandon(d);
}

/// Generate cylinder level of detail, giving a LOD state                     
///   @param model - the cylinder generator                                   
///   @param lod - the LOD state to generate                                  
///   @return a newly generated descriptor, for the LOD model you can use it  
///           to generate the new geometry                                    
template<CT::Cylinder T, CT::Topology TOPOLOGY>
Normalized Generate<T, TOPOLOGY>::Detail(const Mesh* model, const LOD&) {
   return model->GetDescriptor();
}

/// Generate positions for a cylinder                                         
///   @param model - the model to fill                                        
GENERATE() Positions(Mesh* model) {
   if constexpr (CT::Triangle<TOPOLOGY>) {
      // A cylinder made out of triangles                               
      using E = TTriangle<PointType>;
      TAny<E> data;
      data.Reserve(TriangleCount);
      for (Offset i = 0; i < TriangleCount; ++i)
         data << E {Vertices, Indices[i]};
      model->Commit<Traits::Place>(Abandon(data));
   }
   else if constexpr (CT::Line<TOPOLOGY>) {
      // A cylinder made out of lines                                   
      LANGULUS_ERROR("TODO");
   }
   else LANGULUS_ERROR("Unsupported topology for cylinder positions");
}

/// Generate normals for a cylinder                                           
///   @param model - the geometry instance to save data in                    
GENERATE() Normals(Mesh* model) {
   static_assert(Dimensions >= 3,
      "Can't generate normals for cylinder of this many dimensions");

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
   else LANGULUS_ERROR("Unsupported topology for cylinder normals");
}

/// Generate indices for a cylinder                                           
///   @param model - the geometry instance to save data in                    
GENERATE() Indices(Mesh* model) {
   TAny<uint32_t> data;
   if constexpr (CT::Triangle<TOPOLOGY>) {
      // A cylinder made out of triangles                               
      data.Reserve(IndexCount);
      for (Offset i = 0; i < TriangleCount; ++i) {
         data << Indices[i][0];
         data << Indices[i][1];
         data << Indices[i][2];
      }
   }
   else if constexpr (CT::Line<TOPOLOGY>) {
      // A cylinder made out of lines                                   
      TODO();
   }
   else LANGULUS_ERROR("Unsupported topology for cylinder indices");

   model->template Commit<Traits::Index>(Abandon(data));
}

/// Generate texture coordinates for a cylinder                               
///   @param model - the geometry instance to save data in                    
GENERATE() TextureCoords(Mesh* model) {
   if constexpr (CT::Triangle<TOPOLOGY>) {
      if (model->GetTextureMapper() == MapMode::Mesh) {
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
   else LANGULUS_ERROR("Unsupported topology for cylinder texture coordinates");
}

GENERATE() TextureIDs(Mesh*) {
   TODO();
}

GENERATE() Instances(Mesh*) {
   TODO();
}

GENERATE() Rotations(Mesh*) {
   TODO();
}

GENERATE() Colors(Mesh* model) {
   TODO();
}

#undef GENERATE