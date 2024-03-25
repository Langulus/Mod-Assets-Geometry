///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright (c) 2016 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Common.hpp"


namespace Tools
{

   /// Subdivide faces                                                        
   ///   @param div - subdivisions                                            
   ///   @param indices - indices (can be null)                               
   ///   @param source - original vertices                                    
   ///   @param dest - destination container where new vertices go            
   ///   @param pcount - number of primitives                                 
   ///   @param vstart - starting vertex                                      
   ///   @param vstep - vertex step                                           
   ///   @param vperprim - vertices per primitive                             
   template<class DATA, class INDEX>
   void Subdivide(pcptr div, const Any* indices, const Any* source, Any* output, const pcptr pcount, const pcptr vstart, const pcptr vstep, const pcptr vperprim) {
      // Can't subdivide points                                          
      if (vperprim == 1) {
         *output = *source;
         return;
      }

      // Calculate new vcount                                             
      div = div ? div : 1;
      const auto step = DATA(div - 1);
      const auto newbase = vperprim == 3 ? pcSum(div) : div * div;

      // Allocate new data                                                
      // From this point onward, the data is not indexed                  
      TAny<DATA> out_view;
      out_view.Reserve(newbase * pcount);
      const auto* oldarray = source->As<const DATA*>();
      const INDEX* idx = indices ? indices->As<const INDEX*>() : nullptr;

      // Subdivide triangles                                             
      if (vperprim == 3) {
         for (auto i = vstart; i < (indices ? indices->GetCount() : source->GetCount()); i += vstep) {
            const auto& a = idx ? oldarray[idx[i]] : oldarray[i];
            const auto& b = idx ? oldarray[idx[i + 1]] : oldarray[i + 1];
            const auto& c = idx ? oldarray[idx[i + 2]] : oldarray[i + 2];

            const auto AB = (b - a) / step;
            const auto CB = (c - b) / step;

            // Begins at A and goes to BC edge for each row               
            for (pcptr r = 0; r < div; ++r)
               for (pcptr v = 0; v <= r; v++)
                  out_view << a + AB * DATA(r) + CB * DATA(v);
         }
      }
      // Subdivide lines                                                
      else if (vperprim == 2) {
         for (auto i = vstart; i < (indices ? indices->GetCount() : source->GetCount()); i += vstep) {
            const auto& a = idx ? oldarray[idx[i]] : oldarray[i];
            const auto& b = idx ? oldarray[idx[i + 1]] : oldarray[i + 1];

            const auto AB = (b - a) / step;

            // Begins at A and goes to B edge for each row               
            for (pcptr r = 0; r < div; ++r)
               out_view << a + AB * DATA(r);
         }
      }

      *output = out_view;
   }

   /// Normalize                                                               
   ///   @param output - destination container where normalized values go      
   template<class T>
   void Normalize(Any* output) {
      TODO();
   }

} // namespace Tools