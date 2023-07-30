///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "../Mesh.hpp"
#include <Math/Primitives/TTriangle.hpp>
#include <Math/Primitives/TLine.hpp>
#include <Math/Mapping.hpp>
#include <Math/Colors.hpp>


namespace Geometry::Zode
{

   /// A zode (or a zenith node) is a radial segment, and the number of       
   /// segments decides the radial size of the zode (360 / ZodeSegments)      
   constexpr Count ZodeSegments = 4;

   /// Generate zode positions                                                
   ///   @param instance - the geometry to generate for                       
   void GeneratePOS(Mesh* instance) {
      // An example zode with tesselation of 3                          
      // The zode is always on the XY plane, at Z=0                     
      //                                                                
      //            (0.5,0)    ________________    (0,0.5)              
      //   div 3 ___________   \/\/\/\/\/\/\/\/       15 triangles      
      //   div 2 ____________   \/\/\/\/\/\/\/        13 triangles      
      //   div 3 _____________   \/\/\/\/\/\/         11 triangles      
      //   div 1 ______________   \/\/\/\/\/          9 triangles       
      //   div 3 _______________   \/\/\/\/           7 triangles       
      //   div 2 ________________   \/\/\/            5 triangles       
      //   div 3 _________________   \/\/             3 triangles       
      //                              \/              1 triangle        
      //                            origin                              
      //                                                                
      // Tesselation of 0 sums up to 1 strips, 3 points (3*1)           
      // Tesselation of 1 sums up to 2 strips, 6 points (3*2)           
      // Tesselation of 2 sums up to 4 strips, 15 points (3*5)          
      // Tesselation of 3 sums up to 8 strips, 45 points (3*15)         
      //                                                                
      auto offset = instance->GetTraitValue<Traits::Position, vec3>();
      const auto distance = offset.Length();
      offset = offset / distance;

      // Create the orientation matrix, that will align to the sphere   
      mat4 orient;
      if (offset.Abs() == Vectors::Up<real>.xyz()) {
         // If the offset is parallel to up vector, use right vector    
         // to avoid degeneracy                                         
         orient = mat4::LookAt(offset, Vectors::Right<real>);
      }
      else {
         // Otherwise use a standard up vector                          
         orient = mat4::LookAt(offset, Vectors::Up<real>);
      }
      orient.SetPosition(offset * real(0.5));

      // This rotator is used to spin each segment                      
      const auto rotator = mat4::RotationAxis(offset, pcD2R(real(360) / real(ZodeSegments)));

      // Calculate point count based on tesselation                     
      auto tesselation = instance->GetTesselation();
      const auto strips = pcptr(std::pow(pcptr(2), tesselation));
      const auto rows = strips + 1;
      const auto points = pcSum(rows);
      const auto step = (HALFPI<real> / distance) / real(strips);
      const auto dir = vec3(step, -step, 0);

      // Generate the unique points for all segments                    
      auto content = instance->GetData<Traits::Position>();
      if (content->Is<Point3>()) {
         Point3 pmin {Point3::Max()};
         Point3 pmax {Point3::Min()};

         content->Allocate(points * ZodeSegments);
         for (pcptr index = 0; index < ZodeSegments; ++index) {
            *content << Point3(orient * vec4(0, 0, 0, 1));
            for (pcptr row = 1; row < rows; ++row) {
               const auto rowStart = vec3(0, step * real(row), 0);
               for (pcptr point = 0; point <= row; ++point) {
                  const Point3 position = vec3(orient * vec4(rowStart + dir * real(point), 1)).Normalize() * real(0.5);
                  *content << position;
                  pmin = pmin.Min(position);
                  pmax = pmax.Max(position);
               }
            }

            orient *= rotator;
         }

         auto contentRange = instance->GetDataRange<Traits::Position>();
         *contentRange = TComplexRange<Point3>(pmin, pmax);
      }
      else TODO();

      // Save the positions                                             
      instance->GetView().mPCount = content->GetCount();
   }

   /// Generate zode normals                                                  
   ///   @param instance - the geometry to generate for                       
   void GenerateNOR(Mesh* instance) {
      const auto positions = instance->GetData<Traits::Position>();
      if (!positions || !*positions)
         throw Except::Content();

      if (positions->Is<Point3>()) {
         auto normals = instance->GetData<Traits::Aim>();
         if (normals->Is<Normal>()) {
            Normal nmin {1};
            Normal nmax {-1};

            normals->Allocate(positions->GetCount());
            for (pcptr i = 0; i < normals->GetReserved(); ++i) {
               const auto n = Normal {positions->Get<Point3>(i)};
               *normals << n;
               nmin = nmin.Min(n);
               nmax = nmax.Max(n);
            }

            auto contentRange = instance->GetDataRange<Traits::Aim>();
            *contentRange = TComplexRange<Normal>(nmin, nmax);
         }
         else TODO();
      }
      else {
         pcLogFuncWarning << "No normals will be generated for zode, "
            "because positions are of incompatible format " << positions->GetToken();
      }
   }

   /// Generate texture coordinates                                           
   ///   @param instance - the geometry to generate for                       
   void GenerateTEX(Mesh* instance) {
      const auto positions = instance->GetData<Traits::Position>();
      if (!positions || !*positions)
         throw Except::Content();

      if (positions->Is<Point3>()) {
         auto content = instance->GetData<Traits::Sampler>();
         if (content->Is<Sampler3>()) {
            Sampler3 smin {1};
            Sampler3 smax {0};

            content->Allocate(positions->GetCount());
            for (pcptr i = 0; i < content->GetReserved(); ++i) {
               const auto s = Sampler3 {positions->Get<Point3>(i)};
               *content << s;
               smin = smin.Min(s);
               smax = smax.Max(s);
            }

            auto contentRange = instance->GetDataRange<Traits::Sampler>();
            *contentRange = TComplexRange<Sampler3>(smin, smax);
         }
         else TODO();
      }
      else {
         pcLogFuncWarning << "No samplers will be generated for zode, "
            "because positions are of incompatible format " << positions->GetToken();
      }
   }

   void GenerateTID(Mesh*) {
      TODO();
   }

   void GenerateINS(Mesh*) {
      TODO();
   }

   void GenerateROT(Mesh*) {
      TODO();
   }

   void GenerateCOL(Mesh*) {
      TODO();
   }

   /// Generate zode indices                                                  
   ///   @param instance - the geometry to generate indices for               
   void GenerateIDX(Mesh* instance) {
      auto tesselation = instance->GetTesselation();
      const auto strips = pcu32(std::pow(2, tesselation));
      auto content = instance->GetData<Traits::Index>();

      if (content->Is<pcu32>()) {
         if (instance->CheckTopology<ATriangleStrip>()) {
            // A zode made of triangle strips                           
            const auto indicesPerSegment = strips + pcSum(strips) * 2 + (strips - 1) * 2;
            const auto verticesPerSegment = pcSum(strips + 1);
            content->Allocate(indicesPerSegment * ZodeSegments + 2 * (ZodeSegments - 1));

            // Generate indices for all segments                        
            for (pcu32 index = 0; index < ZodeSegments; ++index) {
               // Generate strips for this segment                      
               const auto segmentStart = index * verticesPerSegment;
               for (pcu32 strip = 0; strip < strips; ++strip) {
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
                  const auto loVertex = segmentStart + pcSum(strip);
                  const auto hiVertex = loVertex + strip + 1;
                  const auto loMax = hiVertex;
                  const auto hiMax = hiVertex + strip + 2;

                  // Place the first triangle                           
                  *content << hiMax - 1;
                  *content << loMax - 1;

                  // Then place the rest of the triangles               
                  for (pcu32 triangle = 1; triangle <= (strip + 1) * 2; ++triangle) {
                     if ((triangle % 2) == 1) {
                        // Top vertex                                   
                        *content << hiMax - (triangle + 1) / 2;
                     }
                     else {
                        // Bottom vertex                                
                        *content << loMax - triangle / 2;
                     }
                  }

                  // We add degenerate triangle in order to link strips 
                  if (strip < strips - 1) {
                     *content << content->Get<pcu32>(content->GetCount() - 1);
                     *content << hiMax;
                  }
               }

               // We add a degenerate triangle in order to link regions 
               if (index < ZodeSegments - 1) {
                  *content << content->Get<pcu32>(content->GetCount() - 1);
                  *content << segmentStart + verticesPerSegment;
               }
            }

            auto contentRange = instance->GetDataRange<Traits::Index>();
            *contentRange = TRange<pcu32>(0u, content->GetCount() - 1);
         }
         else if (instance->CheckTopology<ATriangle>()) {
            // A zode made of a triangle list                           
            TODO();
         }
         else if (instance->CheckTopology<ALine>()) {
            // A zode made of a line list                               
            TODO();
         }
         else if (instance->CheckTopology<ALineStrip>()) {
            // A zode made of a line strip                              
            TODO();
         }
         else if (instance->CheckTopology<APoint>()) {
            // A zode made out of points. No indices required.          
            return;
         }
         else TODO();
      }
      else TODO();

      instance->GetView().mICount = content->GetCount();
   }

   /// Generate zode code                                                     
   ///   @param instance - the geometry to generate code for                  
   void GenerateCODE(Mesh*) {
      TODO();
   }

   /// Signed distance function                                               
   ///   @param instance - the generator                                      
   ///   @param point - the sampling point                                    
   ///   @return the distance to the geometry at the given point              
   real SDF(const Mesh*, const vec3& point) {
      return TTriangle<vec3>().SD(point);
   }

   /// Set generators for the zode                                            
   void SetGenerators() {
      mVertexGenerator = Geometry::Zode::GeneratePOS;
      mNormalGenerator = Geometry::Zode::GenerateNOR;
      mTangentGenerator = DefaultTangentGenerator;
      mBitangentGenerator = DefaultBitangentGenerator;
      mTextureCoordGenerator = Geometry::Zode::GenerateTEX;
      mInstanceGenerator = Geometry::Zode::GenerateINS;
      mRotationGenerator = Geometry::Zode::GenerateROT;
      mColorGenerator = Geometry::Zode::GenerateCOL;
      mIndexGenerator = Geometry::Zode::GenerateIDX;
      mSDF = Geometry::Zode::SDF;
      mCodeGenerator = Geometry::Zode::GenerateCODE;
      mTesselator = nullptr;
      mLodGenerator = nullptr;
   }

   /// Default zode definition                                                
   ///   @return true if the default definition exists                        
   bool DefaultCreate() {
      SetTopology<ATriangleStrip>();
      SetTextureMapper(Mapper::Plane);
      AddDataDeclaration<Traits::Position>(MetaOf<Point3>());
      AddDataDeclaration<Traits::Aim>(MetaOf<Normal>());
      AddDataDeclaration<Traits::Sampler>(MetaOf<Sampler2>());
      return true;
   }

} // namespace Geometry::Zode
