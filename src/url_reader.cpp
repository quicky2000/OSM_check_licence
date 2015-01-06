/*
    This file is part of OSM_check_licence
    Copyright (C) 2012  Julien Thevenon ( julien_thevenon at yahoo.fr )

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include "url_reader.h"
#include "my_buffer.h"
#include <iostream>
#include <stdlib.h>

url_reader::url_reader(void):
  m_curl_handler(NULL)
{
  std::cout << "Init libcurl" << std::endl ;
  if(curl_global_init(CURL_GLOBAL_ALL))
    {
      std::cout << "Problem with curl initialisation" << std::endl ;
      exit(-1);
    }
  m_curl_handler = curl_easy_init();
}

url_reader::~url_reader(void)
{
  curl_easy_cleanup(m_curl_handler);
  curl_global_cleanup();
  std::cout << "libcurl cleanup done" << std::endl ;
}

url_reader & url_reader::get_instance(void)
{
  if(m_instance == NULL)
    {
      m_instance = new url_reader();
    }
  return *m_instance;
}

void url_reader::remove_instance(void)
{
  delete m_instance;
}

char * url_reader::escape_string(const char * p_str)
{
  return curl_easy_escape(m_curl_handler,p_str,0);
}


void url_reader::read_url(const char * p_url,my_buffer & p_buffer)
{
  std::cout << "URL is \"" << p_url << "\"" << std::endl ;
  curl_easy_setopt(m_curl_handler, CURLOPT_URL, p_url);
  curl_easy_setopt(m_curl_handler, CURLOPT_WRITEFUNCTION,url_reader::receive_data); 
  curl_easy_setopt(m_curl_handler, CURLOPT_WRITEDATA, (void *)&p_buffer);
  CURLcode res = curl_easy_perform(m_curl_handler);
  if(res)
    {
      std::cout << "Error when downloading \"" << p_url << "\"" << std::endl ;
    }
}

size_t url_reader::receive_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
  size_t l_real_size = size * nmemb ;
#ifdef DEBUG_URL_READER
  std::cout << "url_reader::receive data of size " << l_real_size << std::endl ;
#endif
  my_buffer * l_buffer = (my_buffer*) userp;
  l_buffer->add_data(l_real_size,buffer);
  return size*nmemb;
  
}

url_reader * url_reader::m_instance = NULL;

//EOF
