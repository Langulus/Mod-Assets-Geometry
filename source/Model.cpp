///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "Model.hpp"
#include "GeometryLibrary.hpp"


/// GUI system construction                                                   
///   @param producer - the system producer                                   
///   @param descriptor - instructions for configuring the GUI                
Model::Model(GUI* producer, const Descriptor& descriptor)
   : A::UI::System {MetaOf<GUISystem>(), descriptor}
   , ProducedFrom {producer, descriptor}
   , mItems {this}
   , mFonts {this} {

}

/// GUI system destruction                                                    
Model::~Model() {
   if (mContext)
      ImGui::DestroyContext(mContext);
}

/// Produce GUI elements and fonts                                            
///   @param verb - creation verb to satisfy                                  
void Model::Create(Verb& verb) {
   mItems.Create(verb);
   mFonts.Create(verb);
}

/// React on environmental change                                             
void Model::Refresh() {

}