///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "../Mesh.hpp"
#include "Zode.inl"
#include <Math/Primitives/Sphere.hpp>
#include <Math/Primitives/Triangle.hpp>
#include <Math/Primitives/Line.hpp>
#include <Math/Mapping.hpp>
#include <Math/Color.hpp>


///                                                                           
///    Sphere mesh generators                                                 
///                                                                           
///   @tparam T - the primitive to use for point type and dimensions          
///   @tparam TOPOLOGY - are we generating triangles/lines/points?            
///                                                                           
template<CT::Sphere T, CT::Topology TOPOLOGY = A::Triangle>
struct GenerateSphere {
   using PointType = typename T::PointType;
   using ScalarType = TypeOf<PointType>;
   static constexpr Count Dimensions = T::MemberCount;
   static constexpr ScalarType Half = ScalarType {1} / ScalarType {2};

   static_assert(Dimensions >= 2, "Sphere should be at least 2D");
   
   /// Properties for a 3D sphere                                             
   struct Constants3D {
      static constexpr Count VertexCount = 12;
      static constexpr Count TriangleCount = 20;
      static constexpr Count IndexCount = TriangleCount * 3;

      static constexpr ScalarType IcosahedronX = 
         ScalarType {1} / (ScalarType {2} * Sqrt(ScalarType {5}));
      static constexpr ScalarType IcosahedronZ = 
         ScalarType {2} / (ScalarType {2} * Sqrt(ScalarType {5}));
      
      /// Icosahedron's unique vertices                                       
      static constexpr PointType Vertices[VertexCount] = {
         { -IcosahedronX,             0,  IcosahedronZ },
         {  IcosahedronX,             0,  IcosahedronZ },
         { -IcosahedronX,             0, -IcosahedronZ },
         {  IcosahedronX,             0, -IcosahedronZ },
         {             0,  IcosahedronZ,  IcosahedronX },
         {             0,  IcosahedronZ, -IcosahedronX },
         {             0, -IcosahedronZ,  IcosahedronX },
         {             0, -IcosahedronZ, -IcosahedronX },
         {  IcosahedronZ,  IcosahedronX,             0 },
         { -IcosahedronZ,  IcosahedronX,             0 },
         {  IcosahedronZ, -IcosahedronX,             0 },
         { -IcosahedronZ, -IcosahedronX,             0 }
      };

      /// Indices for the 20 icosahedron faces                                
      static constexpr uint32_t Indices[TriangleCount][3] = {
         {0,4,1},  {0,9,4},  {9,5,4},  {4,5,8},  {4,8,1},
         {8,10,1}, {8,3,10}, {5,3,8},  {5,2,3},  {2,7,3},
         {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6},
         {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5},  {7,2,11}
      };
   };
   
   /// Properties for a 2D circle                                             
   struct Constants2D {
      //TODO
   };

   using D = Conditional<Dimensions == 2, Constants2D, Constants3D>;

   NOD() static Construct Default(Neat&&);
   NOD() static Construct Detail(const Mesh*, const LOD&);

   static void Indices(Mesh*);
   static void Positions(Mesh*);
   static void Normals(Mesh*);
   static void TextureCoords(Mesh*);
   static void Materials(Mesh*);
   static void Instances(Mesh*);
};

#define GENERATE() template<CT::Sphere T, CT::Topology TOPOLOGY> \
   void GenerateSphere<T, TOPOLOGY>::


/// Default sphere generation                                                 
///   @param descriptor - the descriptor to use                               
///   @return a newly generated descriptor, with missing traits being set to  
///           their defaults                                                  
template<CT::Sphere T, CT::Topology TOPOLOGY>
Construct GenerateSphere<T, TOPOLOGY>::Default(Neat&& descriptor) {
   auto d = Forward<Neat>(descriptor);
   d.SetDefaultTrait<Traits::MapMode>(MapMode::Cube);

   if constexpr (CT::Triangle<TOPOLOGY>) {
      // A sphere made out of triangles                                 
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
      // A sphere made out of lines                                     
      d.SetDefaultTrait<Traits::Topology>(
         MetaOf<TOPOLOGY>());
      d.SetDefaultTrait<Traits::Place>(
         MetaOf<TLine<PointType>>());
   }
   else if constexpr (CT::Point<TOPOLOGY>) {
      // A sphere made out of points                                    
      d.SetDefaultTrait<Traits::Topology>(
         MetaOf<TOPOLOGY>());
      d.SetDefaultTrait<Traits::Place>(
         MetaOf<PointType>());
   }
   else LANGULUS_ERROR("Unsupported topology for box");

   return Construct::From<A::Mesh>(Abandon(d));
}

/// Generate box level of detail, giving a LOD state                          
///   @param model - the box generator                                        
///   @param lod - the LOD state to generate                                  
///   @return a newly generated descriptor, you can use it to generate the    
///           required level-of-detail geometry                               
template<CT::Sphere T, CT::Topology TOPOLOGY>
Construct GenerateSphere<T, TOPOLOGY>::Detail(const Mesh* model, const LOD& lod) {
   if (lod.mLODIndex == 0) {
      // On zero LOD index, we're at optimal distance, so we return     
      // the original unmodified geometry                               
      return Construct::From<A::Mesh>(model->GetNeat());
   }

   unsigned tesselation = 0;
   model->GetNeat().ExtractTrait<Traits::Tesselation>(tesselation);
   if (tesselation > 0 and lod.mLODIndex < 0) {
      // Find a lower tesselation of the geosphere, because the LOD     
      // is for an object that is further away                          
      auto newTesselation = tesselation + lod.mLODIndex;
      if (newTesselation < 0)
         newTesselation = 0;

      // Create the LOD descriptor, based on the current one            
      auto newMesh = model->GetNeat();
      newMesh.Set(Traits::Tesselation {std::round(newTesselation)});
      return Construct::From<A::Mesh>(Abandon(newMesh));
   }
   else if (lod.mLODIndex > 0) {
      // LOD is for a sphere that is very close. At this point the      
      // sphere turns into a plane to the observer. For optimization    
      // purposes, this plane is made of four triangular patches        
      // that are centered on the camera, and facing outwards the       
      // sphere. These so called 'zodes' are designed to be indexed     
      // from the center towards the edges and are optimized            
      // especially for planet & convex terrain rendering               

      // Calculate the angle between the tangents to the sphere         
      const auto cosTheta = 1 / (1 + lod.GetNormalizedDistance());

      // Calculate the visible spherical cap area, and the required     
      // number of such areas to cover the whole sphere                 
      const auto taurr = TAU<ScalarType> * lod.mRadius * lod.mRadius;
      const auto area = 2 * taurr * (ScalarType {1} - cosTheta);
      const auto areaFraction = Abs(area / taurr) * 2;
      const auto areasToCover = Ceil(1 / areaFraction);

      // Make a stepping normal that is somewhat snapped to the         
      // theta this way we will avoid generating content on each        
      // small reorientation                                            
      const auto intersection = lod.mView.GetPosition() - lod.mModel.GetPosition();
      if (intersection.Length() == 0)
         return Construct::From<A::Mesh>(model->GetNeat());

      const auto steppingNormal = (intersection.Normalize() * areasToCover).Round();

      // Clone only the descriptor and change type                      
      auto newMesh = Construct::From<A::Mesh>(TZode<PointType> {});
      // Set the place around which the zode is centered                
      newMesh.Set(Traits::Place {steppingNormal});
      // Set the size of the sphere in order to calculate curvature     
      newMesh.Set(Traits::Size {lod.mModel.GetScale()});
      // Set the topology                                               
      newMesh.Set(Traits::Topology {TypeOf<A::TriangleStrip>()});
      // Set the tesselation                                            
      newMesh.Set(Traits::Tesselation {tesselation + static_cast<unsigned>(lod.mLODIndex) / 2});

      // The octave gets progressively lower until we reach state       
      //real octave = 0;
      //newContent.Value(TraitID_Octave::Meta(), octave);
      //octave = pcLerp(octave, real(lod.mOctave), lodIndex / real(6));
      //newContent.SetTrait<TraitID_Octave>(pcptr(octave));
      return Abandon(newMesh);
   }

   return Construct::From<A::Mesh>(model->GetNeat());
}

/// Generate positions for a sphere/circle                                    
///   @param model - the model to fill                                        
GENERATE() Positions(Mesh* model) {
   // A geosphere/circle made of triangles/lines/points                 
   // This geometry is always indexes, so all positions go              
   TAny<PointType> data = D::Vertices;
   model->Commit<Traits::Place>(Abandon(data));
}

/// Generate normals for a sphere/circle                                      
///   @param model - the geometry instance to save data in                    
GENERATE() Normals(Mesh* model) {
   if constexpr (CT::Triangle<TOPOLOGY>) {
      TAny<Normal> data;
      data.Reserve(D::VertexCount);

      if constexpr (Dimensions >= 3) {
         // Normals for a 3D sphere - shared for each point             
         for (auto& v : D::Vertices)
            data << Normal {v};
      }
      else if constexpr (Dimensions == 2) {
         // Normals for a 2D circle, always facing the user (-Z)        
         constexpr Normal b {Axes::Backward<ScalarType>};
         for (auto& v : D::Vertices)
            data << b;
      }
      else LANGULUS_ERROR("Shouldn't be reached");

      model->template Commit<Traits::Aim>(Abandon(data));
   }
   else LANGULUS_ERROR("Unsupported topology for sphere/circle normals");
}

/// Generate indices for a sphere/circle                                      
///   @param model - the geometry instance to save data in                    
GENERATE() Indices(Mesh* model) {
   TAny<uint32_t> data;
   if constexpr (CT::Triangle<TOPOLOGY>) {
      // A geosphere/circle made out of triangles                       
      data.Reserve(D::IndexCount);
      for (int i = 0; i < D::TriangleCount; ++i) {
         data << D::Indices[i][0];
         data << D::Indices[i][1];
         data << D::Indices[i][2];
      }
   }
   else if constexpr (CT::Line<TOPOLOGY>) {
      // A geosphere/circle made out of lines                           
      data.Reserve(D::LinesIndexCount);
      for (int i = 0; i < D::LineCount; ++i) {
         data << D::LineIndices[i][0];
         data << D::LineIndices[i][1];
      }
   }
   else LANGULUS_ERROR("Unsupported topology for sphere/circle indices");

   model->template Commit<Traits::Index>(Abandon(data));
}

/// Generate texture coordinates for a sphere/circle                          
///   @param model - the geometry instance to save data in                    
GENERATE() TextureCoords(Mesh* model) {
   TAny<PointType> data;
   data.Reserve(D::VertexCount);
   for (auto& v : D::Vertices)
      data << (v + Half);
   model->template Commit<Traits::Sampler>(Abandon(data));
}

GENERATE() Materials(Mesh* model) {
   if constexpr (CT::Triangle<TOPOLOGY>) {
      // A cube made out of triangles                                   
      TAny<RGB> data;
      data.Reserve(D::IndexCount);

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

GENERATE() Instances(Mesh*) {
   TODO();
}

#undef GENERATE