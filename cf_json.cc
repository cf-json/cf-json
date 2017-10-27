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
  if (status != JSON_OK)
  {
    std::cout << "invalid JSON format for " << file_name << std::endl;
    return -1;
  }

  std::string name(file_name);
  name += ".nc";
  if (nc_create(name.c_str(), NC_WRITE, &grp_id) != NC_NOERR)
  {
    return -1;
  }

  do_objects_group(value, "/", grp_id);

  if (nc_close(grp_id) != NC_NOERR)
  {

  }

  free(buf);
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//cf_json::do_group_objects
//choose between JSON objects that represent:
//groups, attributes, variables, dimensions
//any of these JSON objects are located at each group
/////////////////////////////////////////////////////////////////////////////////////////////////////

void cf_json::do_objects_group(JsonValue obj, const char* grp_name, int grp_id)
{
  //parameter must be JSON object 
  assert(obj.getTag() == JSON_OBJECT);
  JsonNode *node;

  std::cout << grp_name << ": has ";
  for (node = obj.toNode(); node != nullptr; node = node->next)
  {
    std::cout << node->key << ",";
  }
  std::cout << std::endl;

  for (node = obj.toNode(); node != nullptr; node = node->next)
  {
    if (std::string(node->key).compare("groups") == 0)
    {
      do_groups(node->value, grp_name, grp_id);
    }
    else if (std::string(node->key).compare("attributes") == 0)
    {
      do_attributes(node->value, grp_name, grp_id);
    }
    else if (std::string(node->key).compare("dimensions") == 0)
    {
      do_dimensions(node->value, grp_name, grp_id);
    }
    else if (std::string(node->key).compare("variables") == 0)
    {
      do_variables(node->value, grp_name, grp_id);
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//cf_json::do_groups
/////////////////////////////////////////////////////////////////////////////////////////////////////

int cf_json::do_groups(JsonValue obj, const char* grp_name, int grp_id)
{

  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//cf_json::do_dimensions
/////////////////////////////////////////////////////////////////////////////////////////////////////

int cf_json::do_dimensions(JsonValue obj, const char* grp_name, int grp_id)
{


  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//cf_json::do_variables
/////////////////////////////////////////////////////////////////////////////////////////////////////

int cf_json::do_variables(JsonValue obj, const char* grp_name, int grp_id)
{

  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//cf_json::do_attributes
/////////////////////////////////////////////////////////////////////////////////////////////////////

int cf_json::do_attributes(JsonValue obj, const char* grp_name, int grp_id)
{

  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//cf_json::get_variables
/////////////////////////////////////////////////////////////////////////////////////////////////////

int cf_json::get_variables(JsonValue obj, const char* var_name, int grp_id)
{

  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//cf_json::get_attributes
/////////////////////////////////////////////////////////////////////////////////////////////////////

int cf_json::get_attributes(JsonValue obj, const char* obj_name, int grp_id)
{


  return 0;
}


