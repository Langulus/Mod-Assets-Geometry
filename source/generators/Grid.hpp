///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "../Mesh.hpp"
#include <Math/Primitives/TLine.hpp>
#include <Math/Primitives/TPoint.hpp>

template<CT::Vector T>
struct TGrid;

namespace Langulus
{
   namespace A
   {

      /// An abstract grid                                                    
      struct Grid {
         LANGULUS(ABSTRACT) true;
         LANGULUS(CONCRETE) TGrid<Vec3>;
      };

   } // namespace Langulus::A

   namespace CT
   {

      /// Concept for distinguishing grids                                    
      template<class... T>
      concept Grid = (DerivedFrom<T, A::Grid> && ...);

   } // namespace Langulus::CT

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
/// | +-----|-+-----|-+ |  | mCellSize.y                                   |  
/// |/      |/      |/| |  v                                               |  
/// +-------+-------+ | +---                                               |  
/// | |/    | |/    | |/                                                   |  
/// | +-----|-+-----|-+---                                                 |  
/// |/      |/      |/   / mCellSize.z                                     |  
/// +-------+-------+   /                                                  |  
///         |<----->| mCellSize.x                                          |  
///                                                                        |  
template<CT::Vector T>
struct TGrid : A::Grid {
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
///   @tparam T - the primitve to use for point type and dimensions           
///   @tparam TOPOLOGY - are we generating triangles/lines/points?            
///                                                                           
template<CT::Grid T, CT::Topology TOPOLOGY = A::Triangle>
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

#include "Grid.inl"