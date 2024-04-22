///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright (c) 2016 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Box.inl"
#include <Langulus/Image.hpp>
#include <Math/Scale.hpp>

template<CT::Vector T>
struct TLabel;

namespace Langulus::A
{
   /// An abstract label                                                      
   struct Label {
      LANGULUS(ABSTRACT) true;
      LANGULUS(CONCRETE) TLabel<Vec3>;
   };
}

namespace Langulus::CT
{
   /// Concept for distinguishing labels                                      
   template<class...T>
   concept Label = (DerivedFrom<T, A::Label> and ...);
}


///                                                                        |  
/// Label geometry generator                                               |  
///                                                                        |  
/// origin                                                                 |  
/// v                                                                      |  
/// +-------+                                                              |  
/// |       |-------+-------+-------+-------+ -----> +X                    |  
/// |   R   |   I   |   G   |   H   |   T   |                              |  
/// |       |       |       |       |       |   \n                         |  
/// +-------+-------+       +-------+-------+                              |  
///                                                                        |  
/// +-------+-------+       +-------+-------+                              |  
/// |   O   |   N   | space |   I   |   T   |                              |  
/// |       |       |       |       |       |                              |  
/// +-------+-------+       +-------+-------+                              |  
/// |                                                                      |  
/// v +Y                                                                   |  
///                                                                        |  
template<CT::Vector T>
struct TLabel : A::Label {
   LANGULUS(ABSTRACT) false;
   LANGULUS(POD) CT::POD<T>;
   LANGULUS(TYPED) TypeOf<T>;
   LANGULUS_BASES(A::Label);

   using PointType = T;
   static constexpr Count MemberCount = T::MemberCount;
   static_assert(MemberCount > 1, "Can't have one-dimensional label");

   Text mText;
   Scale2 mWrapArea;
   bool mWordWrap {};
   Vec2 mAlignment {};
   Real mCharSpacing {};
   Real mLineSpacing {};
   Real mBlankSpacing {};
   Real mTabSpacing {};
   Ref<A::Font> mFont;
};


///                                                                           
///    Label mesh generators                                                  
///                                                                           
///   @tparam T - the primitive to use for point type and dimensions          
///   @tparam TOPOLOGY - are we generating triangles/lines/points?            
///                                                                           
template<CT::Label T, CT::Topology TOPOLOGY = A::Triangle>
struct GenerateLabel {
   using PointType = typename T::PointType;
   using ScalarType = TypeOf<PointType>;
   static constexpr Count Dimensions = T::MemberCount;

   static_assert(CT::Exact<TOPOLOGY, A::Triangle>,
      "Label can be only have triangle topology");
   static_assert(Dimensions >= 2,
      "Label should be at least 2D");

   ///  3     2                                                               
   ///   +---+         Each glyph is indexed in this way                      
   ///   |  /|                                                                
   ///   | / |                                                                
   ///   |/  |                                                                
   ///   +---+                                                                
   ///  1     0                                                               
   static constexpr Count VertexCount = 4;
   static constexpr Count TriangleCount = 2;
   static constexpr Count IndexCount = TriangleCount * 3;

   /// Glyph's unique vertices                                                
   static constexpr Point2 GlyphVertices[VertexCount] = {
      {1, 1},           // Bottom-right corner                          
      {0, 1},           // Bottom-left corner                           
      {1, 0},           // Top-right corner                             
      {0, 0}            // Top-left corner                              
   };

   /// Indices for a single glyph made of triangles (counter-clockwise)       
   static constexpr uint32_t GlyphIndices[IndexCount] = {
      0,2,1,2,3,1,
   };

   NOD() static bool Default(Construct&);
   NOD() static Construct Detail(const Mesh*, const LOD&);

   static void Indices(const Mesh*);
   static void Positions(const Mesh*);
   static void Normals(const Mesh*);
   static void TextureCoords(const Mesh*);
   static void Materials(const Mesh*);
   static void Instances(const Mesh*);
};

#define GENERATE() template<CT::Label T, CT::Topology TOPOLOGY> \
   void GenerateLabel<T, TOPOLOGY>::


/// Default label generation                                                  
///   @param desc - the descriptor to use                                     
///   @return a newly generated descriptor, with missing traits being set to  
///           their defaults                                                  
template<CT::Label T, CT::Topology TOPOLOGY>
bool GenerateLabel<T, TOPOLOGY>::Default(Construct& desc) {
   auto& d = desc.GetDescriptor();

   if constexpr (CT::Triangle<TOPOLOGY>) {
      // A sphere made out of triangles                                 
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
      d.SetDefaultTrait<Traits::Place>(
         MetaOf<TLine<PointType>>());
   }
   else if constexpr (CT::Point<TOPOLOGY>) {
      // A sphere made out of points                                    
      d.SetDefaultTrait<Traits::Place>(
         MetaOf<PointType>());
   }
   else return false;

   d.SetDefaultTrait<Traits::Topology>(MetaOf<TOPOLOGY>());
   d.SetDefaultTrait<Traits::MapMode>(MapMode::Cube);
   desc.SetType<A::Mesh>();
   return true;
}

/// Generate label level of detail, giving a LOD state                        
///   @param model - the generator                                            
///   @param lod - the LOD state to generate                                  
///   @return the same descriptor, labels don't have LOD                      
template<CT::Label T, CT::Topology TOPOLOGY>
Construct GenerateLabel<T, TOPOLOGY>::Detail(const Mesh* model, const LOD&) {
   return Construct::From<A::Mesh>(model->GetNeat());
}

/// Generate positions for label                                              
///   @param model - the model to fill                                        
GENERATE() Positions(const Mesh* model) {
   T label;
   model->GetNeat().ExtractData(label);
   TMany<PointType> data;
   data.Reserve(VertexCount * label.mText.GetCount());

   Vec2 limits;
   Vec2 scanned;
   for (const auto& c : label.mText) {
      const auto& glyph = label.mFont->GetGlyph(c);
      const auto o = scanned - glyph.mKerning + Vec2 {0, 0.5};

      // A 2D rectangle for each visible glyph                          
      // Each rectangle is glyphSize / fontSize wide [0;1]              
      switch (c) {
      case ' ':
         scanned[0] += label.mBlankSpacing + 1;
         break;
      case '\t':
         scanned[0] += label.mTabSpacing + 3;
         break;
      case '\n':
         scanned[0] = 0;
         scanned[1] += label.mLineSpacing + 1;
         break;
      default:
         for (auto& v : GlyphVertices)
            data << v + o;
         scanned[0] += label.mCharSpacing + 1;
      }

      limits = scanned.Max(limits);
   }

   if (limits[0] > 0)
      limits[1] += 1;
}

/// Generate indices for label                                                
///   @param model - the geometry instance to save data in                    
GENERATE() Indices(const Mesh* model) {
   T label;
   model->GetNeat().ExtractData(label);
   TMany<uint32_t> data;
   data.Reserve(IndexCount * label.mText.GetCount());

   Offset relevantCharIndex = 0;
   for (auto& c : label.mText) {
      if (IsSpace(c))
         continue;

      for (auto& i : GlyphIndices)
         data << relevantCharIndex * VertexCount + i;
      ++relevantCharIndex;
   }

   model->template Commit<Traits::Index>(Abandon(data));
}

/// Generate normals for label                                                
///   @param model - the geometry instance to save data in                    
GENERATE() Normals(const Mesh* model) {
   T label;
   model->GetNeat().ExtractData(label);
   constexpr Normal n = Axes::Backward<ScalarType>;
   TMany<Normal> data;
   data.Reserve(VertexCount * label.mText.GetCount());

   for (auto& c : label.mText) {
      if (IsSpace(c))
         continue;

      for (auto& v : GlyphVertices)
         data << n;
   }

   model->template Commit<Traits::Aim>(Abandon(data));
}

/// Generate texture coordinates for label                                    
///   @param model - the geometry instance to save data in                    
GENERATE() TextureCoords(const Mesh* model) {
   T label;
   model->GetNeat().ExtractData(label);

   const Vec4 size {
      label.mFont->GetAtlasSize(),
      label.mFont->GetAtlasSize()
   };

   TMany<Sampler2> data;
   data.Reserve(VertexCount * label.mText.GetCount());

   for (auto& c : label.mText) {
      if (IsSpace(c))
         continue;

      const auto uv = Vec4(label.mFont->GetGlyph(c).mRect) / size;
      for (auto& v : GlyphVertices)
         data << uv.xy() + v * uv.zw();
   }

   model->template Commit<Traits::Sampler>(Abandon(data));
}

GENERATE() Materials(const Mesh*) {
   TODO();
}

GENERATE() Instances(const Mesh*) {
   TODO();
}

#undef GENERATE