///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "../Model.hpp"


namespace Geometry::Tree
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

   /// LOD function                                                           
   const Model* LOD(const Model* instance, const LOD&) {
      return instance;
   }

   /// Signed distance function                                               
   ///   @param instance - the generator                                      
   ///   @param point - the sampling point                                    
   ///   @return the distance to the geometry at the given point              
   Real SDF(const Model*, const Vec3&) {
      TODO();
   }

   /// Set generators for the tree                                            
   void SetGenerators() {
      mSDF = Geometry::Tree::SDF;
      mCodeGenerator = Geometry::Tree::GenerateCODE;
   }

   /// Default tree definition                                                
   ///   @return true if the default definition exists                        
   bool DefaultCreate() {
      SetTopology<ATriangle>();
      SetTextureMapper(Mapper::Custom);
      AddDataDeclaration<Traits::Place>(MetaData::Of<Triangle3>());
      AddDataDeclaration<Traits::Aim>(MetaData::Of<Normal>());
      AddDataDeclaration<Traits::Sampler>(MetaData::Of<Sampler2>());
      return true;
   }

} // namespace Geometry::Tree
