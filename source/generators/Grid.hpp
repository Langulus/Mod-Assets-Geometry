///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright (c) 2016 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Mesh.hpp"
#include <Langulus/Math/Primitives/Line.hpp>

template<CT::Vector>
struct TGrid;

using Grid2 = TGrid<Vec2>;
using Grid3 = TGrid<Vec3>;


namespace Langulus
{
   namespace A
   {
      /// An abstract grid                                                    
      struct Grid : A::Primitive {
         LANGULUS(ABSTRACT) true;
         LANGULUS(CONCRETE) TGrid<Vec3>;
         LANGULUS_BASES(A::Primitive);
      };
   }

   namespace CT
   {
      /// Concept for distinguishing grids                                    
      template<class...T>
      concept Grid = (DerivedFrom<T, A::Grid> and ...);
   }

} // namespace Langulus

///                                                                        |  
/// 2D/3D grid, centered around origin                                     |  
///                                                                        |  
///                                                                        |  
///             ^ +Y      / +Z                                             |  
///             |        /                                                 |  
///     +-------+-------+---> +X                                           |  
///    /       /       /|                 mExtent is the number of cells   |  
///   +-------+-------+ |                 you could count, from the center |  
///  /       /       /| |                 to each other direction          |  
/// +-------+-------+ | +---                                               |  
/// | |/    | |/    | |/|  ^                                               |  
/// | +-----|-*-----|-+ |  | mCellSize.y                                   |  
/// |/      |/origin|/| |  v                                               |  
/// +-------+-------+ | +---                                               |  
/// | |/    | |/    | |/                                                   |  
/// | +-----|-+-----|-+---                                                 |  
/// |/      |/      |/   / mCellSize.z                                     |  
/// +-------+-------+   /                                                  |  
///         |<----->| mCellSize.x                                          |  
///                                                                        |  
template<CT::Vector T>
struct TGrid : A::Grid {
private:
   static consteval auto GenerateToken() {
      constexpr auto defaultClassName = RTTI::LastCppNameOf<TGrid>();
      ::std::array<char, defaultClassName.size() + 1> name {};
      ::std::size_t offset {};

      if constexpr (T::MemberCount > 3) {
         for (auto i : defaultClassName)
            name[offset++] = i;
         return name;
      }

      // Write prefix                                                   
      for (auto i : "Grid")
         name[offset++] = i;

      // Write size                                                     
      --offset;
      name[offset++] = '0' + T::MemberCount;

      // Write suffix                                                   
      for (auto i : SuffixOf<TypeOf<T>>())
         name[offset++] = i;
      return name;
   }

public:
   LANGULUS(NAME) GenerateToken();
   LANGULUS(ABSTRACT) false;
   LANGULUS(POD) CT::POD<T>;
   LANGULUS(TYPED) TypeOf<T>;
   LANGULUS_BASES(A::Grid);

   using PointType = T;
   static constexpr Count MemberCount = T::MemberCount;
   static_assert(MemberCount > 1, "Can't have one-dimensional grid");

   T mCellSize {1};
   TVector<Count, MemberCount> mExtent {5};
};


///                                                                           
///    Grid mesh generators                                                   
///                                                                           
///   @tparam T - the primitive to use for point type and dimensions          
///   @tparam TOPOLOGY - are we generating triangles/lines/points?            
///                                                                           
template<CT::Grid T, CT::Topology TOPOLOGY = A::Line>
struct GenerateGrid {
   using PointType = typename T::PointType;
   using ScalarType = TypeOf<PointType>;
   static constexpr Count Dimensions = T::MemberCount;
   static constexpr ScalarType Half = ScalarType {1} / ScalarType {2};

   static bool Default(Construct&);
   static auto Detail(const Mesh*, const LOD&) -> Construct;

   static void Indices(Mesh*);
   static void Positions(Mesh*);
   static void Normals(Mesh*);
   static void TextureCoords(Mesh*);
   static void Materials(Mesh*);
   static void Instances(Mesh*);
};

#include "Grid.inl"