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
Mesh::Mesh(MeshLibrary* producer, const Descriptor& descriptor)
   : A::Mesh {MetaOf<::Mesh>(), producer, descriptor} {
   VERBOSE_MESHES("Initializing...");

   // Parse the descriptor for a filename                               
   Path filename;
   descriptor.ForEachDeep(
      [&](const Text& text) {
         filename = text;
      },
      [&](const Trait& trait) {
         if (trait.TraitIs<Traits::Name, Traits::Path>())
            filename = trait.template AsCast<Text>();
      }
   );

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
void Mesh::Create(Verb& verb) {

}

/// Get level of detail mesh                                                  
///   @param lod - the level of detail state to generate LOD from             
///   @return the new geometry                                                
Ref<A::Mesh> Mesh::GetLOD(const LOD& lod) const {
   TODO();
}

/// Create mesh generator by analyzing A::Primitive                           
///   @param data - container that contains the primitive                     
void Mesh::FromPrimitive(const Block& data) {
   if (!data.CastsTo<A::Primitive>())
      return;

   mGenerators.Clear();

   if       (FillGenerators<GeneratorBox,  Box2>(data));
   else if  (FillGenerators<GeneratorBox,  Box3>(data));
   else if  (FillGenerators<GeneratorGrid, Grid2>(data));
   else if  (FillGenerators<GeneratorGrid, Grid3>(data));
   else TODO();
}

/// Load mesh via filename/file interface                                     
///   @param descriptor - the file to load                                    
void Mesh::LoadFile(const Any& descriptor) {
   descriptor.ForEach(
      [&](const A::File& file) {
         TODO();
		},
      [&](const Text& path) {
			auto file = GetRuntime()->GetFile(path);
			if (file)
            TODO();
      }
   );
}
