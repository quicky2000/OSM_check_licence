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
