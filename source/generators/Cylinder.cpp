///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "../Model.hpp"

namespace Geometry::Cylinder
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

   /// LOD function for the cylinder                                          
   const Model* LOD(const Model*, const LodState&) {
      TODO();
   }

   /// Signed distance function for a cylinder                                 
   ///   @param instance - the generator                                       
   ///   @param point - the sampling point                                    
   ///   @return the distance to the cylinder at the given point               
   real SDF(const Model*, const vec3& point) {
      return TCylinder<vec3>().SD(point);
   }

   /// Set generators for the cylinder                                        
   void SetGenerators() {
      mSDF = Geometry::Cylinder::SDF;
      mCodeGenerator = Geometry::Cylinder::GenerateCODE;
   }

   /// Default cylinder definition                                            
   ///   @return true if the default definition exists                        
   bool DefaultCreate() {
      SetTopology<ATriangle>();
      SetTextureMapper(Mapper::Cylinder);
      AddDataDeclaration<Traits::Position>(MetaData::Of<Triangle3>());
      AddDataDeclaration<Traits::Aim>(MetaData::Of<Normal>());
      AddDataDeclaration<Traits::Sampler>(MetaData::Of<Sampler2>());
      return true;
   }

} // namespace Geometry::Cylinder
