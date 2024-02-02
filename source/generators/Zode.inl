///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "../Mesh.hpp"
#include <Math/Primitives/Triangle.hpp>
#include <Math/Primitives/Line.hpp>
#include <Math/Matrix.hpp>
#include <Math/Mapping.hpp>
#include <Math/Color.hpp>
#include <Math/Sampler.hpp>
#include <Math/Angle.hpp>

template<CT::Vector T>
struct TZode;


namespace Langulus::A
{
   /// An abstract zode                                                       
   struct Zode {
      LANGULUS(ABSTRACT) true;
      LANGULUS(CONCRETE) TZode<Vec3>;
   };
}

namespace Langulus::CT
{
   /// Concept for distinguishing zodes                                       
   template<class...T>
   concept Zode = (DerivedFrom<T, A::Zode> and ...);
}


///                                                                           
/// An example zode with tesselation of 3                                     
/// The zode is always on the XY plane, at Z=0                                
///                                                                           
///                         radial size                                       
///                    \ <--------------->  /                                 
///                     \                  /                                  
///            (0.5;0)    ________________    (0;0.5)                         
///   div 3 ___________   \/\/\/\/\/\/\/\/       15 triangles                 
///   div 2 ____________   \/\/\/\/\/\/\/        13 triangles                 
///   div 3 _____________   \/\/\/\/\/\/         11 triangles                 
///   div 1 ______________   \/\/\/\/\/          9 triangles                  
///   div 3 _______________   \/\/\/\/           7 triangles                  
///   div 2 ________________   \/\/\/            5 triangles                  
///   div 3 _________________   \/\/             3 triangles                  
///                              \/              1 triangle                   
///                            origin                                         
///                                                                           
/// Tesselation of 0 sums up to 1 strips, 3 points (3*1)                      
/// Tesselation of 1 sums up to 2 strips, 6 points (3*2)                      
/// Tesselation of 2 sums up to 4 strips, 15 points (3*5)                     
/// Tesselation of 3 sums up to 8 strips, 45 points (3*15)                    
///                                                                           
template<CT::Vector T>
struct TZode : A::Zode {
   LANGULUS(ABSTRACT) false;
   LANGULUS(POD) CT::POD<T>;
   LANGULUS(TYPED) TypeOf<T>;
   LANGULUS_BASES(A::Zode);

   using PointType = T;
   using ScalarType = TypeOf<T>;
   static constexpr Count MemberCount = T::MemberCount;
   static_assert(MemberCount == 3, "Zode must always be three-dimensional");

   // A zode (or a zenith node) is a radial segment, and the number of  
   // segments decides the radial size of the zode (360 / ZodeSegments) 
   static constexpr Count Segments = 4;
   static constexpr Radians RadialSize = Degrees {360 / Segments};
};


///                                                                           
///    Zode mesh generators                                                   
///                                                                           
///   @tparam T - the primitive to use for point type and dimensions          
///   @tparam TOPOLOGY - are we generating triangles/lines/points?            
///                                                                           
template<CT::Zode T, CT::Topology TOPOLOGY = A::Triangle>
struct GenerateZode {
   using PointType = typename T::PointType;
   using ScalarType = TypeOf<PointType>;
   static constexpr Count Dimensions = T::MemberCount;
   static constexpr ScalarType Half = ScalarType {1} / ScalarType {2};

   static_assert(Dimensions >= 3, "Zode should be at least 3D");

   NOD() static Construct Default(Neat&&);
   NOD() static Construct Detail(const Mesh*, const LOD&);

   static void Indices(Mesh*);
   static void Positions(Mesh*);
   static void Normals(Mesh*);
   static void TextureCoords(Mesh*);
   static void Materials(Mesh*);
   static void Instances(Mesh*);
};

#define GENERATE() template<CT::Zode T, CT::Topology TOPOLOGY> \
   void GenerateZode<T, TOPOLOGY>::


/// Generate positions for a zode                                             
///   @param model - the model to fill                                        
GENERATE() Positions(Mesh* model) {
   PointType offset;
   model->GetNeat().ExtractTrait<Traits::Place>(offset);
   unsigned tesselation = 0;
   model->GetNeat().ExtractTrait<Traits::Tesselation>(tesselation);

   const auto distance = offset.Length();
   offset = offset / distance;

   // Create the orientation matrix, that will align to the sphere      
   Mat4 orient;
   if (offset.Abs() == Axes::Up<ScalarType>.xyz()) {
      // If the offset is parallel to up vector, use right vector       
      // to avoid degeneracy                                            
      orient = Mat4::LookAt(offset, Axes::Right<ScalarType>);
   }
   else {
      // Otherwise use a standard up vector                             
      orient = Mat4::LookAt(offset, Axes::Up<ScalarType>);
   }
   orient.SetPosition(offset * Half);

   // This rotator is used to spin each segment                         
   const auto rotator = Mat4::RotateAxis(offset, T::RadialSize);

   // Calculate point count based on tesselation                        
   const auto strips = Pow(2u, tesselation);
   const auto rows = strips + 1;
   const auto points = Sum(rows);
   const auto step = (HALFPI<ScalarType> / distance) / static_cast<ScalarType>(strips);
   const auto dir = PointType {step, -step, 0};

   // Generate the unique points for all segments                       
   TAny<PointType> data;
   data.Reserve(points * T::Segments);
   for (Offset index = 0; index < T::Segments; ++index) {
      data << PointType {orient * Vec4 {0, 0, 0, 1}};

      for (Offset row = 1; row < rows; ++row) {
         const auto rowStart = PointType {0, step * static_cast<ScalarType>(row), 0};

         for (Offset point = 0; point <= row; ++point) {
            const auto position = PointType {
               orient * Vec4(rowStart + dir * static_cast<ScalarType>(point), 1)
            }.Normalize() * Half;

            data << position;
         }
      }

      orient *= rotator;
   }

   model->Commit<Traits::Place>(Abandon(data));
}

/// Generate normals for a zode                                               
///   @param model - the geometry instance to save data in                    
GENERATE() Normals(Mesh* model) {
   const auto positions = model->GetData<Traits::Place>();

   TAny<Normal> data;
   data.Reserve(positions->GetCount());
   for (Offset i = 0; i < positions->GetCount(); ++i)
      data << Normal {positions->AsCast<Vec3>(i)};
   model->Commit<Traits::Aim>(Abandon(data));
}

/// Generate indices for a zode                                               
///   @param model - the geometry instance to save data in                    
GENERATE() Indices(Mesh* model) {
   unsigned tesselation = 0;
   model->GetNeat().ExtractTrait<Traits::Tesselation>(tesselation);
   const auto strips = Pow(2u, tesselation);
   TAny<uint32_t> data;

   if constexpr (CT::TriangleStrip<TOPOLOGY>) {
      // A zode made of triangle strips                                 
      const auto indicesPerSegment = strips + Sum(strips) * 2 + (strips - 1) * 2;
      const auto verticesPerSegment = Sum(strips + 1);
      data.Reserve(indicesPerSegment * T::Segments + 2 * (T::Segments - 1));

      // Generate indices for all segments                              
      for (uint32_t index = 0; index < T::Segments; ++index) {
         // Generate strips for this segment                            
         const auto segmentStart = index * verticesPerSegment;

         for (uint32_t strip = 0; strip < strips; ++strip) {
            // High is [                                                
            //   startingVertex + strip + 1;                            
            //   startingVertex + strip * 2 + 3                         
            // )                                                        
            //                                                          
            //    \/\/\/\/\/\/\/\/\/\/\/     strip from the zode        
            //                                                          
            // Low is [                                                 
            //   startingVertex;                                        
            //   startingVertex + strip + 1                             
            // )                                                        
            //                                                          
            const auto loVertex = segmentStart + Sum(strip);
            const auto hiVertex = loVertex + strip + 1;
            const auto loMax = hiVertex;
            const auto hiMax = hiVertex + strip + 2;

            // Place the first triangle                                 
            data << hiMax - 1;
            data << loMax - 1;

            // Then place the rest of the triangles                     
            for (uint32_t triangle = 1; triangle <= (strip + 1) * 2; ++triangle) {
               if ((triangle % 2) == 1) {
                  // Top vertex                                         
                  data << hiMax - (triangle + 1) / 2;
               }
               else {
                  // Bottom vertex                                      
                  data << loMax - triangle / 2;
               }
            }

            // We add degenerate triangle in order to link strips       
            if (strip < strips - 1) {
               data << data[-1];
               data << hiMax;
            }
         }

         // We add a degenerate triangle in order to link regions       
         if (index < T::Segments - 1) {
            data << data[-1];
            data << segmentStart + verticesPerSegment;
         }
      }

      model->Commit<Traits::Index>(Abandon(data));
   }
   else if constexpr (CT::Triangle<TOPOLOGY>) {
      // A zode made of a triangle list                                 
      TODO();
   }
   else if constexpr (CT::LineStrip<TOPOLOGY>) {
      // A zode made of a line strip                                    
      TODO();
   }
   else if constexpr (CT::Line<TOPOLOGY>) {
      // A zode made of a line list                                     
      TODO();
   }
   else LANGULUS_ERROR("Unsupported topology for box positions");
}

/// Generate texture coordinates for a zode                                   
///   @param model - the geometry instance to save data in                    
GENERATE() TextureCoords(Mesh* model) {
   const auto positions = model->GetData<Traits::Place>();

   TAny<Sampler3> data;
   data.Reserve(positions->GetCount());
   for (Offset i = 0; i < positions->GetCount(); ++i)
      data << Sampler3 {positions->template AsCast<Vec3>(i)};
   model->Commit<Traits::Sampler>(Abandon(data));
}

GENERATE() Materials(Mesh*) {
   TODO();
}

GENERATE() Instances(Mesh*) {
   TODO();
}

#undef GENERATE