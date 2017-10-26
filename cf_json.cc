#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "gason.h"
#include <netcdf.h>
#include <assert.h>
#include "cf_json.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//cf_json::convert
/////////////////////////////////////////////////////////////////////////////////////////////////////

int cf_json::convert(const char* file_name)
{
  char *buf = 0;
  size_t length;
  int grp_id;
  FILE *f;
  JsonValue root;

  f = fopen(file_name, "rb");
  if (!f)
  {
    std::cout << "cannot open " << file_name << std::endl;
    return -1;
  }

  fseek(f, 0, SEEK_END);
  length = ftell(f);
  fseek(f, 0, SEEK_SET);
  buf = (char*)malloc(length);
  if (buf)
  {
    fread(buf, 1, length, f);
  }
  fclose(f);

  char *endptr;
  JsonValue value;
  JsonAllocator allocator;
  int status = jsonParse(buf, &endptr, &value, allocator);
  if (status != JSON_OK) {
    exit(EXIT_FAILURE);
  }

  std::string name(file_name);
  name += ".nc";
  if (nc_create(name.c_str(), NC_WRITE, &grp_id) != NC_NOERR)
  {
    return -1;
  }

  if (nc_close(grp_id) != NC_NOERR)
  {

  }

  free(buf);
  return 0;
}
