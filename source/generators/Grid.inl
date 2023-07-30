///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Grid.hpp"

#define GENERATE() template<CT::Grid T, CT::Topology TOPOLOGY> \
   void Generate<T, TOPOLOGY>::


/// Default grid generation                                                   
///   @param descriptor - the descriptor to use                               
///   @return a newly generated descriptor, with missing traits being set to  
///           their defaults                                                  
template<CT::Grid T, CT::Topology TOPOLOGY>
Normalized Generate<T, TOPOLOGY>::Default(Descriptor&& descriptor) {
   Normalized d {descriptor};
   if constexpr (CT::Line<TOPOLOGY>) {
      // A grid made out of lines                                       
      d.SetDefaultTrait<Traits::Topology>(
         MetaOf<TOPOLOGY>());
      d.SetDefaultTrait<Traits::Place>(
         MetaOf<TLine<PointType>>());
   }
   else LANGULUS_ERROR("Unsupported topology for line");
   return Abandon(d);
}

/// Generate grid level of detail, giving a LOD state                         
///   @param model - the grid generator                                       
///   @param lod - the LOD state to generate                                  
///   @return a newly generated descriptor, for the LOD model you can use it  
///           to generate the new geometry                                    
template<CT::Grid T, CT::Topology TOPOLOGY>
Normalized Generate<T, TOPOLOGY>::Detail(const Mesh* model, const LOD&) {
   return model->GetDescriptor();
}

/// Generate positions for a grid                                             
///   @param model - the model to fill                                        
GENERATE() Positions(Mesh* model) {
   // Calculate vertex count - tesselation in this context means        
   // how many times a sector in the grid is divided in half            
   const vec4 resolution = pcPow(real(2), instance->GetTesselation());
   const auto xsteps = pcptr(resolution[0]);
   const auto ysteps = pcptr(resolution[1]);
   const auto zsteps = pcptr(resolution[2]);
   const auto sep = vec3 {1} / vec3 {real(xsteps), real(ysteps), real(zsteps)};

   if constexpr (CT::Line<TOPOLOGY>) {
      // A grid made out of lines                                       
      const auto count = (xsteps + 1) * (ysteps + zsteps + 2) + (ysteps + 1) * (zsteps + 1);
      using Scalar = TypeOf<PointType>;
      using E = TLine<PointType>;
      TAny<E> data;
      data.Reserve(count);

      for (Offset x = 0; x <= xsteps; ++x) {
         const Scalar real_x {x};

         // xsteps*ysteps                                               
         for (Offset y = 0; y <= ysteps; ++y) {
            const vec2 xy {
               -Half + real_x * sep[0],
               -Half + real(y) * sep[1]
            };

            data << E {Vec3 {xy, -Half}, Vec3 {xy, +Half}};
         }

         if constexpr (Dimensions >= 3) {
            // xsteps*zsteps                                            
            for (Offset z = 0; z <= zsteps; ++z) {
               const Scalar xx = -Half + real_x * sep[0];
               const Scalar zz = -Half + real(z) * sep[2];

               data << E {Vec3 {xx, -Half, zz}, Vec3 {xx, +Half, zz}};
            }
         }
      }

      if constexpr (Dimensions >= 3) {
         // ysteps*zsteps                                               
         for (Offset y = 0; y <= ysteps; ++y) {
            const Scalar real_y {y};

            for (Offset z = 0; z <= zsteps; ++z) {
               const Vec2 yz {
                  -Half + real_y * sep[1],
                  -Half + real(z) * sep[2]
               };

               data << E {Vec3 {-Half, yz}, Vec3 {+Half, yz}};
            }
         }
      }

      model->Commit<Traits::Place>(Abandon(data));
   }
   else if constexpr (CT::Point<TOPOLOGY>) {
      // Generate a grid of points                                      
      const auto count = xsteps * ysteps * zsteps;
      using Scalar = TypeOf<PointType>;
      TAny<PointType> data;
      data.Reserve(count);

      for (Offset x = 0; x < xsteps; ++x) {
         const Scalar xx {x};

         for (Offset y = 0; y < ysteps; ++y) {
            const Scalar yy {y};

            if constexpr (Dimensions >= 3) {
               for (Offset z = 0; z < zsteps; ++z) {
                  const Scalar zz {z};

                  // A point in XYZ                                     
                  data << E {xx, yy, zz} * sep - Half<Scalar>;
               }
            }
            else {
               // A point in XY                                         
               data << E {xx, yy} * sep - Half<Scalar>;
            }
         }
      }

      model->Commit<Traits::Place>(Abandon(data));
   }
   else LANGULUS_ERROR("Unsupported topology for grid positions");
}

/// Generate normals for a grid                                               
///   @param model - the geometry instance to save data in                    
GENERATE() Normals(Mesh* model) {
   TODO();
}

/// Generate indices for a grid                                               
///   @param model - the geometry instance to save data in                    
GENERATE() Indices(Mesh* model) {
   TODO();
}

/// Generate texture coordinates for a grid                                   
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
