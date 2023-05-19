///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "../Model.hpp"


namespace Geometry::Grid
{

   constexpr real Half = real(1) / real(2);

   /// Generate vertex positions for the grid											
   ///	@param instance - the geometry instance to save data in					
   void GeneratePOS(Model* instance) {
      // Calculate vertex count - tesselation in this context means 		
      // how many times a sector in the grid is divided in half			
      const vec4 resolution = pcPow(real(2), instance->GetTesselation());
      const auto xsteps = pcptr(resolution[0]);
      const auto ysteps = pcptr(resolution[1]);
      const auto zsteps = pcptr(resolution[2]);
      const auto sep = vec3 {1} / vec3 {real(xsteps), real(ysteps), real(zsteps)};
      auto content = instance->GetData<Traits::Position>();

      if (instance->CheckTopology<ALine>()) {
         // Generate a 3D grid of lines											
         const auto count = (xsteps + 1) * (ysteps + zsteps + 2) + (ysteps + 1) * (zsteps + 1);
         if (content->Is<Line3>()) {
            content->Allocate(count);
            for (pcptr x = 0; x <= xsteps; ++x) {
               const auto real_x = real(x);

               // xsteps*ysteps														
               for (pcptr y = 0; y <= ysteps; ++y) {
                  const vec2 xy {
                     -Half + real_x * sep[0],
                     -Half + real(y) * sep[1]
                  };

                  *content << Line3 {
                     vec3{ xy, -Half },
                     vec3{ xy, +Half }
                  };
               }

               // xsteps*zsteps														
               for (pcptr z = 0; z <= zsteps; ++z) {
                  const auto xx = -Half + real_x * sep[0];
                  const auto zz = -Half + real(z) * sep[2];

                  *content << Line3 {
                     vec3{ xx, -Half, zz },
                     vec3{ xx, +Half, zz }
                  };
               }
            }

            // ysteps*zsteps															
            for (pcptr y = 0; y <= ysteps; ++y) {
               const auto real_y = real(y);

               for (pcptr z = 0; z <= zsteps; ++z) {
                  const vec2 yz {
                     -Half + real_y * sep[1],
                     -Half + real(z) * sep[2]
                  };

                  *content << Line3 {
                     vec3{ -Half, yz },
                     vec3{ +Half, yz }
                  };
               }
            }

            instance->GetView().mPCount = content->GetCount();

            auto contentRange = instance->GetDataRange<Traits::Position>();
            *contentRange = TComplexRange<Point3>(-Half, +Half);
         }
         else TODO();
      }
      else if (instance->CheckTopology<APoint>()) {
         // Generate a 3D grid of points											
         if (content->Is<Point3>()) {
            const auto count = xsteps * ysteps * zsteps;
            content->Allocate(count);
            for (pcptr x = 0; x < xsteps; ++x) {
               const auto xx = real(x);
               for (pcptr y = 0; y < ysteps; ++y) {
                  const auto yy = real(y);
                  for (pcptr z = 0; z < zsteps; ++z) {
                     const auto zz = real(z);
                     *content << Point3{ vec3(xx, yy, zz) * sep - Half};
                  }
               }
            }

            instance->GetView().mPCount = content->GetCount();

            auto contentRange = instance->GetDataRange<Traits::Position>();
            *contentRange = TComplexRange<Point3>(-Half, +Half);
         }
         else TODO();
      }
   }

   void GenerateNOR(Model*) {
      TODO();
   }

   void GenerateTEX(Model*) {
      TODO();
   }

   void GenerateTID(Model*) {
      TODO();
   }

   void GenerateINS(Model*) {
      TODO();
   }

   void GenerateROT(Model*) {
      TODO();
   }

   void GenerateCOL(Model*) {
      TODO();
   }

   void GenerateIDX(Model*) {
      TODO();
   }

   void GenerateCODE(Model*) {
      TODO();
   }

   /// LOD function																				
   ///	@param instance - the generator instance										
   const Model* LOD(const Model* instance, const LodState&) {
      return instance;
   }

   /// Signed distance function																
   ///	@param instance - the generator													
   ///	@param point - the sampling point												
   ///	@return the distance to the geometry at the given point					
   real SDF(const Model*, const vec3&) {
      TODO();
   }

   /// Set generators for the grid																
   void SetGenerators() {
      mVertexGenerator = Geometry::Grid::GeneratePOS;
      mNormalGenerator = Geometry::Grid::GenerateNOR;
      mTangentGenerator = DefaultTangentGenerator;
      mBitangentGenerator = DefaultBitangentGenerator;
      mTextureCoordGenerator = Geometry::Grid::GenerateTEX;
      mInstanceGenerator = Geometry::Grid::GenerateINS;
      mRotationGenerator = Geometry::Grid::GenerateROT;
      mColorGenerator = Geometry::Grid::GenerateCOL;
      mIndexGenerator = Geometry::Grid::GenerateIDX;
      mCodeGenerator = Geometry::Grid::GenerateCODE;
   }

   /// Default grid definition																	
   ///	@return true if the default definition exists									
   bool DefaultCreate() {
      SetTesselation(4);
      SetTopology<ALine>();
      AddDataDeclaration<Traits::Position>(MetaData::Of<Line3>());
      return true;
   }

} // namespace Geometry::Grid
