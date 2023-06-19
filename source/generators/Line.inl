///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "../Model.hpp"
#include <Math/Primitives/TLine.hpp>


///                                                                           
///    Line mesh generators                                                   
///                                                                           
///   @tparam T - the primitve to use for point type and dimensions           
///   @tparam TOPOLOGY - are we generating triangles/lines/points?            
///                                                                           
template<CT::Line T, CT::Topology TOPOLOGY = A::Triangle>
struct Generate {
   using PointType = typename T::PointType;
   static constexpr Count Dimensions = T::MemberCount;

   NOD() static Normalized Default(Descriptor&&);
   NOD() static Normalized Detail(const Model*, const LOD&);

   static void Indices(Model*);
   static void Positions(Model*);
   static void Normals(Model*);
   static void TextureCoords(Model*);
   static void TextureIDs(Model*);
   static void Instances(Model*);
   static void Rotations(Model*);
   static void Colors(Model*);
};

#define GENERATE() template<CT::Line T, CT::Topology TOPOLOGY> \
   void Generate<T, TOPOLOGY>::


/// Default line generation                                                   
///   @param descriptor - the descriptor to use                               
///   @return a newly generated descriptor, with missing traits being set to  
///           their defaults                                                  
template<CT::Line T, CT::Topology TOPOLOGY>
Normalized Generate<T, TOPOLOGY>::Default(Descriptor&& descriptor) {
   Normalized d {descriptor};

   if constexpr (CT::Line<TOPOLOGY>) {
      // A line made out of lines (duh)                                 
      d.SetDefaultTrait<Traits::Topology>(
         MetaOf<TOPOLOGY>());
      d.SetDefaultTrait<Traits::Place>(
         MetaOf<TLine<PointType>>());
   }
   else LANGULUS_ERROR("Unsupported topology for line");

   return Abandon(d);
}

/// Generate line level of detail, giving a LOD state                         
///   @param model - the line generator                                       
///   @param lod - the LOD state to generate                                  
///   @return a newly generated descriptor, for the LOD model you can use it  
///           to generate the new geometry                                    
template<CT::Line T, CT::Topology TOPOLOGY>
Normalized Generate<T, TOPOLOGY>::Detail(const Model* model, const LOD&) {
   return model->GetDescriptor();
}

/// Generate positions for a line                                             
///   @param model - the model to fill                                        
GENERATE() Positions(Model* model) {
   if constexpr (CT::Line<TOPOLOGY>) {
      // A line made out of lines                                       
      using E = TLine<PointType>;
      TAny<E> data;
      data << E {Vectors::Origin, Vectors::Forward};
      model->Commit<Traits::Place>(Abandon(data));
   }
   else LANGULUS_ERROR("Unsupported topology for line positions");
}

/// Generate normals for a line                                               
///   @param model - the geometry instance to save data in                    
GENERATE() Normals(Model* model) {
   TODO();
}

/// Generate indices for a line                                               
///   @param model - the geometry instance to save data in                    
GENERATE() Indices(Model* model) {
   TODO();
}

/// Generate texture coordinates for a line                                   
///   @param model - the geometry instance to save data in                    
GENERATE() TextureCoords(Model* model) {
   TODO();
}

GENERATE() TextureIDs(Model*) {
   TODO();
}

GENERATE() Instances(Model*) {
   TODO();
}

GENERATE() Rotations(Model*) {
   TODO();
}

GENERATE() Colors(Model* model) {
   TODO();
}

#undef GENERATE