///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "Mesh.hpp"
#include "MeshLibrary.hpp"

#include "generators/Box.inl"
#include "generators/Cylinder.inl"
#include "generators/Foliage.inl"
#include "generators/Frustum.inl"
#include "generators/Grid.inl"
#include "generators/Label.inl"
#include "generators/Line.inl"
#include "generators/Sphere.inl"
#include "generators/Triangle.inl"
#include "generators/Zode.inl"


/// Mesh construction                                                         
///   @param producer - the producer                                          
///   @param descriptor - instructions for generator                          
Mesh::Mesh(MeshLibrary* producer, const Neat& descriptor)
   : A::Mesh {MetaOf<::Mesh>(), producer, descriptor} {
   VERBOSE_MESHES("Initializing...");

   // Get a path from the descriptor                                    
   Path filename;
   if (not descriptor.ExtractTrait<Traits::Name, Traits::Path>(filename))
      descriptor.ExtractDataAs(filename);

   if (filename) {
      // Load a filename if such was provided                           
      auto fileInterface = producer->GetFolder()->GetFile(filename);
      if (fileInterface)
         TODO();
   }
   else {
      // Consider all provided data                                     
      if (mDescriptor.ExtractData(mView)) {
         // Upload raw data if any                                      
         Bytes rawData;
         if (mDescriptor.ExtractData(rawData))
            TODO();
      }
      else {
         // Configure a generator, based on provided primitives         
         descriptor.ForEach([&](const Block& group) {
            FromPrimitive(group);
         });

         LANGULUS_ASSERT(mGenerators, Mesh,
            "No generators found in mesh");
      }
   }

   VERBOSE_MESHES("Initialized");
}

/// React on environmental change                                             
void Mesh::Refresh() {

}

/// Produce geometry data                                                     
///   @param verb - creation verb to satisfy                                  
void Mesh::Create(Verb&) {
   TODO();
}

/// Generate data                                                             
///   @param trait - the trait to generate                                    
///   @param index - trait group to generate                                  
///   @return true if data was generated                                      
bool Mesh::Generate(TMeta trait, Offset index) {
   auto foundData = GetDataListMap().FindIt(trait);
   if (foundData) {
      if (foundData.mValue->GetCount() > index)
         return true;
   }

   auto foundGen = mGenerators.FindIt(trait);
   if (foundGen) {
      (*foundGen.mValue)(this);
      foundData = GetDataListMap().FindIt(trait);
      if (foundData) {
         if (foundData.mValue->GetCount() > index)
            return true;
      }
   }

   return false;
}

/// Get level of detail mesh                                                  
///   @param lod - the level of detail state to generate LOD from             
///   @return the new geometry                                                
Ref<A::Mesh> Mesh::GetLOD(const LOD& lod) const {
   if (mLODgenerator) {
      // Generate a request, and fulfill it                             
      Verbs::Create creator {mLODgenerator(this, lod)};
      static_cast<MeshLibrary*>(mProducer)->Create(creator);
      return creator->template As<A::Mesh*>();
   }

   return const_cast<Mesh*>(this);
}

/// Create mesh generator by analyzing A::Primitive                           
///   @param data - container that contains the primitive                     
void Mesh::FromPrimitive(const Block& data) {
   if (not data.CastsTo<A::Primitive>())
      return;

   mGenerators.Clear();

   if       (FillGenerators<GenerateBox,  Box2>(data));
   else if  (FillGenerators<GenerateBox,  Box3>(data));
   else if  (FillGenerators<GenerateGrid, Grid2>(data));
   else if  (FillGenerators<GenerateGrid, Grid3>(data));
   else TODO();
}

/// Load mesh via filename/file interface                                     
///   @param descriptor - the file to load                                    
void Mesh::LoadFile(const Any& descriptor) {
   descriptor.ForEach(
      [&](const A::File&) {
         TODO();
		},
      [&](const Text& path) {
			auto file = GetRuntime()->GetFile(path);
			if (file)
            TODO();
      }
   );
}
