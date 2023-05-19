///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "../Model.hpp"


namespace Geometry::Box
{

   ///																								
   ///	PLATONIC BOX																			
   ///																								
   /// Box's constant occurences																
   constexpr pcptr VertexCount = 8;
   constexpr pcptr TriangleCount = 12;
   constexpr pcptr IndexCount = TriangleCount * 3;
   constexpr pcptr FaceCount = TriangleCount / 2;

   /// Box's unique vertices																	
   const Point3 BoxVertices[VertexCount] = {
      // Left face (-x)																	
      Point3(-Half, -Half,  Half),
      Point3(-Half, -Half, -Half),
      Point3(-Half,  Half,  Half),
      Point3(-Half,  Half, -Half),
      // Right face (+x)																
      Point3( Half, -Half, -Half),
      Point3( Half, -Half,  Half),
      Point3( Half,  Half,  Half),
      Point3( Half,  Half, -Half)
   };

   /// Face mapping																				
   const Sampler2 FaceMapping[FaceCount] = {
      Sampler2(0, 0), Sampler2(0, 1), Sampler2(1, 0),
      Sampler2(1, 0), Sampler2(0, 1), Sampler2(1, 1)
   };

   /// Indices for the 12 box triangles													
   const pcu32 BoxIndices[TriangleCount][3] = {
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

   /// Generate positions for box															
   ///	@param instance - the geometry instance to save data in					
   void GeneratePOS(Model* instance) {
      auto content = instance->GetData<Traits::Position>();
      if (instance->CheckTopology<ATriangle>()) {
         // A cube made out of triangles											
         if (content->Is<Triangle3>() || content->Is<Point3>()) {
            content->SetDataID<Triangle3>(true);
            content->Allocate(TriangleCount);
            for (pcptr i = 0; i < TriangleCount; ++i)
               *content << Triangle3(BoxVertices, BoxIndices[i]);
            auto contentRange = instance->GetDataRange<Traits::Position>();
            *contentRange = TComplexRange<Point3>(-0.5, +0.5);
         }
         else TODO();
      }
      else if (instance->CheckTopology<ALine>()) {
         // A cube made out of lines												
         TODO();
      }
      else if (instance->CheckTopology<APoint>()) {
         // A cube made out of points												
         TODO();
      }
      else TODO();
      instance->GetView().mPCount = content->GetCount();
   }

   /// Generate normals for a box															
   ///	@param instance - the geometry instance to save data in					
   void GenerateNOR(Model* instance) {
      if (!instance->CheckTopology<ATriangle>())
         TODO();

      constexpr auto left = Normal(Vectors::Left<real>);
      constexpr auto right = Normal(Vectors::Right<real>);
      constexpr auto up = Normal(Vectors::Up<real>);
      constexpr auto down = Normal(Vectors::Down<real>);
      constexpr auto forward = Normal(Vectors::Forward<real>);
      constexpr auto backward = Normal(Vectors::Backward<real>);

      auto content = instance->GetData<Traits::Aim>();
      if (content->Is<Normal>()) {
         content->Allocate(IndexCount);
         *content << left << left << left << left << left << left;
         *content << right << right << right << right << right << right;
         *content << up << up << up << up << up << up;
         *content << down << down << down << down << down << down;
         *content << forward << forward << forward << forward << forward << forward;
         *content << backward << backward << backward << backward << backward << backward;
         auto contentRange = instance->GetDataRange<Traits::Aim>();
         *contentRange = TComplexRange<Normal>(Normal {-1}, Normal {1});
      }
      else TODO();
   }

   /// Generate indices for a box															
   ///	@param instance - the geometry instance to save data in					
   void GenerateIDX(Model* instance) {
      if (instance->CheckTopology<APoint>())
         TODO();

      auto content = instance->GetData<Traits::Index>();
      if (content->Is<pcu32>()) {
         if (instance->CheckTopology<ATriangle>()) {
            // A box made out of triangles										
            content->Allocate(IndexCount);
            for (pcptr i = 0; i < TriangleCount; ++i) {
               *content << BoxIndices[i][0];
               *content << BoxIndices[i][1];
               *content << BoxIndices[i][2];
            }
         }
         else if (instance->CheckTopology<ALine>()) {
            // A box made out of lines												
            TODO();
         }
         else TODO();

         auto contentRange = instance->GetDataRange<Traits::Index>();
         *contentRange = TRange<pcu32>(0, IndexCount - 1);
      }
      else TODO();

      instance->GetView().mICount = content->GetCount();
   }

   /// Generate texture coordinates for a box											
   ///	@param instance - the geometry instance to save data in					
   void GenerateTEX(Model* instance) {
      auto content = instance->GetData<Traits::Sampler>();
      if (instance->CheckTopology<ATriangle>()) {
         if (instance->GetTextureMapper() == Mapper::Model) {
            // Generate model mapping												
            if (content->Is<Sampler3>()) {
               content->Allocate(IndexCount);
               for (pcptr i = 0; i < TriangleCount; ++i) {
                  *content << BoxVertices[BoxIndices[i][0]] + Half;
                  *content << BoxVertices[BoxIndices[i][1]] + Half;
                  *content << BoxVertices[BoxIndices[i][2]] + Half;
               }
               auto contentRange = instance->GetDataRange<Traits::Sampler>();
               *contentRange = TComplexRange<Sampler3>(0, 1);
            }
            else TODO();
         }
         else if (instance->GetTextureMapper() == Mapper::Face) {
            // Generate face mapping												
            if (content->Is<Sampler2>()) {
               content->Allocate(IndexCount);
               for (pcptr i = 0; i < IndexCount; ++i)
                  *content << FaceMapping[i % (IndexCount / FaceCount)];
               auto contentRange = instance->GetDataRange<Traits::Sampler>();
               *contentRange = TComplexRange<Sampler2>(0, 1);
            }
            else TODO();
         }
         else TODO();
      }
      else if (instance->CheckTopology<ALine>()) {
         TODO();
      }
      else TODO();
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

   void GenerateCOL(Model* instance) {
      if (instance->CheckTopology<ATriangle>()) {
         auto content = instance->GetData<Traits::Color>();
         if (content->Is<rgba>()) {
            // A cube made out of triangles										
            content->Allocate(IndexCount);

            *content << rgba(64, 64, 64, 255);
            *content << rgba(64, 64, 255, 255);
            *content << rgba(64, 255, 255, 255);

            *content << rgba(255, 255, 64, 255);
            *content << rgba(64, 64, 64, 255);
            *content << rgba(64, 255, 64, 255);

            *content << rgba(255, 64, 255, 255);
            *content << rgba(64, 64, 64, 255);
            *content << rgba(255, 64, 64, 255);

            *content << rgba(255, 255, 64, 255);
            *content << rgba(255, 64, 64, 255);
            *content << rgba(64, 64, 64, 255);

            *content << rgba(64, 64, 64, 255);
            *content << rgba(64, 255, 255, 255);
            *content << rgba(64, 255, 64, 255);

            *content << rgba(255, 64, 255, 255);
            *content << rgba(64, 64, 255, 255);
            *content << rgba(64, 64, 64, 255);

            *content << rgba(64, 255, 255, 255);
            *content << rgba(64, 64, 255, 255);
            *content << rgba(255, 64, 255, 255);

            *content << rgba(255, 255, 255, 255);
            *content << rgba(255, 64, 64, 255);
            *content << rgba(255, 255, 64, 255);

            *content << rgba(255, 64, 64, 255);
            *content << rgba(255, 255, 255, 255);
            *content << rgba(255, 64, 255, 255);

            *content << rgba(255, 255, 255, 255);
            *content << rgba(255, 255, 64, 255);
            *content << rgba(64, 255, 64, 255);

            *content << rgba(255, 255, 255, 255);
            *content << rgba(64, 255, 64, 255);
            *content << rgba(64, 255, 255, 255);

            *content << rgba(255, 255, 255, 255);
            *content << rgba(64, 255, 255, 255);
            *content << rgba(255, 64, 255, 255);

            auto contentRange = instance->GetDataRange<Traits::Color>();
            *contentRange = TComplexRange<rgba>(rgba {0,0,0,255}, rgba {255});
         }
         else TODO();
      }
      else if (instance->CheckTopology<ALine>()) {
         // A cube made out of lines												
         TODO();
      }
      else if (instance->CheckTopology<APoint>()) {
         // A cube made out of points												
         TODO();
      }
      else TODO();
   }

   /// Generate GLSL code for the box														
   ///	@param instance - the box generator												
   void GenerateCODE(Model* instance) {
      if (instance->CheckTopology<ATriangle>()) {
         // A cube made out of triangles											
         auto content = instance->GetData<Traits::Code>();
         if (content->Is<GLSL>()) {
            *content << TBox<vec3>::GetDefinition<GLSL>();
            *content << TBox<vec3>().GetUse<GLSL>();
         }
         else TODO();
      }
      else if (instance->CheckTopology<ALine>()) {
         // A cube made out of lines												
         TODO();
      }
      else if (instance->CheckTopology<APoint>()) {
         // A cube made out of points												
         TODO();
      }
      else TODO();
   }

   /// Generate box level of detail															
   ///	@param instance - the box generator												
   ///	@param state - the LOD state to generate										
   ///	@return the new LOD content														
   const Model* LOD(const Model* instance, const LodState&) {
      return instance;
   }

   /// Signed distance function for a box													
   ///	@param instance - the box generator												
   ///	@param point - the sampling point												
   ///	@return the distance to the box at the given point							
   real SDF(const Model*, const vec3& point) {
      return TBox<vec3>().SD(point);
   }

   /// Set generators for the rectangle														
   void SetGenerators() {
      mSDF = Geometry::Box::SDF;
      mVertexGenerator = Geometry::Box::GeneratePOS;
      mNormalGenerator = Geometry::Box::GenerateNOR;
      mTangentGenerator = DefaultTangentGenerator;
      mBitangentGenerator = DefaultBitangentGenerator;
      mCodeGenerator = Geometry::Box::GenerateCODE;
      mTextureCoordGenerator = Geometry::Box::GenerateTEX;
      mInstanceGenerator = Geometry::Box::GenerateINS;
      mRotationGenerator = Geometry::Box::GenerateROT;
      mColorGenerator = Geometry::Box::GenerateCOL;
      mIndexGenerator = Geometry::Box::GenerateIDX;
      mCodeGenerator = Geometry::Box::GenerateCODE;
   }

   /// Default box definition																		
   ///	@return true if the default definition exists									
   bool DefaultCreate() {
      SetTopology<ATriangle>();
      SetTextureMapper(Mapper::Cubic);
      AddDataDeclaration<Traits::Position>(MetaData::Of<Triangle3>());
      AddDataDeclaration<Traits::Aim>(MetaData::Of<Normal>());
      AddDataDeclaration<Traits::Sampler>(MetaData::Of<Sampler2>());
      return true;
   }

} // namespace Geometry::Box
