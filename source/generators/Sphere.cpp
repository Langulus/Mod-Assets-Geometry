///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "../Model.hpp"


namespace Geometry::Sphere
{

   ///                                                                        
   ///   PLATONIC ICOSAHEDRON                                                   
   ///                                                                        
   /// Icosahedron's constant occurences                                       
   constexpr real Half = real(1) / real(2);

   constexpr real IcosahedronX = 1 / (2 * pcSqrt(real(5)));
   constexpr real IcosahedronZ = 2 / (2 * pcSqrt(real(5)));

   constexpr pcptr VertexCount = 12;
   constexpr pcptr TriangleCount = 20;
   constexpr pcptr TriangleIndexCount = TriangleCount * 3;

   /// Icosahedron's unique vertices                                          
   const vec3 IcosahedronVertices[VertexCount] = {
      vec3(-IcosahedronX,   0, IcosahedronZ),
      vec3(IcosahedronX, 0, IcosahedronZ),
      vec3(-IcosahedronX, 0, -IcosahedronZ),
      vec3(IcosahedronX, 0, -IcosahedronZ),
      vec3(0, IcosahedronZ, IcosahedronX),
      vec3(0, IcosahedronZ, -IcosahedronX),
      vec3(0, -IcosahedronZ, IcosahedronX),
      vec3(0, -IcosahedronZ, -IcosahedronX),
      vec3(IcosahedronZ, IcosahedronX, 0),
      vec3(-IcosahedronZ, IcosahedronX, 0),
      vec3(IcosahedronZ, -IcosahedronX, 0),
      vec3(-IcosahedronZ, -IcosahedronX, 0)
   };

   /// Icosahedron's unique texture coords                                    
   const vec3 IcosahedronUVW[VertexCount] = {
      IcosahedronVertices[0] + Half,
      IcosahedronVertices[1] + Half,
      IcosahedronVertices[2] + Half,
      IcosahedronVertices[3] + Half,
      IcosahedronVertices[4] + Half,
      IcosahedronVertices[5] + Half,
      IcosahedronVertices[6] + Half,
      IcosahedronVertices[7] + Half,
      IcosahedronVertices[8] + Half,
      IcosahedronVertices[9] + Half,
      IcosahedronVertices[10] + Half,
      IcosahedronVertices[11] + Half
   };

   /// Indices for the 20 icosahedron faces                                    
   const pcu32 IcosahedronIndices[TriangleCount][3] = {
      {0,4,1},  {0,9,4},  {9,5,4},  {4,5,8},  {4,8,1},
      {8,10,1}, {8,3,10}, {5,3,8},  {5,2,3},  {2,7,3},
      {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6},
      {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5},  {7,2,11}
   };

   /// Generate geosphere vertices                                             
   ///   @param instance - the geometry to generate positions for               
   void GeneratePOS(Model* instance) {
      // A geosphere made out of triangles/lines/points                  
      // Note, that this geometry is always indexed                     
      auto content = instance->GetData<Traits::Position>();
      if (instance->CheckTopology<ATriangle>()) {
         if (content->Is<Point3>()) {
            content->Allocate(VertexCount);
            for (int i = 0; i < VertexCount; ++i)
               *content << Point3(IcosahedronVertices[i]);
            auto contentRange = instance->GetDataRange<Traits::Position>();
            *contentRange = TComplexRange<Point3>(-Half, +Half);
         }
         else TODO();
      }
      else TODO();

      instance->GetView().mPCount = content->GetCount();
   }

   /// Generate geosphere normals                                             
   ///   @param instance - the geometry to generate normals for               
   void GenerateNOR(Model* instance) {
      auto content = instance->GetData<Traits::Aim>();
      if (instance->CheckTopology<ATriangle>()) {
         if (content->Is<Normal>()) {
            content->Allocate(VertexCount);
            for (int i = 0; i < VertexCount; ++i)
               *content << Normal(IcosahedronVertices[i]);
            auto contentRange = instance->GetDataRange<Traits::Aim>();
            *contentRange = TComplexRange<Normal>(Normal {-1}, Normal {+1});
         }
         else TODO();
      }
      else TODO();
   }

   /// Generate geosphere texture coordinates                                 
   ///   @param instance - the geometry to generate texture coordinates for   
   void GenerateTEX(Model* instance) {
      auto content = instance->GetData<Traits::Sampler>();
      if (instance->CheckTopology<ATriangle>()) {
         if (content->Is<Sampler3>()) {
            content->Allocate(VertexCount);
            for (int i = 0; i < VertexCount; ++i)
               *content << Sampler3(IcosahedronUVW[i]);
            auto contentRange = instance->GetDataRange<Traits::Sampler>();
            *contentRange = TComplexRange<Sampler3>(0, 1);
         }
         else TODO();
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

   /// Generate geosphere indices                                             
   ///   @param instance - the geometry to generate indices for               
   void GenerateIDX(Model* instance) {
      auto content = instance->GetData<Traits::Index>();
      if (instance->CheckTopology<ATriangle>()) {
         if (content->Is<pcu32>()) {
            // A geosphere made out of triangles                        
            content->Allocate(TriangleIndexCount);
            for (int i = 0; i < TriangleCount; ++i) {
               *content << IcosahedronIndices[i][0];
               *content << IcosahedronIndices[i][1];
               *content << IcosahedronIndices[i][2];
            }
            auto contentRange = instance->GetDataRange<Traits::Index>();
            *contentRange = TRange<pcu32>(0u, TriangleIndexCount - 1);
         }
         else if (instance->CheckTopology<ALine>()) {
            // A geosphere made out of lines                              
            TODO();
         }
         else if (instance->CheckTopology<APoint>()) {
            // A geosphere made out of points                           
            TODO();
         }
         else TODO();
      }
      else TODO();

      instance->GetView().mICount = content->GetCount();
   }

   /// Generate geosphere code                                                
   ///   @param instance - the geometry to generate indices for               
   void GenerateCODE(Model*) {
      TODO();
   }

   /// Tesselate the geosphere                                                
   ///   @param instance - the geometry to tesselate                           
   void Tesselate(Model* instance) {
      // Use the standard tesselation first                              
      CGeneratorGeometry::DefaultTesselator(instance);

      // And since this is a sphere - normalize positions & samplers      
      instance->Normalize(Traits::Position::ID, true);
      instance->Normalize(Traits::Sampler::ID, true);
   }

   /// LOD function for a geosphere                                             
   ///   @param instance - the geometry to get LOD of                           
   ///   @param lod - lod state and info required to compute the LOD            
   ///   @return the resulting LOD content                                    
   const Model* LOD(const Model* instance, const LodState& lod) {
      if (lod.mLODIndex == 0) {
         // On zero LOD index, we're at optimal distance, so we return   
         // the original unmodified geometry                              
         return instance;
      }

      const auto tesselation = instance->GetTesselation();
      if (tesselation > 0 && lod.mLODIndex < 0) {
         // Find a lower tesselation of the geosphere, because the LOD   
         // is for an object that is further away                        
         auto newTesselation = tesselation + lod.mLODIndex;
         if (newTesselation < 0)
            newTesselation = 0;

         // Clone only the content descriptor                           
         auto newContent = instance->GetConstruct().Clone();

         // Overwrite the tesselation. This will cause a rehash         
         newContent <<= Trait::From<Traits::Tesselation>(pcptr(std::round(newTesselation)));
         pcLogFuncVerbose << "Creating LOD: " << newContent;
         pcLogFuncVerbose << "With Base: " << instance->GetConstruct();

         // Create content generator                                    
         Any result;
         if (!instance->GetProducer()->Generate<CGeometrySphere>(newContent, result) || result.IsEmpty()) {
            pcLogFuncError << "Can't produce LOD(" << lod.mLODIndex << ") for " << instance;
            return instance;
         }

         return result.As<CGeneratorGeometry*>();
      }
      else if (lod.mLODIndex > 0) {
         // LOD is for a sphere that is very close. At this point the   
         // sphere turns into a plane to the observer. For optimization   
         // purposes, this plane is made of four triangular patches      
         // that are centered on the camera, and facing outwards the      
         // sphere. These so called 'zodes' are designed to be indexed   
         // from the center towards the edges and are optimized         
         // especially for planet & convex terrain rendering            

         // Calculate the angle between the tangents to the sphere      
         const auto cosTheta = 1 / (1 + lod.GetNormalizedDistance());

         // Calculate the visible spherical cap area, and the required   
         // number of such areas to cover the whole sphere               
         const auto taurr = TAU<real> * lod.mRadius * lod.mRadius;
         const auto area = 2 * taurr * (real(1) - cosTheta);
         const auto areaFraction = pcAbs(area / taurr) * 2;
         const auto areasToCover = pcCeil(1 / areaFraction);

         // Make a stepping normal that is somewhat snapped to the      
         // theta this way we will avoid generating content on each      
         // small reorientation                                          
         const auto intersection = lod.mView.GetPosition() - lod.mModel.GetPosition();
         if (intersection.Length() == 0)
            return instance;
         const auto steppingNormal = (intersection.Normalize() * areasToCover).Round();

         // Clone only the descriptor and change type                     
         auto newContent = instance->GetConstruct().CloneAs<CGeometryZode>();
         // Set the place around which the zode is centered               
         newContent <<= Trait::From<Traits::Position>(steppingNormal);
         // Set the size of the sphere in order to calculate curvature   
         newContent <<= Trait::From<Traits::Scale>(lod.mModel.GetScale());
         // Set the topology                                             
         newContent <<= Trait::From<Traits::Topology>(ATriangleStrip::ID);
         // Set the tesselation                                          
         newContent <<= Trait::From<Traits::Tesselation>(tesselation + pcptr(lod.mLODIndex) / 2);
         pcLogFuncVerbose << "Creating LOD: " << newContent;

         // The octave gets progressively lower until we reach state      
         //real octave = 0;
         //newContent.Value(TraitID_Octave::Meta(), octave);
         //octave = pcLerp(octave, real(lod.mOctave), lodIndex / real(6));
         //newContent.SetTrait<TraitID_Octave>(pcptr(octave));

         // Overwrite the tesselation                                    

         // Create content generator for all four parts                  
         Any result;
         if (!instance->GetProducer()->Generate<CGeometryZode>(newContent, result) || result.IsEmpty()) {
            pcLogFuncError << "Can't produce LOD(" << lod.mLODIndex << ") for " << instance;
            return instance;
         }

         return result.As<Model*>();
      }

      return instance;
   }

   /// Signed distance function for a geosphere                                 
   ///   @param instance - the generator                                       
   ///   @param point - the sampling point                                    
   ///   @return the distance to the geosphere at the given point               
   real SDF(const Model*, const vec3& point) {
      return TSphere<vec3>().SD(point);
   }

   /// Set generators for the sphere                                             
   void SetGenerators() {
      mVertexGenerator = Geometry::Sphere::GeneratePOS;
      mTesselator = Geometry::Sphere::Tesselate;
      mLodGenerator = Geometry::Sphere::LOD;
      mNormalGenerator = Geometry::Sphere::GenerateNOR;
      mTangentGenerator = DefaultTangentGenerator;
      mBitangentGenerator = DefaultBitangentGenerator;
      mTextureCoordGenerator = Geometry::Sphere::GenerateTEX;
      mInstanceGenerator = Geometry::Sphere::GenerateINS;
      mRotationGenerator = Geometry::Sphere::GenerateROT;
      mColorGenerator = Geometry::Sphere::GenerateCOL;
      mIndexGenerator = Geometry::Sphere::GenerateIDX;
      mSDF = Geometry::Sphere::SDF;
      mCodeGenerator = Geometry::Sphere::GenerateCODE;
   }

   /// Default sphere definition                                                   
   ///   @return true if the default definition exists                           
   bool DefaultCreate() {
      SetTopology<ATriangle>();
      SetTextureMapper(Mapper::Model);
      AddDataDeclaration<Traits::Position>(MetaData::Of<Triangle3>());
      AddDataDeclaration<Traits::Aim>(MetaData::Of<Normal>());
      AddDataDeclaration<Traits::Sampler>(MetaData::Of<Sampler3>());
      return true;
   }

} // namespace Geometry::Geosphere
