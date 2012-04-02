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
