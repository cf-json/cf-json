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

void cf_json::do_objects_group(JsonValue value, const char* grp_name, int grp_id)
{
  //parameter must be JSON object 
  assert(value.getTag() == JSON_OBJECT);
  JsonNode *node;

  std::cout << grp_name << ": has ";
  for (node = value.toNode(); node != nullptr; node = node->next)
  {
    std::cout << node->key << ",";
  }
  std::cout << std::endl;

  for (node = value.toNode(); node != nullptr; node = node->next)
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

int cf_json::do_groups(JsonValue value, const char* grp_name, int grp_id)
{
  //parameter must be JSON object 
  assert(value.getTag() == JSON_OBJECT);

  for (JsonNode *node = value.toNode(); node != nullptr; node = node->next)
  {
    assert(node->value.getTag() == JSON_OBJECT);
    std::cout << grp_name << ":group:" << node->key << std::endl;

    //iterate in subgroup with name 'node->key'
    do_objects_group(node->value, node->key, grp_id);
  }

  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//cf_json::do_dimensions
/////////////////////////////////////////////////////////////////////////////////////////////////////

int cf_json::do_dimensions(JsonValue value, const char* grp_name, int grp_id)
{
  //parameter must be JSON object 
  assert(value.getTag() == JSON_OBJECT);

  for (JsonNode *node = value.toNode(); node != nullptr; node = node->next)
  {
    //dimension value must be a number
    assert(node->value.getTag() == JSON_NUMBER);
    int dim = (int)node->value.toNumber();
    std::cout << grp_name << ":dimension:" << node->key << ":" << dim << std::endl;
  }

  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//cf_json::do_variables
/////////////////////////////////////////////////////////////////////////////////////////////////////

int cf_json::do_variables(JsonValue value, const char* grp_name, int grp_id)
{
  //parameter must be JSON object 
  assert(value.getTag() == JSON_OBJECT);

  for (JsonNode *node = value.toNode(); node != nullptr; node = node->next)
  {
    assert(node->value.getTag() == JSON_OBJECT);
    std::cout << grp_name << ":variable:" << node->key << std::endl;

    //get variables with name 'node->key'
    if (get_variables(node->value, node->key, grp_id) < 0)
    {
      return -1;
    }
  }

  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//cf_json::do_attributes
/////////////////////////////////////////////////////////////////////////////////////////////////////

int cf_json::do_attributes(JsonValue value, const char* grp_name, int grp_id)
{
  //parameter must be JSON object 
  assert(value.getTag() == JSON_OBJECT);

  for (JsonNode *node = value.toNode(); node != nullptr; node = node->next)
  {
    std::cout << node->key << ",";

    //get attributes with name 'node->key'
    if (get_attributes(node->value, node->key, grp_id) < 0)
    {
      return -1;
    }
  }

  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//cf_json::get_variables
/////////////////////////////////////////////////////////////////////////////////////////////////////

int cf_json::get_variables(JsonValue value, const char* var_name, int grp_id)
{

  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//cf_json::get_attributes
/////////////////////////////////////////////////////////////////////////////////////////////////////

int cf_json::get_attributes(JsonValue value, const char* obj_name, int grp_id)
{


  return 0;
}


