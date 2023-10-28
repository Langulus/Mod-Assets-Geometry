///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
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
///   @param descriptor - instructions for configuring the module             
MeshLibrary::MeshLibrary(Runtime* runtime, const Neat&)
   : A::AssetModule {MetaOf<MeshLibrary>(), runtime}
   , mMeshes {this} {
   VERBOSE_MESHES("Initializing...");
   // Extract mesh folder, if any                                       
   //TODO configure mFolder from descriptor

   try {
      mMeshFolder = Path {"assets"} / "meshes";
      mFolder = GetRuntime()->GetFolder(mMeshFolder);
   }
   catch (...) {
      Logger::Warning(Self(),
         "Can't access mesh asset library folder `", mMeshFolder,
         "` - either folder is missing, or there's probably "
         "no file system module available. "
         "Mesh reading/writing won't be available, "
         "but you can still generate meshes"
      );
   }

   VERBOSE_MESHES("Initialized");
}

/// Module update routine                                                     
///   @param dt - time from last update                                       
void MeshLibrary::Update(Time) {

}

/// Create/Destroy meshes                                                     
///   @param verb - the creation/destruction verb                             
void MeshLibrary::Create(Verb& verb) {
   mMeshes.Create(verb);
}

/// Get the mesh library folder                                               
///   @return the folder interface                                            
const A::Folder* MeshLibrary::GetFolder() const noexcept {
   return *mFolder;
}
