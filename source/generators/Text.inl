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

#define LGLS_VERBOSE(a) //a


namespace GeometryText
{

   ///  3     2                                                               
   ///   +---+         Each glyph's origin is at point 3 (0,0)                
   ///   |  /|         The vertices are in the XY plane by default (Z=0)      
   ///   | + |                                                                
   ///   |/  |                                                                
   ///   +---+                                                                
   ///  1     0                                                               

   /// Rect's constant occurences                                             
   constexpr Count VertexCount = 4;
   constexpr Count TriangleCount = 2;
   constexpr Count IndexCount = TriangleCount * 3;

   /// Glyph's unique vertices                                                
   constexpr Point2 Vertices[VertexCount] = {
      {1, 1},           // Bottom-right corner                          
      {0, 1},           // Bottom-left corner                           
      {1, 0},           // Top-right corner                             
      {0, 0}            // Top-left corner                              
   };

   /// Indices for a single glyph made of triangles (counter-clockwise)       
   const uint32_t Indices[IndexCount] = {
      0,2,1,2,3,1,
   };

} // namespace GeometryText

   /// Generate glyph positions                                               
   ///   @param instance - the geometry instance to save data in              
   void GeneratePOS(Mesh* instance) {
      if (!instance->CheckTopology<ATriangle>())
         TODO();

      // Get important text traits                                      
      const auto text = instance->GetTraitValue<Traits::Text, ::PCFW::Text>();
      const auto fontName = instance->GetTraitValue<Traits::Font, ::PCFW::Text>();
      const auto alignment = instance->GetTraitValue<Traits::Position, vec2>();
      const auto spacing = instance->GetTraitValue<Traits::Spacing, vec4>();

      // Make sure the font is generated                                
      auto font = instance->GetProducer()->GetFont(fontName);
      font->Generate();

      // Generate positions                                             
      auto content = instance->GetData<Traits::Position>();
      if (content->Is<Point2>()) {
         content->Allocate(VertexCount * text.GetCount());
         vec2 limits;
         vec2 scanned;
         for (auto& c : text) {
            const auto& glyph = font->GetGlyph(c);
            const auto offset = vec2 {
               0, glyph.mHorizontalBearingAndAdvance[1]
            } / AFont::DefaultSpreadInPixels;
            const auto o = scanned - offset + vec2 {0, 0.5};

            // A 2D rectangle for each visible glyph                     
            // Each rectangle is glyphSize / fontSize wide [0;1]         
            switch (c) {
            case ' ':
               LGLS_VERBOSE(pcLogSpecial << "Char ' ' with offset " << o);
               scanned[0] += spacing[2] + 1;
               break;
            case '\t':
               LGLS_VERBOSE(pcLogSpecial << "Char '\\t' with offset " << o);
               scanned[0] += spacing[3] + 3;
               break;
            case '\n':
               LGLS_VERBOSE(pcLogSpecial << "Char '\\n' with offset " << o);
               scanned[0] = 0;
               scanned[1] += spacing[1] + 1;
               break;
            default:
               LGLS_VERBOSE(pcLogSpecial << "Char " << c << " with offset " << o << ccTab);
               for (pcptr i = 0; i < VertexCount; ++i) {
                  const auto t = GlyphVertices[i] + o;
                  LGLS_VERBOSE(pcLogSpecial << "Point " << i << ": " << t);
                  *content << t;
               }
               LGLS_VERBOSE(pcLog << ccUntab);
               scanned[0] += spacing[0] + 1;
            }

            limits = scanned.Max(limits);
         }

         if (limits[0] > 0)
            limits[1] += 1;

         auto contentRange = instance->GetDataRange<Traits::Position>();
         *contentRange = TComplexRange<Point2>(0, limits);
      }
      else TODO();

      instance->GetView().mPCount = content->GetCount();
      LGLS_VERBOSE(pcLogSpecial 
         << "Text contains " << text.GetCount() 
         << " characters, which resulted in " 
         << content->GetCount() << " points");
   }

   /// Generate glyph normals                                                   
   ///   @param instance - the geometry instance to save data in               
   void GenerateNOR(Mesh* instance) {
      if (!instance->CheckTopology<ATriangle>())
         TODO();

      const auto text = instance->GetTraitValue<Traits::Text, ::PCFW::Text>();

      // Text made out of triangles                                       
      auto content = instance->GetData<Traits::Aim>();
      if (content->Is<Normal>()) {
         content->Allocate(VertexCount * text.GetCount());
         for (auto& c : text) {
            switch (c) {
            case ' ': case '\t': case '\n':
               break;
            default:
               for (pcptr i = 0; i < VertexCount; ++i)
                  *content << Normal(Vectors::Backward<real>);
            }
         }

         auto contentRange = instance->GetDataRange<Traits::Aim>();
         *contentRange = TComplexRange<Normal>(Normal {Vectors::Backward<real>});
      }
      else TODO();
   }

   /// Generate glyph texture coordinates                                       
   ///   @param instance - the geometry instance to save data in               
   void GenerateTEX(Mesh* instance) {
      if (!instance->CheckTopology<ATriangle>())
         TODO();

      const auto text = instance->GetTraitValue<Traits::Text, ::PCFW::Text>();
      const auto fontName = instance->GetTraitValue<Traits::Font, ::PCFW::Text>();

      // Make sure the font is generated                                 
      const auto font = instance->GetProducer()->GetFont(fontName);
      font->Generate();

      const auto size = vec4 {
         font->GetWidth(), font->GetHeight(),
         font->GetWidth(), font->GetHeight()
      };

      // Text made out of triangles                                       
      auto content = instance->GetData<Traits::Sampler>();
      if (instance->GetTextureMapper() == Mapper::Custom) {
         // Generate mapping                                             
         if (content->Is<Sampler2>()) {
            Sampler2 uvmin {Sampler2::Max()};
            Sampler2 uvmax {Sampler2::Min()};

            content->Allocate(VertexCount * text.GetCount());
            for (auto& c : text) {
               switch (c) {
               case ' ': case '\t': case '\n':
                  break;
               default: {
                  const auto uv = vec4(font->GetGlyph(c).mRect) / size;
                  LGLS_VERBOSE(pcLogSpecial << "Char " << c << " with UV at " << uv << ccTab);
                  for (pcptr i = 0; i < VertexCount; ++i) {
                     const auto t = uv.xy() + GlyphVertices[i] * uv.zw();
                     LGLS_VERBOSE(pcLogSpecial << "Point " << i << ": " << t);
                     uvmin = uvmin.Min(t);
                     uvmax = uvmax.Max(t);
                     *content << t;
                  }
                  LGLS_VERBOSE(pcLog << ccUntab);
               }
               }
            }

            auto contentRange = instance->GetDataRange<Traits::Sampler>();
            *contentRange = TComplexRange<Sampler2>(uvmin, uvmax);
         }
         else TODO();
      }
      else TODO();
   }

   /// Generate glyph indices                                                   
   ///   @param instance - the geometry instance to save data in               
   void GenerateIDX(Mesh* instance) {
      if (!instance->CheckTopology<ATriangle>())
         TODO();

      const auto text = instance->GetTraitValue<Traits::Text, ::PCFW::Text>();
      auto content = instance->GetData<Traits::Index>();
      if (content->Is<pcu32>()) {
         // Text made out of triangles                                    
         content->Allocate(IndexCount * text.GetCount());
         pcptr relevantCharIndex = 0;
         for (auto& c : text) {
            switch (c) {
            case ' ': case '\t': case '\n':
               break;
            default:
               for (pcptr i = 0; i < IndexCount; ++i)
                  *content << pcu32(relevantCharIndex * VertexCount + TriangleIndices[i]);
               ++relevantCharIndex;
            }
         }

         auto contentRange = instance->GetDataRange<Traits::Index>();
         *contentRange = TRange<pcu32>(0u, content->GetCount() - 1);
      }
      else TODO();

      instance->GetView().mICount = content->GetCount();
   }

   /// Generate rectangle colors                                                
   ///   @param instance - the geometry instance to save data in               
   void GenerateCOL(Mesh*) {
      TODO();
   }

   /// Generate code for the rectangle                                          
   ///   @param instance - the geometry instance to save data in               
   void GenerateCODE(Mesh*) {
      TODO();
   }

   /// Signed distance function                                                
   ///   @param instance - the generator                                       
   ///   @param point - the sampling point                                    
   ///   @return the distance to the geometry at the given point               
   real SDF(const Mesh*, const vec3&) {
      TODO();
   }

   /// Set generators for the rectangle                                          
   void SetGenerators() {
      mSDF = Geometry::Text::SDF;
      mCodeGenerator = Geometry::Text::GenerateCODE;
      mVertexGenerator = Geometry::Text::GeneratePOS;
      mTesselator = DefaultTesselator;
      mNormalGenerator = Geometry::Text::GenerateNOR;
      mTangentGenerator = DefaultTangentGenerator;
      mBitangentGenerator = DefaultBitangentGenerator;
      mTextureCoordGenerator = Geometry::Text::GenerateTEX;
      mColorGenerator = Geometry::Text::GenerateCOL;
      mIndexGenerator = Geometry::Text::GenerateIDX;
   }

   /// Default rectangle definition                                                
   ///   @return true if the default definition exists                           
   bool DefaultCreate() {
      SetTopology<ATriangle>();
      SetTextureMapper(Mapper::Custom);
      AddDataDeclaration<Traits::Position>(MetaOf<Point2>());
      AddDataDeclaration<Traits::Sampler>(MetaOf<Sampler2>());
      AddDataDeclaration<Traits::Index>(MetaOf<pcu32>());
      return true;
   }