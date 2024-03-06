///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "Mesh.hpp"
#include "OBJ.hpp"


/// Load OBJ file                                                             
///   @param file - [in/out] the file to load from                            
///   @return true if model was loaded without any problems                   
bool Mesh::ReadOBJ(const A::File& file) {
   auto loadTime = SteadyClock::Now();
   auto stream = file.NewReader();
   if (not stream) {
      Logger::Error("Cannot open file: ", file);
      return false;
   }

   // Empty mesh                                                     
   Obj::Mesh m;

   // Add dummy position/texcoord/normal                             
   m.positions << 0;
   m.texcoords << 0;
   m.normals   << Point3f {0, 0, 1};

   // Data needed during parsing                                     
   Obj::Data data;
   data.mesh = &m;
   data.material = 0;
   data.line = 1;

   // Create buffer for reading file                                 
   Text buffer;
   buffer.Reserve(2 * Obj::BufferSize);
   auto start = buffer.GetRaw();

   for (;;) {
      // Read another buffer's worth from file                       
      auto read = stream->Read(buffer);
      if (read == 0 and start == buffer.GetRaw())
         break;

      // Ensure buffer ends in a newline                             
      if (read < Obj::BufferSize) {
         if (read == 0 or start[read - 1] != '\n')
            start[read++] = '\n';
      }

      const auto end = start + read;
      if (end == buffer)
         break;

      // Find last new line                                          
      auto last = end;
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
      Obj::parse_buffer(&data, buffer.GetRaw(), last, stream);

      // Copy overflow for next buffer                               
      auto bytes = end - last;
      memmove(buffer.GetRaw(), last, bytes);
      start = buffer.GetRaw() + bytes;
   }

   // Flush final object/group                                          
   m.face_count = m.face_vertices.GetCount();
   m.index_count = m.indices.GetCount();
   m.material_count = m.materials.GetCount();
   m.object_count = m.objects.GetCount();
   m.group_count = m.groups.GetCount();

   // Save the contents                                                 
   Commit<Traits::Place>   (Move(m.positions));
   Commit<Traits::Aim>     (Move(m.normals));
   Commit<Traits::Sampler> (Move(m.texcoords));
   Commit<Traits::Color>   (Move(m.colors));

   Logger::Verbose(Logger::Green, "File ", file.GetFilePath(), 
      " loaded in ", SteadyClock::Now() - loadTime);
   return true;
}

/// Parse a chunk of obj file memory                                          
///   @param data                                                             
///   @param ptr                                                              
///   @param end                                                              
///   @param stream                                                           
void Obj::parse_buffer(
   Data* data, const char* ptr, const char* end, Ref<A::File::Reader>& stream
) {
   const char* p;
   p = ptr;

   while (p != end) {
      p = skip_whitespace(p);

      switch (*p) {
      case 'v':
         // Parse a vertex                                              
         p++;

         switch (*p++) {
         case ' ': case '\t':
            p = parse_vertex(data, p);
            break;
         case 't':
            p = parse_texcoord(data, p);
            break;
         case 'n':
            p = parse_normal(data, p);
            break;
         default:
            p--; // Roll p++ back in case *p was a newline              
         }
         break;

      case 'f':
         // Parse a face                                                
         p++;

         switch (*p++) {
         case ' ': case '\t':
            p = parse_face(data, p);
            break;
         default:
            p--; // Roll p++ back in case *p was a newline              
         }
         break;

      case 'o':
         // Parse an object                                             
         p++;

         switch (*p++) {
         case ' ': case '\t':
            p = parse_object(data, p);
            break;
         default:
            p--; // Roll p++ back in case *p was a newline              
         }
         break;

      case 'g':
         // Parse a group                                               
         p++;

         switch (*p++) {
         case ' ': case '\t':
            p = parse_group(data, p);
            break;
         default:
            p--; // Roll p++ back in case *p was a newline              
         }
         break;

      case 'm':
         // Parse a material library                                    
         p++;

         if (Token {p, p + 5} == "tllib" and is_whitespace(p[5])) {
            ptr = skip_whitespace(ptr + 5);
            auto e = (ptr = skip_name(ptr));
            const Path lib = Token {ptr, e};
            if (lib) {
               auto file = stream->GetFile()->RelativeFile(lib);
               if (file)
                  read_mtllib(data, *file);
            }
         }
         break;

      case 'u':
         // Parse a built-in material                                   
         p++;

         if (Token {p, p + 5} == "semtl" and is_whitespace(p[5]))
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
      for (auto ii = data->mesh->colors.GetCount();
                ii < data->mesh->positions.GetCount();
              ++ii)
         data->mesh->colors << 1;
   }
}

/// @brief 
/// @param data 
/// @param file 
/// @param stream 
/// @return 
int Obj::read_mtllib(Data* data, const A::File& file) {
   const char* s;

   // Read entire file                                                  
   auto contents = file.ReadAs<Text>();
   contents << '\n';

   Material mtl;
   int found_d = 0;
   const char* p = contents.GetRaw();
   const char* e = contents.GetRawEnd();

   while (p < e) {
      p = skip_whitespace(p);

      switch (*p) {
      case 'n':
         p++;

         if (Token {p, p + 5} == "ewmtl" and is_whitespace(p[5])) {
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

         if (Token {p, p + 4} == "llum" and is_whitespace(p[4]))
            p = read_mtl_int(p + 4, &mtl.illum);
         break;

      case 'm':
         p++;

         if (Token {p, p + 3} == "ap_") {
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
            else if (Token {p, p + 4} == "bump"
            or Token {p, p + 4} == "Bump"
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

/// Parse three floats for a vertex position, and optionally three floats     
/// for RGB color                                                             
///   @param data - the mesh to save data in                                  
///   @param ptr - text to parse                                              
///   @return pointer to the next statement for parsing                       
const char* Obj::parse_vertex(Data* data, const char* ptr) {
   Point3f v;
   ptr = parse_float(ptr, &v[0]);
   ptr = parse_float(ptr, &v[1]);
   ptr = parse_float(ptr, &v[2]);
   data->mesh->positions << v;
   ptr = skip_whitespace(ptr);

   if (not is_newline(*ptr)) {
      // Fill the colors array until it matches the size of the         
      // positions array. I guess it is possible to have vertices       
      // without color?                                                 
      for (auto ii = data->mesh->colors.GetCount();
                ii < data->mesh->positions.GetCount() - 1;
              ++ii)
         data->mesh->colors << 1;

      ptr = parse_float(ptr, &v[0]);
      ptr = parse_float(ptr, &v[1]);
      ptr = parse_float(ptr, &v[2]);
      data->mesh->colors << v;
   }

   return ptr;
}

/// Parse two floats for texture coordinates                                  
///   @param data - the mesh to save data in                                  
///   @param ptr - text to parse                                              
///   @return pointer to the next statement for parsing                       
const char* Obj::parse_texcoord(Data* data, const char* ptr) {
   Point2f v;
   ptr = parse_float(ptr, &v[0]);
   ptr = parse_float(ptr, &v[1]);
   data->mesh->texcoords << v;
   return ptr;
}

/// Parse three floats for normal coordinates                                 
///   @param data - the mesh to save data in                                  
///   @param ptr - text to parse                                              
///   @return pointer to the next statement for parsing                       
const char* Obj::parse_normal(Data* data, const char* ptr) {
   Point3f v;
   ptr = parse_float(ptr, &v[0]);
   ptr = parse_float(ptr, &v[1]);
   ptr = parse_float(ptr, &v[2]);
   data->mesh->normals << v;
   return ptr;
}

/// @brief 
/// @param data 
/// @param ptr 
/// @return 
const char* Obj::parse_face(Data* data, const char* ptr) {
   ptr = skip_whitespace(ptr);
   unsigned count = 0;

   while (not is_newline(*ptr)) {
      int v = 0;
      int t = 0;
      int n = 0;

      ptr = parse_int(ptr, &v);
      if (*ptr == '/') {
         ptr++;

         if (*ptr != '/')
            ptr = parse_int(ptr, &t);

         if (*ptr == '/') {
            ptr++;
            ptr = parse_int(ptr, &n);
         }
      }

      Index vn;
      if (v < 0)
         vn.p = data->mesh->positions.GetCount() - static_cast<Idx>(-v);
      else if (v > 0)
         vn.p = static_cast<Idx>(v);
      else
         return ptr; // Skip lines with no valid vertex index           

      if (t < 0)
         vn.t = data->mesh->texcoords.GetCount() - static_cast<Idx>(-t);
      else if (t > 0)
         vn.t = static_cast<Idx>(t);
      else
         vn.t = 0;

      if (n < 0)
         vn.n = data->mesh->normals.GetCount() - static_cast<Idx>(-n);
      else if (n > 0)
         vn.n = static_cast<Idx>(n);
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

/// @brief 
/// @param data 
/// @param ptr 
/// @return 
const char* Obj::parse_object(Data* data, const char* ptr) {
   auto s = (ptr = skip_whitespace(ptr));
   auto e = (ptr = skip_name(ptr));
   flush_object(data);
   data->object.name = Token(s, e);
   return ptr;
}

/// @brief 
/// @param data 
/// @param ptr 
/// @return 
const char* Obj::parse_group(Data* data, const char* ptr) {
   auto s = (ptr = skip_whitespace(ptr));
   auto e = (ptr = skip_name(ptr));
   flush_group(data);
   data->group.name = Token(s, e);
   return ptr;
}

/// @brief 
/// @param data 
/// @param ptr 
/// @return 
const char* Obj::parse_usemtl(Data* data, const char* ptr) {
   auto s = (ptr = skip_whitespace(ptr));
   auto e = (ptr = skip_name(ptr));

   // Find an existing material with the same name                   
   Material* mtl;
   unsigned idx = 0;
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

/// @brief 
/// @param p 
/// @param v 
/// @return 
const char* Obj::read_mtl_int(const char* p, int* v) {
   return parse_int(p, v);
}

/// @brief 
/// @param p 
/// @param v 
/// @return 
const char* Obj::read_mtl_single(const char* p, float* v) {
   return parse_float(p, v);
}

/// @brief 
/// @param p 
/// @param v 
/// @return 
const char* Obj::read_mtl_triple(const char* p, float v[3]) {
   p = read_mtl_single(p, v + 0);
   p = read_mtl_single(p, v + 1);
   p = read_mtl_single(p, v + 2);
   return p;
}

/// @brief 
/// @param data 
/// @param ptr 
/// @param map 
/// @return 
const char* Obj::read_map(Data* data, const char* ptr, Texture* map) {
   ptr = skip_whitespace(ptr);

   // Don't support options at present                               
   if (*ptr == '-')
      return ptr;

   // Read name                                                      
   auto e = (ptr = skip_name(ptr));
   map->name = Token(ptr, e);
   return ptr;
}

/// @brief 
/// @param ptr 
/// @param val 
/// @return 
const char* Obj::parse_int(const char* ptr, int* val) {
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

/// @brief 
/// @param ptr 
/// @param val 
/// @return 
const char* Obj::parse_float(const char* ptr, float* val) {
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

/// @brief 
/// @param c 
/// @return 
int Obj::is_whitespace(char c) {
   return c == ' ' or c == '\t' or c == '\r';
}

/// @brief 
/// @param c 
/// @return 
int Obj::is_newline(char c) {
   return c == '\n';
}

/// @brief 
/// @param c 
/// @return 
int Obj::is_digit(char c) {
   return c >= '0' and c <= '9';
}

/// @brief 
/// @param c 
/// @return 
int Obj::is_exponent(char c) {
   return c == 'e' or c == 'E';
}

/// Skip a name by going to the end of the line, and reverting back to first  
/// symbol that isn't whitespace                                              
///   @param ptr - data to scan                                               
///   @return a pointer to the end of the name                                
const char* Obj::skip_name(const char* ptr) {
   auto s = ptr;
   while (not is_newline(*ptr))
      ptr++;
   while (ptr > s and is_whitespace(*(ptr - 1)))
      ptr--;
   return ptr;
}

/// Skip all whitespace forward                                               
///   @param ptr - data to scan                                               
///   @return a pointer to beginning of next non-whitespace symbol            
const char* Obj::skip_whitespace(const char* ptr) {
   while (is_whitespace(*ptr))
      ++ptr;
   return ptr;
}

/// Skip forward until a new line begins                                      
///   @param ptr - data to scan                                               
///   @return a pointer to beginning of next line                             
const char* Obj::skip_line(const char* ptr) {
   while (not is_newline(*ptr++))
      ;
   return ptr;
}

/// Add the currently staged object to content                                
///   @param data - mesh data                                                 
void Obj::flush_object(Data* data) {
   // Add object if not empty                                           
   if (data->object.face_count > 0)
      data->mesh->objects << Move(data->object);

   // Reset for more data                                               
   data->object.face_offset = data->mesh->face_vertices.GetCount();
   data->object.index_offset = data->mesh->indices.GetCount();
}

/// Add the currently staged group to content                                 
///   @param data - mesh data                                                 
void Obj::flush_group(Data* data) {
   // Add group if not empty                                            
   if (data->group.face_count > 0)
      data->mesh->groups << Move(data->group);

   // Reset for more data                                               
   data->group.face_offset = data->mesh->face_vertices.GetCount();
   data->group.index_offset = data->mesh->indices.GetCount();
}