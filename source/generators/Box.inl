///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "../Mesh.hpp"
#include <Math/Primitives/TBox.hpp>
#include <Math/Primitives/TTriangle.hpp>
#include <Math/Primitives/TLine.hpp>
#include <Math/Mapping.hpp>
#include <Math/Colors.hpp>


namespace GeometryBox
{

   /// Box's constant occurences                                              
   constexpr Count VertexCount = 8;
   constexpr Count TriangleCount = 12;
   constexpr Count IndexCount = TriangleCount * 3;
   constexpr Count FaceCount = TriangleCount / 2;

   /// Box's unique vertices                                                  
   constexpr Point3 Vertices[VertexCount] = {
      // Left face (-X)                                                 
      Point3 {-Half<>, -Half<>,  Half<>},
      Point3 {-Half<>, -Half<>, -Half<>},
      Point3 {-Half<>,  Half<>,  Half<>},
      Point3 {-Half<>,  Half<>, -Half<>},
      // Right face (+X)                                                
      Point3 { Half<>, -Half<>, -Half<>},
      Point3 { Half<>, -Half<>,  Half<>},
      Point3 { Half<>,  Half<>,  Half<>},
      Point3 { Half<>,  Half<>, -Half<>}
   };

   /// Face mapping                                                           
   constexpr Sampler2 FaceMapping[FaceCount] = {
      Sampler2 {0, 0}, Sampler2 {0, 1}, Sampler2 {1, 0},
      Sampler2 {1, 0}, Sampler2 {0, 1}, Sampler2 {1, 1}
   };

   /// Indices for the 12 box triangles                                       
   constexpr uint32_t Indices[TriangleCount][3] = {
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

} // namespace Geometry::Box


///                                                                           
///    Box mesh generators                                                    
///                                                                           
///   @tparam T - the box primitve to use for point type and dimensions       
///   @tparam TOPOLOGY - are we generating triangles/lines/points?            
///                                                                           
template<CT::Box T, CT::Topology TOPOLOGY = A::Triangle>
struct Generate {
   using PointType = typename T::PointType;
   static constexpr Count Dimensions = T::MemberCount;

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

#define GENERATE() template<CT::Box T, CT::Topology TOPOLOGY> \
   void Generate<T, TOPOLOGY>::


/// Default box generation                                                    
///   @param descriptor - the descriptor to use                               
///   @return a newly generated descriptor, with missing traits being set to  
///           their defaults                                                  
template<CT::Box T, CT::Topology TOPOLOGY>
Normalized Generate<T, TOPOLOGY>::Default(Descriptor&& descriptor) {
   Normalized d {descriptor};
   d.SetDefaultTrait<Traits::MapMode>(MapMode::Cube);

   if constexpr (CT::Triangle<TOPOLOGY>) {
      // A box made out of triangles                                    
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
      // A box made out of lines                                        
      d.SetDefaultTrait<Traits::Topology>(
         MetaOf<TOPOLOGY>());
      d.SetDefaultTrait<Traits::Place>(
         MetaOf<TLine<PointType>>());
   }
   else if constexpr (CT::Point<TOPOLOGY>) {
      // A box made out of points                                       
      d.SetDefaultTrait<Traits::Topology>(
         MetaOf<TOPOLOGY>());
      d.SetDefaultTrait<Traits::Place>(
         MetaOf<PointType>());
   }
   else LANGULUS_ERROR("Unsupported topology for box");

   return Abandon(d);
}

/// Generate box level of detail, giving a LOD state                          
///   @param model - the box generator                                        
///   @param lod - the LOD state to generate                                  
///   @return a newly generated descriptor, for the LOD model you can use it  
///           to generate the new geometry                                    
template<CT::Box T, CT::Topology TOPOLOGY>
Normalized Generate<T, TOPOLOGY>::Detail(const Mesh* model, const LOD&) {
   return model->GetDescriptor();
}

/// Generate positions for a box                                              
///   @param model - the model to fill                                        
GENERATE() Positions(Mesh* model) {
   using namespace GeometryBox;

   if constexpr (CT::Triangle<TOPOLOGY>) {
      // A box made out of triangles                                    
      using E = TTriangle<PointType>;
      TAny<E> data;
      data.Reserve(TriangleCount);
      for (Offset i = 0; i < TriangleCount; ++i)
         data << E {Vertices, Indices[i]};
      model->Commit<Traits::Place>(Abandon(data));
   }
   else if constexpr (CT::Line<TOPOLOGY>) {
      // A box made out of lines                                        
      LANGULUS_ERROR("TODO");
   }
   else if constexpr (CT::Point<TOPOLOGY>) {
      // A box made out of points                                       
      LANGULUS_ERROR("TODO");
   }
   else LANGULUS_ERROR("Unsupported topology for box positions");
}

/// Generate normals for a box                                                
///   @param model - the geometry instance to save data in                    
GENERATE() Normals(Mesh* model) {
   using namespace GeometryBox;
   static_assert(Dimensions >= 3,
      "Can't generate normals for box of this many dimensions");

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
   else LANGULUS_ERROR("Unsupported topology for box normals");
}

/// Generate indices for a box                                                
///   @param model - the geometry instance to save data in                    
GENERATE() Indices(Mesh* model) {
   using namespace GeometryBox;

   TAny<uint32_t> data;
   if constexpr (CT::Triangle<TOPOLOGY>) {
      // A box made out of triangles                                    
      data.Reserve(IndexCount);
      for (Offset i = 0; i < TriangleCount; ++i) {
         data << Indices[i][0];
         data << Indices[i][1];
         data << Indices[i][2];
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

/// Generate texture coordinates for a box                                    
///   @param model - the geometry instance to save data in                    
GENERATE() TextureCoords(Mesh* model) {
   using namespace GeometryBox;

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
   else if constexpr (CT::Point<TOPOLOGY>) {
      // A box made out of points                                       
      LANGULUS_ERROR("TODO");
   }
   else LANGULUS_ERROR("Unsupported topology for box texture coordinates");
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
   using namespace GeometryBox;

   if constexpr (CT::Triangle<TOPOLOGY>) {
      // A cube made out of triangles                                   
      TAny<RGB> data;
      data.Reserve(IndexCount);

      data << RGB {64,  64,   64};
      data << RGB {64,  64,   255};
      data << RGB {64,  255,  255};

      data << RGB {255, 255,  64};
      data << RGB {64,  64,   64};
      data << RGB {64,  255,  64};

      data << RGB {255, 64,   255};
      data << RGB {64,  64,   64};
      data << RGB {255, 64,   64};

      data << RGB {255, 255,  64};
      data << RGB {255, 64,   64};
      data << RGB {64,  64,   64};

      data << RGB {64,  64,   64};
      data << RGB {64,  255,  255};
      data << RGB {64,  255,  64};

      data << RGB {255, 64,   255};
      data << RGB {64,  64,   255};
      data << RGB {64,  64,   64};

      data << RGB {64,  255,  255};
      data << RGB {64,  64,   255};
      data << RGB {255, 64,   255};

      data << RGB {255, 255,  255};
      data << RGB {255, 64,   64};
      data << RGB {255, 255,  64};

      data << RGB {255, 64,   64};
      data << RGB {255, 255,  255};
      data << RGB {255, 64,   255};

      data << RGB {255, 255,  255};
      data << RGB {255, 255,  64};
      data << RGB {64,  255,  64};

      data << RGB {255, 255,  255};
      data << RGB {64,  255,  64};
      data << RGB {64,  255,  255};

      data << RGB {255, 255,  255};
      data << RGB {64,  255,  255};
      data << RGB {255, 64,   255};

      model->template Commit<Traits::Color>(Abandon(data));
   }
   else if constexpr (CT::Line<TOPOLOGY>) {
      // A cube made out of lines                                       
      TODO();
   }
   else if constexpr (CT::Point<TOPOLOGY>) {
      // A cube made out of points                                      
      TODO();
   }
   else LANGULUS_ERROR("Unsupported topology for box colors");
}

#undef GENERATE