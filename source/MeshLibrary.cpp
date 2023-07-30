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
MeshLibrary::MeshLibrary(Runtime* runtime, const Descriptor&)
   : A::AssetModule {MetaOf<MeshLibrary>(), runtime}
   , mMeshes {this} {
   Logger::Verbose(Self(), "Initializing...");
   Logger::Verbose(Self(), "Initialized");
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