///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright (c) 2016 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "MeshLibrary.hpp"

LANGULUS_DEFINE_MODULE(
   MeshLibrary, 9, "AssetsGeometry",
   "Mesh reader, writer and generator", "",
   MeshLibrary, Mesh, Traits::Tesselation
)


/// Module construction                                                       
///   @param runtime - the runtime that owns the module                       
///   @param desc - instructions for configuring the module                   
MeshLibrary::MeshLibrary(Runtime* runtime, const Neat& desc)
   : Resolvable {this}
   , Module {runtime}
   /*, mMeshes {this}*/ {
   VERBOSE_MESHES("Initializing...");

   // Extract mesh folder, if any                                       
   Path repo = "assets/meshes";
   if (not desc.ExtractTrait<Traits::Name, Traits::Path>(repo))
      desc.ExtractDataAs(repo);

   try { mFolder = GetRuntime()->GetFolder(repo); }
   catch (...) {
      Logger::Warning(Self(),
         "Can't access mesh asset library folder `", repo,
         "` - either folder is missing, or there's probably "
         "no file system module available. "
         "Mesh reading/writing won't be available, "
         "but you can still generate meshes"
      );
   }

   VERBOSE_MESHES("Initialized");
}

/// Create/destroy meshes                                                     
///   @param verb - the creation/destruction verb                             
void MeshLibrary::Create(Verb& verb) {
   Construct request;
   verb.ForEachDeep(
      [&](const Construct& construct) {
         // For each construct...                                       
         if (not construct.CastsTo<A::Mesh>())
            return;
         request = construct;
      },
      [&](const DMeta& type) {
         // For each type...                                            
         if (not type or not type->CastsTo<A::Mesh>())
            return;
         request = Construct {type};
      }
   );

   if (request.IsUntyped())
      return;

   // Mesh descriptor might be partial, so we attempt to autocomplete it
   // with default traits, provided by the desired generator. This      
   // ensures, that partial requests can match other partial requests,  
   // if they end up the same after the implicit traits are considered  
   // It's like an additional level of normalization over Neat          
   if (Mesh::AutocompleteDescriptor(request)) {
      VERBOSE_MESHES("Mesh autocompleted to: ", request);
      auto local = verb.Fork(&request);
      mMeshes.Create(this, local);
      verb << Abandon(local.GetOutput());
   }
   else {
      // Couldn't be normalized more, but we can still attempt to make  
      // the mesh, it probably contains a filename, or raw data         
      mMeshes.Create(this, verb);
   }
}