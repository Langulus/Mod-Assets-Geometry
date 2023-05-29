///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "../Model.hpp"
#include <Math/Primitives/TBox.hpp>
#include <Math/Primitives/TTriangle.hpp>
#include <Math/Primitives/TLine.hpp>
#include <Math/Mapping.hpp>
#include <Math/Colors.hpp>


namespace GeometryRect
{

   ///  3     2                                                               
   ///   +---+         Each corner is at distance 0.5 from center by default  
   ///   |  /|         The vertices are in the XY plane by default (Z=0)      
   ///   | + |                                                                
   ///   |/  |                                                                
   ///   +---+                                                                
   ///  1     0                                                               

   /// Rect's constant occurences                                             
   constexpr Count VertexCount = 4;
   constexpr Count TriangleCount = 2;
   constexpr Count LineCount = 4;
   constexpr Count IndexCount = TriangleCount * 3;
   constexpr Count FaceCount = TriangleCount / 2;

   /// Rect's unique vertices                                                 
   constexpr Point3 Vertices[VertexCount] = {
      Point3 { Half<>,  Half<>, 0},     // Bottom-right corner           
      Point3 {-Half<>,  Half<>, 0},     // Bottom-left corner            
      Point3 { Half<>, -Half<>, 0},     // Top-right corner              
      Point3 {-Half<>, -Half<>, 0}      // Top-left corner               
   };

   /// Face mapping                                                           
   constexpr Sampler2 FaceMapping[3] = {
      Sampler2 {0, 0},
      Sampler2 {0, 1},
      Sampler2 {1, 0},
   };

   /// Indices for the rect triangles (counter-clockwise)                     
   constexpr uint32_t TriangleIndices[TriangleCount][3] = {
      {0,2,1}, {2,3,1},
   };

   /// Indices for the rect lines                                             
   constexpr uint32_t LineIndices[LineCount][2] = {
      {0,1}, {1,3}, {3,2}, {2,0},
   };

} //namespace GeometryRect


   /// Generate rectangle positions                                           
   ///   @param instance - the geometry instance to save data in              
   void GeneratePOS(CGeneratorGeometry* instance) {
      auto content = instance->GetData<Traits::Position>();

      if (instance->CheckTopology<ATriangle>()) {
         // A rectangle made out of triangles                           
         if (content->Is<Triangle3>() || content->Is<Point3>()) {
            // A 3D rectangle                                           
            content->SetDataID<Triangle3>(true);
            content->Allocate(TriangleCount);
            *content << Triangle3(RectVertices, TriangleIndices[0]);
            *content << Triangle3(RectVertices, TriangleIndices[1]);
            auto contentRange = instance->GetDataRange<Traits::Position>();
            *contentRange = TComplexRange<Point3>(RectVertices[3], RectVertices[0]);
         }
         else if (content->Is<Triangle2>() || content->Is<Point2>()) {
            // A 2D rectangle                                           
            content->SetDataID<Triangle2>(true);
            content->Allocate(TriangleCount);
            *content << Triangle2(RectVertices, TriangleIndices[0]);
            *content << Triangle2(RectVertices, TriangleIndices[1]);
            auto contentRange = instance->GetDataRange<Traits::Position>();
            *contentRange = TComplexRange<Point2>(RectVertices[3], RectVertices[0]);
         }
         else TODO();
      }
      else if (instance->CheckTopology<ALine>()) {
         // A rectangle made out of lines                               
         if (content->Is<Line3>() || content->Is<Point3>()) {
            content->SetDataID<Line3>(true);
            content->Allocate(LineCount);
            *content << Line3(RectVertices, LineIndices[0]);
            *content << Line3(RectVertices, LineIndices[1]);
            *content << Line3(RectVertices, LineIndices[2]);
            *content << Line3(RectVertices, LineIndices[3]);
            auto contentRange = instance->GetDataRange<Traits::Position>();
            *contentRange = TComplexRange<Point3>(RectVertices[3], RectVertices[0]);
         }
         else if (content->Is<Line2>() || content->Is<Point2>()) {
            content->SetDataID<Line2>(true);
            content->Allocate(LineCount);
            *content << Line2(RectVertices, LineIndices[0]);
            *content << Line2(RectVertices, LineIndices[1]);
            *content << Line2(RectVertices, LineIndices[2]);
            *content << Line2(RectVertices, LineIndices[3]);
            auto contentRange = instance->GetDataRange<Traits::Position>();
            *contentRange = TComplexRange<Point2>(RectVertices[3], RectVertices[0]);
         }
         else TODO();
      }
      else if (instance->CheckTopology<APoint>()) {
         // A rectangle made out of points                              
         if (content->Is<Point3>()) {
            content->Allocate(VertexCount);
            *content << RectVertices[0];
            *content << RectVertices[1];
            *content << RectVertices[2];
            *content << RectVertices[3];
            auto contentRange = instance->GetDataRange<Traits::Position>();
            *contentRange = TComplexRange<Point3>(RectVertices[3], RectVertices[0]);
         }
         else TODO();
      }
      else TODO();

      instance->GetView().mPCount = content->GetCount();
   }

   /// Generate rectangle normals                                             
   ///   @param instance - the geometry instance to save data in               
   void GenerateNOR(CGeneratorGeometry* instance) {
      if (!instance->CheckTopology<ATriangle>())
         TODO();

      // A rectangle made out of triangles                              
      auto content = instance->GetData<Traits::Aim>();
      if (content->Is<Normal>()) {
         content->Allocate(IndexCount);
         for (pcptr i = 0; i < IndexCount; ++i)
            *content << Normal(Vectors::Backward<real>);
         auto contentRange = instance->GetDataRange<Traits::Aim>();
         *contentRange = TComplexRange<Normal>(Normal {Vectors::Backward<real>});
      }
      else TODO();
   }

   /// Generate rectangle texture coordinates                                 
   ///   @param instance - the geometry instance to save data in               
   void GenerateTEX(CGeneratorGeometry* instance) {
      auto content = instance->GetData<Traits::Sampler>();

      if (instance->CheckTopology<ATriangle>()) {
         // A rectangle made out of triangles                           
         if (instance->GetTextureMapper() == Mapper::Model ||
             instance->GetTextureMapper() == Mapper::Plane
         ) {
            // Generate model mapping                                    
            if (content->Is<Sampler3>()) {
               content->Allocate(IndexCount);
               for (pcptr i = 0; i < TriangleCount; ++i) {
                  *content << RectVertices[TriangleIndices[i][0]] + vec3(Half, Half, 0);
                  *content << RectVertices[TriangleIndices[i][1]] + vec3(Half, Half, 0);
                  *content << RectVertices[TriangleIndices[i][2]] + vec3(Half, Half, 0);
               }
               auto contentRange = instance->GetDataRange<Traits::Sampler>();
               *contentRange = TComplexRange<Sampler3>(0, 1);
            }
            else if (content->Is<Sampler2>()) {
               content->Allocate(IndexCount);
               for (pcptr i = 0; i < TriangleCount; ++i) {
                  *content << RectVertices[TriangleIndices[i][0]].xy() + Half;
                  *content << RectVertices[TriangleIndices[i][1]].xy() + Half;
                  *content << RectVertices[TriangleIndices[i][2]].xy() + Half;
               }
               auto contentRange = instance->GetDataRange<Traits::Sampler>();
               *contentRange = TComplexRange<Sampler2>(0, 1);
            }
            else TODO();
         }
         else if (instance->GetTextureMapper() == Mapper::Face) {
            // Generate face mapping                                    
            if (content->Is<Sampler2>()) {
               content->Allocate(IndexCount);
               for (pcptr i = 0; i < IndexCount; ++i)
                  *content << FaceMapping[i % 3];
               auto contentRange = instance->GetDataRange<Traits::Sampler>();
               *contentRange = TComplexRange<Sampler2>(0, 1);
            }
            else TODO();
         }
         else TODO();
      }
      else TODO();
   }

   /// Generate rectangle indices coordinates                                 
   ///   @param instance - the geometry instance to save data in               
   void GenerateIDX(CGeneratorGeometry* instance) {
      if (instance->CheckTopology<APoint>())
         TODO();

      auto content = instance->GetData<Traits::Index>();

      if (content->Is<pcu32>()) {
         if (instance->CheckTopology<ATriangle>()) {
            // A box made out of triangles                              
            content->Allocate(IndexCount);
            for (pcptr i = 0; i < TriangleCount; ++i) {
               *content << TriangleIndices[i][0];
               *content << TriangleIndices[i][1];
               *content << TriangleIndices[i][2];
            }
            auto contentRange = instance->GetDataRange<Traits::Index>();
            *contentRange = TRange<pcu32>(0u, IndexCount - 1);
         }
         else if (instance->CheckTopology<ALine>()) {
            // A box made out of lines                                    
            TODO();
         }
         else TODO();
      }
      else TODO();

      instance->GetView().mICount = content->GetCount();
   }

   /// Generate rectangle colors                                                
   ///   @param instance - the geometry instance to save data in               
   void GenerateCOL(CGeneratorGeometry*) {
      TODO();
   }

   /// Generate code for the rectangle                                          
   ///   @param instance - the geometry instance to save data in               
   void GenerateCODE(CGeneratorGeometry*) {
      TODO();
   }

   /// LOD function                                                            
   const CGeneratorGeometry* LOD(const CGeneratorGeometry* instance, const LodState&) {
      return instance;
   }

   /// Signed distance function                                                
   ///   @param instance - the generator                                       
   ///   @param point - the sampling point                                    
   ///   @return the distance to the geometry at the given point               
   real SDF(const CGeneratorGeometry*, const vec3& point) {
      return TBox<vec3>().SD(point);
   }

} // namespace Geometry::Rectangle

/// Set generators for the rectangle                                          
void CGeometryRectangle::SetGenerators() {
   mSDF = Geometry::Rectangle::SDF;
   mCodeGenerator = Geometry::Rectangle::GenerateCODE;
   mVertexGenerator = Geometry::Rectangle::GeneratePOS;
   mTesselator = DefaultTesselator;
   mNormalGenerator = Geometry::Rectangle::GenerateNOR;
   mTangentGenerator = DefaultTangentGenerator;
   mBitangentGenerator = DefaultBitangentGenerator;
   mTextureCoordGenerator = Geometry::Rectangle::GenerateTEX;
   mColorGenerator = Geometry::Rectangle::GenerateCOL;
   mIndexGenerator = Geometry::Rectangle::GenerateIDX;
}

/// Default rectangle definition                                                
///   @return true if the default definition exists                           
bool CGeometryRectangle::DefaultCreate() {
   SetTopology<ATriangle>();
   SetTextureMapper(Mapper::Plane);
   AddDataDeclaration<Traits::Position>(MetaData::Of<Triangle2>());
   AddDataDeclaration<Traits::Sampler>(MetaData::Of<Sampler2>());
   return true;
}
