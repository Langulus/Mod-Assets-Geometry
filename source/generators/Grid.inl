///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright (c) 2016 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Grid.hpp"

#define GENERATE() template<CT::Grid T, CT::Topology TOPOLOGY> \
   void GenerateGrid<T, TOPOLOGY>::


/// Default grid generation                                                   
///   @param descriptor - the descriptor to use                               
///   @return a newly generated descriptor, with missing traits being set to  
///           their defaults                                                  
template<CT::Grid T, CT::Topology TOPOLOGY>
bool GenerateGrid<T, TOPOLOGY>::Default(Construct& desc) {
   auto& d = desc.GetDescriptor();

   if constexpr (CT::Line<TOPOLOGY>) {
      // A grid made out of lines                                       
      d.SetDefaultTrait<Traits::Place>(MetaOf<TLine<PointType>>());
   }
   else return false;

   d.SetDefaultTrait<Traits::Topology>(MetaOf<TOPOLOGY>());
   desc.SetType<A::Mesh>();
   return true;
}

/// Generate grid level of detail, giving a LOD state                         
///   @param model - the grid generator                                       
///   @param lod - the LOD state to generate                                  
///   @return a newly generated descriptor, for the LOD model you can use it  
///           to generate the new geometry                                    
template<CT::Grid T, CT::Topology TOPOLOGY>
Construct GenerateGrid<T, TOPOLOGY>::Detail(const Mesh* model, const LOD&) {
   return Construct::From<A::Mesh>(model->GetNeat());
}

/// Generate positions for a grid                                             
///   @param model - the model to fill                                        
GENERATE() Positions(const Mesh* model) {
   // Calculate vertex count - tesselation in this context means        
   // how many times a sector in the grid is divided in half            
   unsigned tesselation = 0;
   model->GetDescriptor().ExtractTrait<Traits::Tesselation>(tesselation);
   const Vec3u steps {Pow(2u, tesselation)};
   const auto sep = ScalarType {1} / PointType {steps};

   if constexpr (CT::Triangle<TOPOLOGY>) {
      TODO();
   }
   else if constexpr (CT::Line<TOPOLOGY>) {
      // A grid made out of lines                                       
      const auto count = (steps[0] + 1) * (steps[1] + steps[2] + 2) + (steps[1] + 1) * (steps[2] + 1);
      using E = TLine<PointType>;
      TMany<E> data;
      data.Reserve(count);

      for (Offset x = 0; x <= steps[0]; ++x) {
         const ScalarType real_x = static_cast<ScalarType>(x);

         // xsteps*ysteps                                               
         for (Offset y = 0; y <= steps[1]; ++y) {
            const Vec2 xy {
               -Half + real_x * sep[0],
               -Half + static_cast<ScalarType>(y) * sep[1]
            };

            data << E {Vec3 {xy, -Half}, Vec3 {xy, +Half}};
         }

         if constexpr (Dimensions >= 3) {
            // xsteps*zsteps                                            
            for (Offset z = 0; z <= steps[2]; ++z) {
               const ScalarType xx = -Half + real_x * sep[0];
               const ScalarType zz = -Half + static_cast<ScalarType>(z) * sep[2];

               data << E {Vec3 {xx, -Half, zz}, Vec3 {xx, +Half, zz}};
            }
         }
      }

      if constexpr (Dimensions >= 3) {
         // ysteps*zsteps                                               
         for (Offset y = 0; y <= steps[1]; ++y) {
            const ScalarType real_y = static_cast<ScalarType>(y);

            for (Offset z = 0; z <= steps[2]; ++z) {
               const Vec2 yz {
                  -Half + real_y * sep[1],
                  -Half + static_cast<ScalarType>(z) * sep[2]
               };

               data << E {Vec3 {-Half, yz}, Vec3 {+Half, yz}};
            }
         }
      }

      model->Commit<Traits::Place>(Abandon(data));
   }
   else if constexpr (CT::Point<TOPOLOGY>) {
      // Generate a grid of points                                      
      const auto count = steps[0] * steps[1] * steps[2];
      TMany<PointType> data;
      data.Reserve(count);

      for (Offset x = 0; x < steps[0]; ++x) {
         const ScalarType xx = static_cast<ScalarType>(x);

         for (Offset y = 0; y < steps[1]; ++y) {
            const ScalarType yy = static_cast<ScalarType>(y);

            if constexpr (Dimensions >= 3) {
               for (Offset z = 0; z < steps[2]; ++z) {
                  const ScalarType zz = static_cast<ScalarType>(z);

                  // A point in XYZ                                     
                  data << PointType {xx, yy, zz} * sep - Half;
               }
            }
            else {
               // A point in XY                                         
               data << PointType {xx, yy} * sep - Half;
            }
         }
      }

      model->Commit<Traits::Place>(Abandon(data));
   }
}

/// Generate normals for a grid                                               
///   @param model - the geometry instance to save data in                    
GENERATE() Normals(const Mesh*) {
   TODO();
}

/// Generate indices for a grid                                               
///   @param model - the geometry instance to save data in                    
GENERATE() Indices(const Mesh*) {
   TODO();
}

/// Generate texture coordinates for a grid                                   
///   @param model - the geometry instance to save data in                    
GENERATE() TextureCoords(const Mesh*) {
   TODO();
}

GENERATE() Materials(const Mesh*) {
   TODO();
}

GENERATE() Instances(const Mesh*) {
   TODO();
}

#undef GENERATE
