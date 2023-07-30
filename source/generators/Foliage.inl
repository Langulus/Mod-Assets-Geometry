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
#include <Math/Mapping.hpp>


///                                                                           
///    Foliage mesh generators                                                
///                                                                           
///   @tparam T - the primitve to use for point type and dimensions           
///   @tparam TOPOLOGY - are we generating triangles/lines/points?            
///                                                                           
template<CT::Foliage T, CT::Topology TOPOLOGY = A::Triangle>
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

#define GENERATE() template<CT::Foliage T, CT::Topology TOPOLOGY> \
   void Generate<T, TOPOLOGY>::


/// Default foliage generation                                                
///   @param descriptor - the descriptor to use                               
///   @return a newly generated descriptor, with missing traits being set to  
///           their defaults                                                  
template<CT::Foliage T, CT::Topology TOPOLOGY>
Normalized Generate<T, TOPOLOGY>::Default(Descriptor&& descriptor) {
   Normalized d {descriptor};

   if constexpr (CT::Triangles<TOPOLOGY>) {
      // Foliage made out of triangles                                  
      d.SetDefaultTrait<Traits::Topology>(
         MetaOf<TOPOLOGY>());
      d.SetDefaultTrait<Traits::Place>(
         MetaOf<TTriangle<PointType>>());
   }
   else LANGULUS_ERROR("Unsupported topology for line");

   return Abandon(d);
}

/// Generate foliage level of detail, giving a LOD state                      
///   @param model - the foliage generator                                    
///   @param lod - the LOD state to generate                                  
///   @return a newly generated descriptor, for the LOD model you can use it  
///           to generate the new geometry                                    
template<CT::Foliage T, CT::Topology TOPOLOGY>
Normalized Generate<T, TOPOLOGY>::Detail(const Mesh* model, const LOD&) {
   return model->GetDescriptor();
}

/// Generate positions for foliage                                            
///   @param model - the model to fill                                        
GENERATE() Positions(Mesh* model) {
   TODO();
}

/// Generate normals for foliage                                              
///   @param model - the geometry instance to save data in                    
GENERATE() Normals(Mesh* model) {
   TODO();
}

/// Generate indices for foliage                                              
///   @param model - the geometry instance to save data in                    
GENERATE() Indices(Mesh* model) {
   TODO();
}

/// Generate texture coordinates for foliage                                  
///   @param model - the geometry instance to save data in                    
GENERATE() TextureCoords(Mesh* model) {
   TODO();
}

GENERATE() TextureIDs(Mesh*) {
   TODO();
}

GENERATE() Instances(Mesh*) {
   TODO();
}

GENERATE() Rotations(Mesh*) {
   TODO();
}

GENERATE() Colors(Mesh* model) {
   TODO();
}

#undef GENERATE