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

const int SHIFT_WIDTH = 4;
void dump_string(const char *s);

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

void cf_json::do_objects_group(JsonValue value, const char* grp_name, int grp_id, int indent)
{
  //parameter must be JSON object 
  assert(value.getTag() == JSON_OBJECT);

  if (!value.toNode())
  {
    //empty group
    fprintf(stdout, "{}");
    fprintf(stdout, "\n");
    return;
  }

  //start JSON object
  fprintf(stdout, "{\n");

  for (JsonNode *node = value.toNode(); node != nullptr; node = node->next)
  {
    assert(node->value.getTag() == JSON_OBJECT);

    fprintf(stdout, "%*s", indent + SHIFT_WIDTH, "");
    dump_string(node->key);
    fprintf(stdout, ": ");

    if (std::string(node->key).compare("groups") == 0)
    {
      do_groups(node->value, grp_name, grp_id, indent + SHIFT_WIDTH);
    }
    else if (std::string(node->key).compare("attributes") == 0)
    {
      do_attributes(node->value, grp_name, grp_id, indent + SHIFT_WIDTH);
    }
    else if (std::string(node->key).compare("dimensions") == 0)
    {
      do_dimensions(node->value, grp_name, grp_id, indent + SHIFT_WIDTH);
    }
    else if (std::string(node->key).compare("variables") == 0)
    {
      do_variables(node->value, grp_name, grp_id, indent + SHIFT_WIDTH);
    }

    //JSON object separator
    if (node->next)
    {
      fprintf(stdout, "\n");
      fprintf(stdout, "%*s,\n", indent + SHIFT_WIDTH, "");
    }
    else
    {
      fprintf(stdout, "\n");
    }
  }

  //end JSON object
  fprintf(stdout, "%*s}\n", indent, "");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//cf_json::do_groups
/////////////////////////////////////////////////////////////////////////////////////////////////////

int cf_json::do_groups(JsonValue value, const char* grp_name, int grp_id, int indent)
{
  //parameter must be JSON object 
  assert(value.getTag() == JSON_OBJECT);

  //start JSON object
  fprintf(stdout, "{\n");

  for (JsonNode *node = value.toNode(); node != nullptr; node = node->next)
  {
    assert(node->value.getTag() == JSON_OBJECT);

    fprintf(stdout, "%*s", indent + SHIFT_WIDTH, "");
    dump_string(node->key);
    fprintf(stdout, ": ");

    //iterate in subgroup with name 'node->key'
    do_objects_group(node->value, node->key, grp_id, indent + SHIFT_WIDTH);
  }

  //end JSON object
  fprintf(stdout, "%*s}\n", indent, "");

  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//cf_json::do_dimensions
/////////////////////////////////////////////////////////////////////////////////////////////////////

int cf_json::do_dimensions(JsonValue value, const char* grp_name, int grp_id, int indent)
{
  //parameter must be JSON object 
  assert(value.getTag() == JSON_OBJECT);

  //start JSON object
  fprintf(stdout, "{\n");

  for (JsonNode *node = value.toNode(); node != nullptr; node = node->next)
  {
    //key, dimension name
    fprintf(stdout, "%*s", indent + SHIFT_WIDTH, "");
    dump_string(node->key);
    fprintf(stdout, ":");

    //dimension value must be a number
    assert(node->value.getTag() == JSON_NUMBER);
    int dim = (int)node->value.toNumber();
    fprintf(stdout, "%d", dim);

    //JSON object separator
    if (node->next)
    {
      fprintf(stdout, "\n");
      fprintf(stdout, "%*s,\n", indent + SHIFT_WIDTH, "");
    }
    else
    {
      fprintf(stdout, "\n");
    }
  }

  //end JSON object
  fprintf(stdout, "%*s}", indent, "");

  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//cf_json::do_variables
/////////////////////////////////////////////////////////////////////////////////////////////////////

int cf_json::do_variables(JsonValue value, const char* grp_name, int grp_id, int indent)
{
  //parameter must be JSON object 
  assert(value.getTag() == JSON_OBJECT);

  //start JSON object
  fprintf(stdout, "{\n");

  for (JsonNode *node = value.toNode(); node != nullptr; node = node->next)
  {
    assert(node->value.getTag() == JSON_OBJECT);

  }

  //end JSON object
  fprintf(stdout, "%*s}", indent, "");
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//cf_json::do_attributes
/////////////////////////////////////////////////////////////////////////////////////////////////////

int cf_json::do_attributes(JsonValue value, const char* grp_name, int grp_id, int indent)
{
  //parameter must be JSON object 
  assert(value.getTag() == JSON_OBJECT);

  //start JSON object
  fprintf(stdout, "{\n");

  for (JsonNode *node = value.toNode(); node != nullptr; node = node->next)
  {

  }

  //end JSON object
  fprintf(stdout, "%*s}", indent, "");
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//cf_json::get_variable_data
/////////////////////////////////////////////////////////////////////////////////////////////////////

int cf_json::get_variable_data(JsonValue value, const char* var_name, int grp_id)
{
  //parameter must be JSON object 
  assert(value.getTag() == JSON_OBJECT);

  for (JsonNode *node = value.toNode(); node != nullptr; node = node->next)
  {
    if (std::string(node->key).compare("shape") == 0)
    {
      //"shape" object must be a JSON array 
      assert(node->value.getTag() == JSON_ARRAY);
      JsonValue arr_dimensions = node->value;

      size_t arr_size = 0; //size of dimensions array
      for (JsonNode *n = arr_dimensions.toNode(); n != nullptr; n = n->next)
      {
        arr_size++;
        assert(n->value.getTag() == JSON_STRING);
      }

    }
    else if (std::string(node->key).compare("type") == 0)
    {
      //"type" object must be a JSON string 
      assert(node->value.getTag() == JSON_STRING);

    }
    else if (std::string(node->key).compare("data") == 0)
    {
      //"data" object can be a JSON array for netCDF numerical types 
      //or a JSON string for netCDF char 
      assert(node->value.getTag() == JSON_ARRAY || node->value.getTag() == JSON_STRING);
    }
    else if (std::string(node->key).compare("attributes") == 0)
    {
      do_attributes(node->value, node->key, grp_id);
    }
  }

  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//cf_json::dump_value
/////////////////////////////////////////////////////////////////////////////////////////////////////

int cf_json::dump_value(JsonValue o, int indent)
{
  switch (o.getTag())
  {
  case JSON_NUMBER:
    fprintf(stdout, "%f", o.toNumber());
    break;
  case JSON_STRING:
    dump_string(o.toString());
    break;
  case JSON_ARRAY:
    if (!o.toNode())
    {
      fprintf(stdout, "[]");
      break;
    }
    fprintf(stdout, "[\n");
    for (auto i : o)
    {
      fprintf(stdout, "%*s", indent + SHIFT_WIDTH, "");
      dump_value(i->value, indent + SHIFT_WIDTH);
      fprintf(stdout, i->next ? ",\n" : "\n");
    }
    fprintf(stdout, "%*s]", indent, "");
    break;
  case JSON_OBJECT:
    if (!o.toNode())
    {
      fprintf(stdout, "{}");
      break;
    }
    fprintf(stdout, "{\n");
    for (auto i : o)
    {
      fprintf(stdout, "%*s", indent + SHIFT_WIDTH, "");
      dump_string(i->key);
      fprintf(stdout, ": ");
      dump_value(i->value, indent + SHIFT_WIDTH);
      fprintf(stdout, i->next ? ",\n" : "\n");
    }
    fprintf(stdout, "%*s}", indent, "");
    break;
  case JSON_TRUE:
    fprintf(stdout, "true");
    break;
  case JSON_FALSE:
    fprintf(stdout, "false");
    break;
  case JSON_NULL:
    fprintf(stdout, "null");
    break;
  }
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//dump_string
/////////////////////////////////////////////////////////////////////////////////////////////////////

void dump_string(const char *s)
{
  fputc('"', stdout);
  while (*s)
  {
    int c = *s++;
    switch (c)
    {
    case '\b':
      fprintf(stdout, "\\b");
      break;
    case '\f':
      fprintf(stdout, "\\f");
      break;
    case '\n':
      fprintf(stdout, "\\n");
      break;
    case '\r':
      fprintf(stdout, "\\r");
      break;
    case '\t':
      fprintf(stdout, "\\t");
      break;
    case '\\':
      fprintf(stdout, "\\\\");
      break;
    case '"':
      fprintf(stdout, "\\\"");
      break;
    default:
      fputc(c, stdout);
    }
  }
  fprintf(stdout, "%s\"", s);
}
