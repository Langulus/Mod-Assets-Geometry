///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "../Model.hpp"


namespace Geometry::Frustum
{

   void GenerateIDX(Model*) {
      TODO();
   }

   void GeneratePOS(Model*) {
      TODO();
   }

   void GenerateNOR(Model*) {
      TODO();
   }

   void GenerateTEX(Model*) {
      TODO();
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

   const Model* LOD(const Model* instance, const LOD&) {
      return instance;
   }

   /// Signed distance function for a frustum                                 
   ///   @param instance - the generator                                      
   ///   @param point - the sampling point                                    
   ///   @return the distance to the frustum at the given point               
   real SDF(const Model*, const vec3& point) {
      return TFrustum<vec3>().SD(point);
   }

   /// Set generators for the frustum                                         
   void SetGenerators() {
      mSDF = Geometry::Frustum::SDF;
      mCodeGenerator = Geometry::Frustum::GenerateCODE;
   }

   /// Default frustum definition                                             
   ///   @return true if the default definition exists                        
   bool DefaultCreate() {
      SetTopology<ATriangle>();
      SetTextureMapper(Mapper::Cubic);
      AddDataDeclaration<Traits::Place>(MetaData::Of<Triangle3>());
      AddDataDeclaration<Traits::Aim>(MetaData::Of<Normal>());
      AddDataDeclaration<Traits::Sampler>(MetaData::Of<Sampler2>());
      return true;
   }

} // namespace Geometry::Frustum
