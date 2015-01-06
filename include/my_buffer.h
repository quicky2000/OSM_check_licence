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
#ifndef _MY_BUFFER_H_
#define _MY_BUFFER_H_

#include <stdio.h>
/**
   Extensible buffer
**/
class my_buffer
{
 public:
  my_buffer(void);
  void add_data(size_t p_size, void * p_data);
  const char * const get_data(void)const;
  size_t get_size(void)const;
  void clear(void);
  ~my_buffer(void);
 private:
  size_t m_size;
  char * m_data;
};

#endif /* _MY_BUFFER_H_ */
