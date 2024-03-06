///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Common.hpp"


///                                                                           
///   An *.OBJ file representation and interface                              
///                                                                           
/// Based on fast_obj v1.2 https://github.com/thisistherk/fast_obj            
/// Completely rewritten to use Langulus containers and C++20                 
///                                                                           
struct Obj {

   using Idx = unsigned;

   /// Parsed texture descriptor                                              
   struct Texture {
      Text name;     // Texture name from .mtl file                     
   };

   /// Parsed material descriptor                                             
   struct Material {
      Text name;                 // Material name                       

      float Ka[3] = {0, 0, 0};   // Ambient                             
      float Kd[3] = {1, 1, 1};   // Diffuse                             
      float Ks[3] = {0, 0, 0};   // Specular                            
      float Ke[3] = {0, 0, 0};   // Emission                            
      float Kt[3] = {0, 0, 0};   // Transmittance                       
      float Ns = 1;              // Shininess                           
      float Ni = 1;              // Index of refraction                 
      float Tf[3] = {1, 1, 1};   // Transmission filter                 
      float d = 1;               // Disolve (alpha)                     
      int   illum = 1;           // Illumination model                  

      // Set for materials that don't come from the associated mtllib   
      int   fallback = 0;

      // Texture maps                                                   
      Texture map_Ka;
      Texture map_Kd;
      Texture map_Ks;
      Texture map_Ke;
      Texture map_Kt;
      Texture map_Ns;
      Texture map_Ni;
      Texture map_d;
      Texture map_bump;
   };

   /// Parsed object index                                                    
   struct Index {
      Idx p;
      Idx t;
      Idx n;
   };

   /// Parsed object group                                                    
   struct Group {
      Text name;                       // Group name                    
      unsigned int face_count = 0;     // Number of faces               
      unsigned int face_offset = 0;    // First face in fastObjMesh face_* arrays
      unsigned int index_offset = 0;   // First index in fastObjMesh indices array
   };

   /// Parsed object mesh                                                     
   struct Mesh {
      // Vertex data                                                    
      TAny<Point3f> positions;
      TAny<Point2f> texcoords;
      TAny<Point3f> normals;
      TAny<Point3f> colors;

      // Face data: one element for each face                           
      Count          face_count;
      TAny<unsigned> face_vertices;
      TAny<unsigned> face_materials;

      // Index data: one element for each face vertex                   
      Count          index_count;
      TAny<Index>    indices;

      // Materials                                                      
      Count          material_count;
      TAny<Material> materials;

      // Mesh objects ('o' tag in .obj file)                            
      Count          object_count;
      TAny<Group>    objects;

      // Mesh groups ('g' tag in .obj file)                             
      Count          group_count;
      TAny<Group>    groups;
   };


   struct Data {
      // Final mesh                                                     
      Mesh* mesh;

      // Current object/group                                           
      Group object;
      Group group;

      // Current material index                                         
      unsigned int material;

      // Current line in file                                           
      unsigned int line;
   };

   // Size of buffer to read into                                       
   static constexpr size_t BufferSize = 65536;

   // Max supported power when parsing float                            
   static constexpr size_t MAX_POWER = 20;
   
   static constexpr double POWER_10_POS[MAX_POWER] = {
       1.0e0,  1.0e1,  1.0e2,  1.0e3,  1.0e4,  1.0e5,  1.0e6,  1.0e7,  1.0e8,  1.0e9,
       1.0e10, 1.0e11, 1.0e12, 1.0e13, 1.0e14, 1.0e15, 1.0e16, 1.0e17, 1.0e18, 1.0e19,
   };

   static constexpr double POWER_10_NEG[MAX_POWER] = {
       1.0e0,   1.0e-1,  1.0e-2,  1.0e-3,  1.0e-4,  1.0e-5,  1.0e-6,  1.0e-7,  1.0e-8,  1.0e-9,
       1.0e-10, 1.0e-11, 1.0e-12, 1.0e-13, 1.0e-14, 1.0e-15, 1.0e-16, 1.0e-17, 1.0e-18, 1.0e-19,
   };

   static int is_whitespace(char c);
   static int is_newline(char c);
   static int is_digit(char c);
   static int is_exponent(char c);
   static const char* skip_name(const char* ptr);
   static const char* skip_whitespace(const char* ptr);
   static const char* skip_line(const char* ptr);
   static void flush_object(Data* data);
   static void flush_group(Data* data);
   static const char* parse_int(const char* ptr, int* val);
   static const char* parse_float(const char* ptr, float* val);
   static const char* parse_vertex(Data* data, const char* ptr);
   static const char* parse_texcoord(Data* data, const char* ptr);
   static const char* parse_normal(Data* data, const char* ptr);
   static const char* parse_face(Data* data, const char* ptr);
   static const char* parse_object(Data* data, const char* ptr);
   static const char* parse_group(Data* data, const char* ptr);
   static const char* parse_usemtl(Data* data, const char* ptr);
   static const char* read_mtl_int(const char* p, int* v);
   static const char* read_mtl_single(const char* p, float* v);
   static const char* read_mtl_triple(const char* p, float v[3]);
   static const char* read_map(Data* data, const char* ptr, Texture* map);
   static int read_mtllib(Data* data, const A::File& file);
   static void parse_buffer(Data* data, const char* ptr, const char* end, Ref<A::File::Reader>& stream);
};