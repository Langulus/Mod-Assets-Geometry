///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "../Model.hpp"
#include <Math/Primitives/TTriangle.hpp>
#include <Math/Primitives/TLine.hpp>
#include <Math/Mapping.hpp>
#include <Math/Colors.hpp>


void GenerateIDX(Model*) {
   TODO();
}

void GeneratePOS(Model* instance) {
   auto content = instance->GetData<Traits::Position>();

   if (instance->CheckTopology<ATriangle>()) {
      // A triangle made out of triangles                              
      if (content->Is<Triangle3>()) {
         content->Allocate(1);
         *content << Triangle3(
            vec3(-0.5, -0.5,  0.0), 
            vec3( 0.0,  0.5,  0.0), 
            vec3( 0.5,  0.0,  0.0)
         );

         auto contentRange = instance->GetDataRange<Traits::Position>();
         *contentRange = TComplexRange<Point3>(vec3(-0.5, -0.5, 0.0), vec3(0.5, 0.5, 0.0));
      }
      else TODO();
   }
   else if (instance->CheckTopology<ALine>()) {
      // A triangle made out of lines                                 
      TODO();
   }
   else if (instance->CheckTopology<APoint>()) {
      // A triangle made out of points                                 
      TODO();
   }
   else TODO();

   instance->GetView().mPCount = content->GetCount();
}

void GenerateNOR(Model* instance) {
   auto content = instance->GetData<Traits::Aim>();
   if (instance->CheckTopology<ATriangle>()) {
      // A rectangle made out of triangles                           
      if (content->Is<Normal>()) {
         content->Allocate(3);
         *content << Normal(Vectors::Backward<real>);
         *content << Normal(Vectors::Backward<real>);
         *content << Normal(Vectors::Backward<real>);

         auto contentRange = instance->GetDataRange<Traits::Aim>();
         *contentRange = TComplexRange<Normal>(Normal {Vectors::Backward<real>});
      }
      else TODO();
   }
   else TODO();
}

void GenerateTEX(Model* instance) {
   auto content = instance->GetData<Traits::Sampler>();
   if (instance->CheckTopology<ATriangle>()) {
      // A rectangle made out of triangles                           
      if (content->Is<Sampler2>()) {
         content->Allocate(3);
         *content << Sampler2(0, 0);
         *content << Sampler2(vec2(1, 2).Normalize());
         *content << Sampler2(1, 0);

         auto contentRange = instance->GetDataRange<Traits::Sampler>();
         *contentRange = TComplexRange<Sampler2>(0, 1);
      }
      else TODO();
   }
   else if (instance->CheckTopology<ALine>()) {
      // A triangle made out of lines                                 
      TODO();
   }
   else if (instance->CheckTopology<APoint>()) {
      // A triangle made out of points                                 
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

void GenerateCOL(Model*) {
   TODO();
}

void GenerateCODE(Model*) {
   TODO();
}

/// LOD function                                                            
const Model* LOD(const Model* instance, const LodState&) {
   return instance;
}

/// Signed distance function                                                
///   @param instance - the generator                                       
///   @param point - the sampling point                                    
///   @return the distance to the geometry at the given point               
real SDF(const Model*, const vec3& point) {
   return TTriangle<vec3>().SD(point);
}

/// Set generators for the triangle                                             
void SetGenerators() {
   mSDF = Geometry::Triangle::SDF;
   mCodeGenerator = Geometry::Triangle::GenerateCODE;
}

/// Default triangle definition                                                
///   @return true if the default definition exists                           
bool DefaultCreate() {
   SetTopology<ATriangle>();
   SetTextureMapper(Mapper::Face);
   AddDataDeclaration<Traits::Position>(MetaData::Of<Triangle3>());
   AddDataDeclaration<Traits::Aim>(MetaData::Of<Normal>());
   AddDataDeclaration<Traits::Sampler>(MetaData::Of<Sampler2>());
   return true;
}