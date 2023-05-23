///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "../Model.hpp"


namespace Geometry::Line
{

   void GenerateIDX(Model*) {
      TODO();
   }

   /// Generate line positions                                                
   ///   @param instance - the geometry instance to save data in              
   void GeneratePOS(Model* instance) {
      // A cube made out of triangles                                   
      auto content = instance->GetData<Traits::Place>();
      if (content->Is<Line3>()) {
         const auto count = 1;
         content->Allocate(count);
         *content << Line3(Vectors::Origin, Vectors::Forward);
         instance->GetView().mPCount = content->GetCount();
         auto contentRange = instance->GetDataRange<Traits::Place>();
         *contentRange = TComplexRange<Point3>(Vectors::Origin, Vectors::Forward);
      }
      else TODO();
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
   real SDF(const Model*, const vec3& point) {
      return TLine<vec3>().SD(point);
   }

   /// Set generators for the line                                            
   void SetGenerators() {
      mSDF = Geometry::Line::SDF;
      mCodeGenerator = Geometry::Line::GenerateCODE;
   }

   /// Default line definition                                                
   ///   @return true if the default definition exists                        
   bool DefaultCreate() {
      SetTopology<ALine>();
      AddDataDeclaration<Traits::Place>(MetaOf<Line3>());
      return true;
   }

} // namespace Geometry::Line
