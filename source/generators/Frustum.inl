///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "../Mesh.hpp"
#include <Math/Primitives/TFrustum.hpp>
#include <Math/Primitives/TTriangle.hpp>
#include <Math/Primitives/TLine.hpp>
#include <Math/Mapping.hpp>
#include <Math/Colors.hpp>


///                                                                           
///    Frustum mesh generators                                                
///                                                                           
///   @tparam T - the primitive to use for point type and dimensions          
///   @tparam TOPOLOGY - are we generating triangles/lines/points?            
///                                                                           
template<CT::Frustum T, CT::Topology TOPOLOGY = A::Triangle>
struct GenerateFrustum {
   using PointType = typename T::PointType;
   using ScalarType = TypeOf<PointType>;
   static constexpr Count Dimensions = T::MemberCount;
   static constexpr ScalarType Half = ScalarType {1} / ScalarType {2};

   template<Count DIMENSIONS>
   struct Constants;

   /// Properties for a 3D box                                                
   template<> struct Constants<3> {
      static constexpr Count VertexCount = 8;
      static constexpr Count TriangleCount = 12;
      static constexpr Count IndexCount = TriangleCount * 3;
      static constexpr Count FaceCount = TriangleCount / 2;

      /// 3D box unique vertices                                              
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
      static constexpr uint32_t Indices[TriangleCount][3] = {
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
   };
   
   /// Properties for a 2D box (rectangle)                                    
   template<> struct Constants<2> {
      ///                                                                     
      ///  3     2                                                            
      ///   +---+         Each corner is at distance 0.5 from center by       
      ///   |  /|         default. The vertices are in the XY plane by        
      ///   | + |         default (Z=0)                                       
      ///   |/  |                                                             
      ///   +---+                                                             
      ///  1     0                                                            
      ///                                                                     
      static constexpr Count VertexCount = 4;
      static constexpr Count TriangleCount = 2;
      static constexpr Count LineCount = 4;
      static constexpr Count IndexCount = TriangleCount * 3;
      static constexpr Count FaceCount = TriangleCount / 2;

      /// 3D box unique vertices                                              
      static constexpr PointType Vertices[VertexCount] = {
         PointType { Half,  Half, 0},     // Bottom-right corner        
         PointType {-Half,  Half, 0},     // Bottom-left corner         
         PointType { Half, -Half, 0},     // Top-right corner           
         PointType {-Half, -Half, 0}      // Top-left corner            
      };

      /// Face mapping                                                        
      static constexpr Sampler2 FaceMapping[VertexCount] = {
         Sampler2 {0, 0},
         Sampler2 {0, 1},
         Sampler2 {1, 0},
         Sampler2 {1, 1}
      };

      /// Indices for the 12 box triangles                                    
      static constexpr uint32_t Indices[TriangleCount][3] = {
         {0,2,1}, {2,3,1},
      };

      /// Indices for the rect lines                                          
      static constexpr uint32_t LineIndices[LineCount][2] = {
         {0,1}, {1,3}, {3,2}, {2,0},
      };
   };

   using D = Constants<Dimensions>;

   NOD() static Construct Default(Neat&&);
   NOD() static Construct Detail(const Mesh*, const LOD&);

   static void Indices(Mesh*);
   static void Positions(Mesh*);
   static void Normals(Mesh*);
   static void TextureCoords(Mesh*);
   static void Materials(Mesh*);
   static void Instances(Mesh*);
};

#define GENERATE() template<CT::Frustum T, CT::Topology TOPOLOGY> \
   void GenerateFrustum<T, TOPOLOGY>::


/// Default frustum generation                                                
///   @param descriptor - the descriptor to use                               
///   @return a newly generated descriptor, with missing traits being set to  
///           their defaults                                                  
template<CT::Frustum T, CT::Topology TOPOLOGY>
Construct GenerateFrustum<T, TOPOLOGY>::Default(Neat&& descriptor) {
   Construct d {descriptor};
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
Construct GenerateFrustum<T, TOPOLOGY>::Detail(const Mesh* model, const LOD&) {
   return model->GetNeat();
}

/// Generate positions for a frustum                                          
///   @param model - the model to fill                                        
GENERATE() Positions(Mesh* model) {
   if constexpr (CT::Triangle<TOPOLOGY>) {
      // A box (3D) or rectangle (2D) made out of triangles             
      using E = TTriangle<PointType>;
      TAny<E> data;
      data.Reserve(D::TriangleCount);
      for (Offset i = 0; i < D::TriangleCount; ++i)
         data << E {D::Vertices, D::Indices[i]};
      model->Commit<Traits::Place>(Abandon(data));
   }
   else if constexpr (CT::Line<TOPOLOGY>) {
      // A box (3D) or rectangle (2D) made out of lines                 
      using E = TLine<PointType>;
      TAny<E> data;
      data.Reserve(D::LineCount);
      for (Offset i = 0; i < D::LineCount; ++i)
         data << E {D::Vertices, D::LineIndices[i]};
      model->Commit<Traits::Place>(Abandon(data));
   }
   else if constexpr (CT::Point<TOPOLOGY>) {
      // A box (3D) or rectangle (2D) made out of points                
      TAny<PointType> data = D::Vertices;
      model->Commit<Traits::Place>(Abandon(data));
   }
   else LANGULUS_ERROR("Unsupported topology for box positions");
}

/// Generate normals for a frustum                                            
///   @param model - the geometry instance to save data in                    
GENERATE() Normals(Mesh* model) {
   if constexpr (CT::Triangle<TOPOLOGY>) {
      constexpr Normal l {Cardinal::Left};
      constexpr Normal r {Cardinal::Right};
      constexpr Normal u {Cardinal::Up};
      constexpr Normal d {Cardinal::Down};
      constexpr Normal f {Cardinal::Forward};
      constexpr Normal b {Cardinal::Backward};

      TAny<Normal> data;
      data.Reserve(D::IndexCount);
      if constexpr (Dimensions == 3) {
         // Normals for a 3D box                                        
         data << l << l << l << l << l << l;
         data << r << r << r << r << r << r;
         data << u << u << u << u << u << u;
         data << d << d << d << d << d << d;
         data << f << f << f << f << f << f;
         data << b << b << b << b << b << b;
      }
      else if constexpr (Dimensions == 2) {
         // Normals for a 2D rect, always facing the user (-Z)          
         for (Offset i = 0; i < D::IndexCount; ++i)
            data << b;
      }

      model->template Commit<Traits::Aim>(Abandon(data));
   }
   else LANGULUS_ERROR("Unsupported topology for box normals");
}

/// Generate indices for a frustum                                            
///   @param model - the geometry instance to save data in                    
GENERATE() Indices(Mesh* model) {
   TAny<uint32_t> data;
   if constexpr (CT::Triangle<TOPOLOGY>) {
      // A box made out of triangles                                    
      data.Reserve(D::IndexCount);
      for (Offset i = 0; i < D::TriangleCount; ++i) {
         data << D::Indices[i][0];
         data << D::Indices[i][1];
         data << D::Indices[i][2];
      }
   }
   else if constexpr (CT::Line<TOPOLOGY>) {
      // A box made out of lines                                        
      TODO();
   }
   else if constexpr (CT::Point<TOPOLOGY>) {
      // A box made out of points                                       
      LANGULUS_ERROR("TODO");
   }
   else LANGULUS_ERROR("Unsupported topology for box indices");

   model->template Commit<Traits::Index>(Abandon(data));
}

/// Generate texture coordinates for a frustum                                
///   @param model - the geometry instance to save data in                    
GENERATE() TextureCoords(Mesh* model) {
   if constexpr (CT::Triangle<TOPOLOGY>) {
      if (model->GetTextureMapper() == MapMode::Model) {
         // Generate model mapping                                      
         TAny<Sampler3> data;
         data.Reserve(D::IndexCount);
         for (Offset i = 0; i < D::TriangleCount; ++i) {
            data << D::Vertices[D::Indices[i][0]] + Half;
            data << D::Vertices[D::Indices[i][1]] + Half;
            data << D::Vertices[D::Indices[i][2]] + Half;
         }

         model->template Commit<Traits::Sampler>(Abandon(data));
      }
      else if (model->GetTextureMapper() == MapMode::Face) {
         // Generate face mapping                                       
         TAny<Sampler2> data;
         data.Reserve(D::IndexCount);
         for (Offset i = 0; i < D::IndexCount; ++i)
            data << D::FaceMapping[i % (D::IndexCount / D::FaceCount)];

         model->template Commit<Traits::Sampler>(Abandon(data));
      }
      else TODO();
   }
   else if constexpr (CT::Line<TOPOLOGY>) {
      TODO();
   }
   else if constexpr (CT::Point<TOPOLOGY>) {
      TODO();
   }
   else LANGULUS_ERROR("Unsupported topology for box texture coordinates");
}

GENERATE() Materials(Mesh*) {
   TODO();
}

GENERATE() Instances(Mesh*) {
   TODO();
}

#undef GENERATE