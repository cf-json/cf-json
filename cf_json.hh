#ifndef STAR_JSON_HH
#define STAR_JSON_HH

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Purpose: Parse a CF-JSON format file
// External dependencies: 
// netCDF library (including HDF5, Curl, zlib, szip)
// Included dependencies
// JSON gason library
/////////////////////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <netcdf.h>
#include "gason.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//cf_json
/////////////////////////////////////////////////////////////////////////////////////////////////////

class cf_json
{
public:
  cf_json()
  {
  }
  int convert(const char* file_name);

private:
  void do_objects_group(JsonValue obj, const char* grp_name, int grp_id);
  int do_groups(JsonValue obj, const char* grp_name, int grp_id);
  int do_dimensions(JsonValue obj, const char* grp_name, int grp_id);
  int do_variables(JsonValue obj, const char* grp_name, int grp_id);
  int do_attributes(JsonValue obj, const char* grp_name, int grp_id);
  int get_variables(JsonValue obj, const char* var_name, int grp_id);
  int get_attributes(JsonValue obj, const char* obj_name, int grp_id);
};

#endif




