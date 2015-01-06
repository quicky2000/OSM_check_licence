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
#ifndef _URL_READER_H_
#define _URL_READER_H_
#include <curl/curl.h>

class my_buffer;

class url_reader
{
 public:
  static url_reader & get_instance(void);
  static void remove_instance(void);
  void read_url(const char * p_url,my_buffer & p_buffer);
  char * escape_string(const char * p_str);
 private:
  /** 
      Private constructor called by singleton to insure that init
      of libcurl will done only once a time
  **/
  url_reader(void);

  /**
     Private constructor called by singleton to insure that ressources
      of libcurl will free only once a time
  **/
  ~url_reader(void);


  static size_t receive_data(void *buffer, size_t size, size_t nmemb, void *userp);

  CURL * m_curl_handler;
  static url_reader * m_instance;
};
#endif /* _URL_READER_H_ */
