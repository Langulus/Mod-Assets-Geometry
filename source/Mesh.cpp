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

#include <Langulus/IO.hpp>


/// Mesh construction                                                         
///   @param producer - the producer                                          
///   @param descriptor - instructions for generator                          
Mesh::Mesh(MeshLibrary* producer, const Neat& descriptor)
   : A::Mesh {MetaOf<::Mesh>(), producer, descriptor} {
   // Get a path from the descriptor                                    
   VERBOSE_MESHES("Initializing...");
   Path filename;
   if (not descriptor.ExtractTrait<Traits::Name, Traits::Path>(filename))
      descriptor.ExtractDataAs(filename);

   if (filename) {
      // Load a filename if such was provided                           
      auto fileInterface = producer->GetFolder()->RelativeFile(filename);
      if (fileInterface)
         ReadOBJ(*fileInterface);
   }
   else {
      // Consider all provided data                                     
      if (descriptor.ExtractData(mView)) {
         // Upload raw data if any                                      
         Bytes rawData;
         if (descriptor.ExtractData(rawData))
            TODO();
      }
      else {
         // Configure a generator, based on provided primitives         
         FromDescriptor(descriptor);
         LANGULUS_ASSERT(mGenerators, Mesh, "No generators found in mesh");
      }
   }

   Couple(descriptor);
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

/// Analyze a descriptor, fill in any default properties that are missing     
/// by calling the appropriate GENERATOR::Default                             
///   @param desc - the descriptor to complete                                
bool Mesh::AutocompleteDescriptor(Construct& desc) {
   // The descriptor should have some primitive defined                 
   const auto primitive = desc.GetDescriptor().FindType<A::Primitive>();
   if (not primitive)
      return false;

   // The descriptor might or might not have the topology defined       
   const auto ttraits = desc.GetDescriptor().GetTraits<Traits::Topology>();
   DMeta topology;
   if (ttraits and *ttraits)
      topology = ttraits[0].As<DMeta>();

   return AutocompleteInner<GenerateBox,  Box2 >(desc, primitive, topology)
       or AutocompleteInner<GenerateBox,  Box3 >(desc, primitive, topology)
       or AutocompleteInner<GenerateGrid, Grid2>(desc, primitive, topology)
       or AutocompleteInner<GenerateGrid, Grid3>(desc, primitive, topology);
}

/// Create mesh generator by analyzing A::Primitive                           
///   @param data - container that contains the primitive                     
void Mesh::FromDescriptor(const Neat& desc) {
   mGenerators.Clear();

   const auto primitive = desc.FindType<A::Primitive>();
   LANGULUS_ASSERT(primitive, Mesh, "No primitive in descriptor: ", desc);

   if       (FillGenerators<GenerateBox,  Box2 >(primitive));
   else if  (FillGenerators<GenerateBox,  Box3 >(primitive));
   else if  (FillGenerators<GenerateGrid, Grid2>(primitive));
   else if  (FillGenerators<GenerateGrid, Grid3>(primitive));
   else LANGULUS_OOPS(Mesh,
      "Shouldn't be reached, make sure descriptor is a "
      "valid mesh descriptor prior to entering this function"
   );
}

/// Load mesh via filename/file interface                                     
///   @param descriptor - the file to load                                    
void Mesh::LoadFile(const Any& descriptor) {
   descriptor.ForEach(
      [&](const A::File& file) {
         ReadOBJ(file);
      },
      [&](const Text& path) {
			auto file = GetRuntime()->GetFile(path);
			if (file)
            ReadOBJ(*file);
      }
   );
}
