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
      Path path;     // Resolved path to texture                        
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
      Count position_count;
      TAny<float> positions;

      Count texcoord_count;
      TAny<float> texcoords;

      Count normal_count;
      TAny<float> normals;

      Count color_count;
      TAny<float> colors;

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

      // Base path for materials/textures                               
      Path base;
   };


   struct Callbacks {
      void* (*file_open)(const char* path, void* user_data);
      void  (*file_close)(void* file, void* user_data);
      size_t(*file_read)(void* file, void* dst, size_t bytes, void* user_data);
      unsigned long (*file_size)(void* file, void* user_data);
   };

private:
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

   
   static int is_whitespace(char c) {
      return (c == ' ' || c == '\t' || c == '\r');
   }

   static int is_newline(char c) {
      return (c == '\n');
   }

   static int is_digit(char c) {
      return (c >= '0' && c <= '9');
   }

   static int is_exponent(char c) {
      return (c == 'e' || c == 'E');
   }

   static const char* skip_name(const char* ptr) {
      const char* s = ptr;

      while (!is_newline(*ptr))
         ptr++;

      while (ptr > s && is_whitespace(*(ptr - 1)))
         ptr--;

      return ptr;
   }

   static const char* skip_whitespace(const char* ptr) {
      while (is_whitespace(*ptr))
         ptr++;

      return ptr;
   }

   static const char* skip_line(const char* ptr) {
      while (!is_newline(*ptr++))
         ;

      return ptr;
   }

   static void flush_object(Data* data) {
      /* Add object if not empty */
      if (data->object.face_count > 0)
         data->mesh->objects << data->object;
      else
         data->object = {};

      /* Reset for more data */
      data->object = {};
      data->object.face_offset = data->mesh->face_vertices.GetCount();
      data->object.index_offset = data->mesh->indices.GetCount();
   }

   static void flush_group(Data* data) {
      /* Add group if not empty */
      if (data->group.face_count > 0)
         data->mesh->groups << data->group;
      else
         data->group = {};

      /* Reset for more data */
      data->group = {};
      data->group.face_offset = data->mesh->face_vertices.GetCount();
      data->group.index_offset = data->mesh->indices.GetCount();
   }

   static const char* parse_int(const char* ptr, int* val) {
      int sign;
      int num;

      if (*ptr == '-') {
         sign = -1;
         ptr++;
      }
      else sign = +1;

      num = 0;
      while (is_digit(*ptr))
         num = 10 * num + (*ptr++ - '0');

      *val = sign * num;
      return ptr;
   }

   static const char* parse_float(const char* ptr, float* val) {
      double        sign;
      double        num;
      double        fra;
      double        div;
      unsigned int  eval;
      const double* powers;


      ptr = skip_whitespace(ptr);

      switch (*ptr) {
      case '+':
         sign = 1.0;
         ptr++;
         break;

      case '-':
         sign = -1.0;
         ptr++;
         break;

      default:
         sign = 1.0;
         break;
      }


      num = 0.0;
      while (is_digit(*ptr))
         num = 10.0 * num + (double)(*ptr++ - '0');

      if (*ptr == '.')
         ptr++;

      fra = 0.0;
      div = 1.0;

      while (is_digit(*ptr)) {
         fra = 10.0 * fra + (double)(*ptr++ - '0');
         div *= 10.0;
      }

      num += fra / div;

      if (is_exponent(*ptr)) {
         ptr++;

         switch (*ptr) {
         case '+':
            powers = POWER_10_POS;
            ptr++;
            break;

         case '-':
            powers = POWER_10_NEG;
            ptr++;
            break;

         default:
            powers = POWER_10_POS;
            break;
         }

         eval = 0;
         while (is_digit(*ptr))
            eval = 10 * eval + (*ptr++ - '0');

         num *= (eval >= MAX_POWER) ? 0.0 : powers[eval];
      }

      *val = (float)(sign * num);
      return ptr;
   }


   static const char* parse_vertex(Data* data, const char* ptr) {
      unsigned int ii;
      float        v;

      for (ii = 0; ii < 3; ii++) {
         ptr = parse_float(ptr, &v);
         data->mesh->positions << v;
      }

      ptr = skip_whitespace(ptr);
      if (!is_newline(*ptr))
      {
         /* Fill the colors array until it matches the size of the positions array */
         for (ii = data->mesh->colors.GetCount(); ii < data->mesh->positions.GetCount() - 3; ++ii)
         {
            data->mesh->colors << 1.0f;
         }

         for (ii = 0; ii < 3; ++ii)
         {
            ptr = parse_float(ptr, &v);
            data->mesh->colors << v;
         }
      }

      return ptr;
   }


   static const char* parse_texcoord(Data* data, const char* ptr) {
      unsigned int ii;
      float        v;

      for (ii = 0; ii < 2; ii++)
      {
         ptr = parse_float(ptr, &v);
         data->mesh->texcoords << v;
      }

      return ptr;
   }


   static const char* parse_normal(Data* data, const char* ptr) {
      unsigned int ii;
      float        v;


      for (ii = 0; ii < 3; ii++)
      {
         ptr = parse_float(ptr, &v);
         data->mesh->normals << v;
      }

      return ptr;
   }


   static const char* parse_face(Data* data, const char* ptr) {
      unsigned int count;
      Index vn;
      int   v;
      int   t;
      int   n;


      ptr = skip_whitespace(ptr);

      count = 0;
      while (!is_newline(*ptr))
      {
         v = 0;
         t = 0;
         n = 0;

         ptr = parse_int(ptr, &v);
         if (*ptr == '/')
         {
            ptr++;
            if (*ptr != '/')
               ptr = parse_int(ptr, &t);

            if (*ptr == '/')
            {
               ptr++;
               ptr = parse_int(ptr, &n);
            }
         }

         if (v < 0)
            vn.p = (data->mesh->positions.GetCount() / 3) - (Idx)(-v);
         else if (v > 0)
            vn.p = (Idx)(v);
         else
            return ptr; /* Skip lines with no valid vertex index */

         if (t < 0)
            vn.t = (data->mesh->texcoords.GetCount() / 2) - (Idx)(-t);
         else if (t > 0)
            vn.t = (Idx)(t);
         else
            vn.t = 0;

         if (n < 0)
            vn.n = (data->mesh->normals.GetCount() / 3) - (Idx)(-n);
         else if (n > 0)
            vn.n = (Idx)(n);
         else
            vn.n = 0;

         data->mesh->indices << vn;
         count++;

         ptr = skip_whitespace(ptr);
      }

      data->mesh->face_vertices << count;
      data->mesh->face_materials << data->material;
      data->group.face_count++;
      data->object.face_count++;
      return ptr;
   }

   static const char* parse_object(Data* data, const char* ptr) {
      auto s = (ptr = skip_whitespace(ptr));
      auto e = (ptr = skip_name(ptr));
      flush_object(data);
      data->object.name = Token(s, e);
      return ptr;
   }

   static const char* parse_group(Data* data, const char* ptr) {
      const char* s;
      const char* e;

      ptr = skip_whitespace(ptr);

      s = ptr;
      ptr = skip_name(ptr);
      e = ptr;

      flush_group(data);
      data->group.name = Token(s, e);
      return ptr;
   }

   static const char* parse_usemtl(Data* data, const char* ptr) {
      const char* s;
      const char* e;
      unsigned int idx;
      Material* mtl;

      ptr = skip_whitespace(ptr);

      // Parse the material name                                        
      s = ptr;
      ptr = skip_name(ptr);
      e = ptr;

      // Find an existing material with the same name                   
      idx = 0;
      while (idx < data->mesh->materials.GetCount()) {
         mtl = &data->mesh->materials[idx];
         if (mtl->name == Token(s, e))
            break;

         idx++;
      }

      // If doesn't exists, create a default one with this name         
      // Note: this case happens when OBJ doesn't have its MTL          
      if (idx == data->mesh->materials.GetCount()) {
         Material new_mtl;
         new_mtl.name = Token(s, e);
         new_mtl.fallback = 1;
         data->mesh->materials << new_mtl;
      }

      data->material = idx;
      return ptr;
   }

   static const char* read_mtl_int(const char* p, int* v) {
      return parse_int(p, v);
   }

   static const char* read_mtl_single(const char* p, float* v) {
      return parse_float(p, v);
   }

   static const char* read_mtl_triple(const char* p, float v[3]) {
      p = read_mtl_single(p, &v[0]);
      p = read_mtl_single(p, &v[1]);
      p = read_mtl_single(p, &v[2]);
      return p;
   }

   static const char* read_map(Data* data, const char* ptr, Texture* map) {
      const char* s;
      const char* e;
      char* path;

      ptr = skip_whitespace(ptr);

      /* Don't support options at present */
      if (*ptr == '-')
         return ptr;


      /* Read name */
      s = ptr;
      ptr = skip_name(ptr);
      e = ptr;

      map->name = Token(s, e);
      map->path = {data->base, map->name};
      return e;
   }

   static int read_mtllib(Data* data, void* file, const Callbacks* callbacks, void* user_data) {
      const char* s;

      // Read entire file                                               
      auto n = callbacks->file_size(file, user_data);
      Text contents;
      contents.Reserve(n + 1);

      auto l = callbacks->file_read(file, contents.GetRaw(), n, user_data);
      contents[l] = '\n';

      Material mtl;

      int found_d = 0;
      const char* p = contents.GetRaw();
      const char* e = p + l;
      while (p < e) {
         p = skip_whitespace(p);

         switch (*p) {
         case 'n':
            p++;

            if (p[0] == 'e'
            and p[1] == 'w'
            and p[2] == 'm'
            and p[3] == 't'
            and p[4] == 'l'
            and is_whitespace(p[5])) {
               // Push previous material (if there is one)              
               if (mtl.name) {
                  data->mesh->materials << mtl;
                  mtl = {};
               }

               // Read name                                             
               p += 5;
               while (is_whitespace(*p))
                  p++;

               s = p;
               p = skip_name(p);

               mtl.name = Token(s, p);
            }
            break;

         case 'K':
            if (p[1] == 'a')
               p = read_mtl_triple(p + 2, mtl.Ka);
            else if (p[1] == 'd')
               p = read_mtl_triple(p + 2, mtl.Kd);
            else if (p[1] == 's')
               p = read_mtl_triple(p + 2, mtl.Ks);
            else if (p[1] == 'e')
               p = read_mtl_triple(p + 2, mtl.Ke);
            else if (p[1] == 't')
               p = read_mtl_triple(p + 2, mtl.Kt);
            break;

         case 'N':
            if (p[1] == 's')
               p = read_mtl_single(p + 2, &mtl.Ns);
            else if (p[1] == 'i')
               p = read_mtl_single(p + 2, &mtl.Ni);
            break;

         case 'T':
            if (p[1] == 'r') {
               float Tr;
               p = read_mtl_single(p + 2, &Tr);
               if (!found_d) {
                  // Ignore Tr if we've already read d                  
                  mtl.d = 1.0f - Tr;
               }
            }
            else if (p[1] == 'f')
               p = read_mtl_triple(p + 2, mtl.Tf);
            break;

         case 'd':
            if (is_whitespace(p[1])) {
               p = read_mtl_single(p + 1, &mtl.d);
               found_d = 1;
            }
            break;

         case 'i':
            p++;

            if (p[0] == 'l'
            and p[1] == 'l'
            and p[2] == 'u'
            and p[3] == 'm'
            and is_whitespace(p[4])) {
               p = read_mtl_int(p + 4, &mtl.illum);
            }
            break;

         case 'm':
            p++;

            if (p[0] == 'a'
            and p[1] == 'p'
            and p[2] == '_') {
               p += 3;

               if (*p == 'K') {
                  p++;

                  if (is_whitespace(p[1])) {
                     if (*p == 'a')
                        p = read_map(data, p + 1, &mtl.map_Ka);
                     else if (*p == 'd')
                        p = read_map(data, p + 1, &mtl.map_Kd);
                     else if (*p == 's')
                        p = read_map(data, p + 1, &mtl.map_Ks);
                     else if (*p == 'e')
                        p = read_map(data, p + 1, &mtl.map_Ke);
                     else if (*p == 't')
                        p = read_map(data, p + 1, &mtl.map_Kt);
                  }
               }
               else if (*p == 'N') {
                  p++;

                  if (is_whitespace(p[1])) {
                     if (*p == 's')
                        p = read_map(data, p + 1, &mtl.map_Ns);
                     else if (*p == 'i')
                        p = read_map(data, p + 1, &mtl.map_Ni);
                  }
               }
               else if (*p == 'd') {
                  p++;

                  if (is_whitespace(*p))
                     p = read_map(data, p, &mtl.map_d);
               }
               else if ((p[0] == 'b' or p[0] == 'B')
               and p[1] == 'u'
               and p[2] == 'm'
               and p[3] == 'p'
               and is_whitespace(p[4])) {
                  p = read_map(data, p + 4, &mtl.map_bump);
               }
            }
            break;

         case '#':
            break;
         }

         p = skip_line(p);
      }

      // Push final material                                            
      if (mtl.name)
         data->mesh->materials << mtl;
      return 1;
   }

   static const char* parse_mtllib(Data* data, const char* ptr, const Callbacks* callbacks, void* user_data) {
      const char* s;
      const char* e;
      char* lib;
      void* file;

      ptr = skip_whitespace(ptr);

      s = ptr;
      ptr = skip_name(ptr);
      e = ptr;

      lib = string_concat(data->base, s, e);

      if (lib) {
         string_fix_separators(lib);

         file = callbacks->file_open(lib, user_data);
         if (file)
         {
            read_mtllib(data, file, callbacks, user_data);
            callbacks->file_close(file, user_data);
         }

         memory_dealloc(lib);
      }

      return ptr;
   }

   static void parse_buffer(Data* data, const char* ptr, const char* end, const Callbacks* callbacks, void* user_data) {
      const char* p;
      p = ptr;

      while (p != end) {
         p = skip_whitespace(p);

         switch (*p) {
         case 'v':
            p++;

            switch (*p++) {
            case ' ':
            case '\t':
               p = parse_vertex(data, p);
               break;

            case 't':
               p = parse_texcoord(data, p);
               break;

            case 'n':
               p = parse_normal(data, p);
               break;

            default:
               p--; /* roll p++ back in case *p was a newline */
            }
            break;

         case 'f':
            p++;

            switch (*p++) {
            case ' ':
            case '\t':
               p = parse_face(data, p);
               break;

            default:
               p--; /* roll p++ back in case *p was a newline */
            }
            break;

         case 'o':
            p++;

            switch (*p++) {
            case ' ':
            case '\t':
               p = parse_object(data, p);
               break;

            default:
               p--; /* roll p++ back in case *p was a newline */
            }
            break;

         case 'g':
            p++;

            switch (*p++)
            {
            case ' ':
            case '\t':
               p = parse_group(data, p);
               break;

            default:
               p--; /* roll p++ back in case *p was a newline */
            }
            break;

         case 'm':
            p++;

            if (p[0] == 't' &&
               p[1] == 'l' &&
               p[2] == 'l' &&
               p[3] == 'i' &&
               p[4] == 'b' &&
               is_whitespace(p[5]))
               p = parse_mtllib(data, p + 5, callbacks, user_data);
            break;

         case 'u':
            p++;

            if (p[0] == 's' &&
               p[1] == 'e' &&
               p[2] == 'm' &&
               p[3] == 't' &&
               p[4] == 'l' &&
               is_whitespace(p[5]))
               p = parse_usemtl(data, p + 5);
            break;

         case '#':
            break;
         }

         p = skip_line(p);

         data->line++;
      }
      if (data->mesh->colors) {
         // Fill the remaining slots in the colors array                
         unsigned int ii;
         for (ii = data->mesh->colors.GetCount(); ii < data->mesh->positions.GetCount(); ++ii)
         {
            data->mesh->colors << 1.0f;
         }
      }
   }

   Mesh fast_obj_read_with_callbacks(const Path& path, const Callbacks* callbacks, void* user_data) {
      char* last;
      Idx  read;
      Idx  bytes;

      // Check if callbacks are valid                                   
      if (not callbacks)
         return {};

      // Open file                                                      
      auto file = callbacks->file_open(path.Terminate().GetRaw(), user_data);
      if (not file)
         return {};

      // Empty mesh                                                     
      Mesh m;

      // Add dummy position/texcoord/normal                             
      m.positions << 0.0f;
      m.positions << 0.0f;
      m.positions << 0.0f;

      m.texcoords << 0.0f;
      m.texcoords << 0.0f;

      m.normals << 0.0f;
      m.normals << 0.0f;
      m.normals << 1.0f;

      // Data needed during parsing                                     
      Data data;
      data.mesh = &m;
      data.material = 0;
      data.line = 1;
      data.base = path.GetDirectory();

      // Create buffer for reading file                                 
      Text buffer;
      buffer.Reserve(2 * BufferSize);
      auto start = buffer.GetRaw();

      for (;;) {
         // Read another buffer's worth from file                       
         read = (Idx)(callbacks->file_read(file, start, BufferSize, user_data));
         if (read == 0 and start == buffer)
            break;

         // Ensure buffer ends in a newline                             
         if (read < BufferSize) {
            if (read == 0 or start[read - 1] != '\n')
               start[read++] = '\n';
         }

         auto end = start + read;
         if (end == buffer)
            break;

         // Find last new line                                          
         last = end;
         while (last > buffer.GetRaw()) {
            last--;
            if (*last == '\n')
               break;
         }

         // Check there actually is a new line                          
         if (*last != '\n')
            break;

         last++;

         // Process buffer                                              
         parse_buffer(&data, buffer, last, callbacks, user_data);

         // Copy overflow for next buffer                               
         bytes = (Idx)(end - last);
         memmove(buffer, last, bytes);
         start = buffer + bytes;
      }

      // Flush final object/group                                       
      m.position_count = m.positions.GetCount() / 3;
      m.texcoord_count = m.texcoords.GetCount() / 2;
      m.normal_count = m.normals.GetCount() / 3;
      m.color_count = m.colors.GetCount() / 3;
      m.face_count = m.face_vertices.GetCount();
      m.index_count = m.indices.GetCount();
      m.material_count = m.materials.GetCount();
      m.object_count = m.objects.GetCount();
      m.group_count = m.groups.GetCount();

      // Clean up                                                       
      callbacks->file_close(file, user_data);
      return m;
   }
};