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
#include <stdint.h>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <assert.h>
#include <map>

class user_licence_checker
{
public:
  typedef enum {ACCEPTED,UNDECIDED,DECLINED} t_status;
  user_licence_checker(void);
  ~user_licence_checker(void);
  t_status check_status_profil_url(const std::string & p_url);
  t_status check_status_user(const std::string & p_user);
  static const std::string licence_to_str(t_status p_status);
private:
  const std::string get_user_url(const std::string & p_user);
  

  static std::string m_base_url;
  url_reader * m_url_reader;
};

user_licence_checker::user_licence_checker(void):
  m_url_reader(& url_reader::get_instance())
{
}

user_licence_checker::~user_licence_checker(void)
{
  url_reader::remove_instance();
}

user_licence_checker::t_status user_licence_checker::check_status_profil_url(const std::string & p_url)
{
  my_buffer l_buffer;
  m_url_reader->read_url(p_url.c_str(),l_buffer);

  std::stringstream l_stream;
  l_stream << l_buffer.get_data();

  bool l_found = false;
  t_status l_result = UNDECIDED;
  std::string l_line;
  while(!l_found && !getline(l_stream,l_line).eof())
    {
      size_t l_pos ;
      if((l_pos = l_line.find("Undecided")) != std::string::npos)
	{
	  l_found = true;
	  l_result = UNDECIDED;
	}
      else if((l_pos = l_line.find("Accepted")) != std::string::npos)
	{
	  l_found = true;
	  l_result = ACCEPTED;
	}
      else if((l_pos = l_line.find("Declined")) != std::string::npos)
	{
	  l_found = true;
	  l_result = DECLINED;
	}
    }
  assert(l_found);
  return l_result;
}

user_licence_checker::t_status user_licence_checker::check_status_user(const std::string & p_user)
{
  std::string l_user_url =  get_user_url(p_user);
  return check_status_profil_url(l_user_url);
}

const std::string user_licence_checker::get_user_url(const std::string & p_user)
{
  char * l_escaped_user = m_url_reader->escape_string(p_user.c_str());
  std::string l_user_url = m_base_url + l_escaped_user;
  curl_free(l_escaped_user);
  return l_user_url;
}

const std::string user_licence_checker::licence_to_str(t_status p_status)
{
  std::string l_result;
  switch(p_status)
    {
    case ACCEPTED:
      l_result = "ACCEPTED";
      break ;
    case UNDECIDED:
      l_result = "UNDECIDED";
      break;
    case DECLINED:
      l_result = "DECLINED";
      break;
    default:
      std::cout << "ERROR status \"" << p_status << "\"" <<std::endl ;
      exit(-1);
    }
  return l_result;
}

std::string user_licence_checker::m_base_url = "http://www.openstreetmap.org/user/";

class licence_checker
{
public:
  void check_message_box(const std::string & p_message_box);
  void generate_report(void);
private:
  std::map<std::string,user_licence_checker::t_status> m_status_users;
  std::map<user_licence_checker::t_status, uint32_t> m_stats;
  user_licence_checker m_user_licence_checker;
};

void licence_checker::check_message_box(const std::string & p_message_box)
{
  std::ifstream l_input_file;
  l_input_file.open(p_message_box.c_str(),std::ifstream::in);
  if(!l_input_file)
    {
      std::cout << "ERROR : not able to open file \"" << p_message_box << "\"" << std::endl ;
      exit(-1);
    }

  m_stats.insert(std::map<user_licence_checker::t_status, uint32_t>::value_type(user_licence_checker::UNDECIDED,0));
  m_stats.insert(std::map<user_licence_checker::t_status, uint32_t>::value_type(user_licence_checker::DECLINED,0));
  m_stats.insert(std::map<user_licence_checker::t_status, uint32_t>::value_type(user_licence_checker::ACCEPTED,0));

  std::string l_line;
  std::string l_user;
  std::string l_user_url;
  while(!getline(l_input_file,l_line).eof())
    {
      size_t l_pos ;
      if((l_pos = l_line.find("inbox-sender")) != std::string::npos)
	{
	  l_pos = l_line.find("href",l_pos);
	  assert(l_pos != std::string::npos);
	  l_line = l_line.substr(l_pos);
	  l_pos = l_line.find("\"");
	  assert(l_pos != std::string::npos);
	  l_line = l_line.substr(l_pos+1);
	  l_pos = l_line.find("\"");
	  assert(l_pos != std::string::npos);
	  l_user_url = l_line.substr(0,l_pos);
	  if(l_user_url.substr(0,5)=="/user")
	    {
	      l_user_url = "http://www.openstreetmap.org" + l_user_url ; 
	    }
	  l_pos = l_line.find(">");
	  assert(l_pos != std::string::npos);
	  l_line = l_line.substr(l_pos+1);
	  l_pos = l_line.find("<");
	  assert(l_pos != std::string::npos);
	  l_user = l_line.substr(0,l_pos);
	}
      if(l_line.find("inbox-subject") != std::string::npos && l_line.find("licence") != std::string::npos)
	{
	  std::map<std::string,user_licence_checker::t_status>::iterator l_iter = m_status_users.find(l_user);
	  if(l_iter == m_status_users.end())
	    {
	      std::cout << "User : \"" << l_user << "\" : <- get licence status" << std::endl ;
	      user_licence_checker::t_status l_status =  m_user_licence_checker.check_status_profil_url(l_user_url);
#ifdef DOUBLE_CHECK
	      assert(l_status == m_user_licence_checker.check_status_user(l_user));
	      std::cout << "URL = \"" << l_user_url << "\"" << std::endl ;
	      std::cout << "USER = \"" << l_user << "\"" << std::endl ;
	      std::cout << l_status << " " << l_user_url << std::endl ;
#endif
	      l_iter = m_status_users.insert(std::map<std::string,user_licence_checker::t_status>::value_type(l_user,l_status)).first;

	      std::map<user_licence_checker::t_status, uint32_t>::iterator l_stat_iter = m_stats.find(l_iter->second);
	      assert(l_stat_iter != m_stats.end());
	      l_stat_iter->second = 1 + l_stat_iter->second;
	    }
	  else
	    {
	      std::cout << "Already in cache" << std::endl ;
	    }
	  std::cout << user_licence_checker::licence_to_str(l_iter->second) << " \"" << l_user << "\"" << std::endl ;
	}    
    }
  l_input_file.close();
}

void licence_checker::generate_report(void)
{
  std::cout << "--------------------------------" << std::endl ;
  uint32_t l_nb_user = m_status_users.size();
  std::cout << "Nb users : " << l_nb_user << std::endl ;
  std::map<user_licence_checker::t_status, uint32_t>::const_iterator l_stat_iter = m_stats.begin();
  std::map<user_licence_checker::t_status, uint32_t>::const_iterator l_stat_iter_end = m_stats.end();
  while(l_stat_iter != l_stat_iter_end)
    {
      std::cout << user_licence_checker::licence_to_str(l_stat_iter->first) << " : " <<  l_stat_iter->second << "\t" << 100.0* ((double)l_stat_iter->second)/((double)l_nb_user) << std::endl ;
      ++l_stat_iter;
    }

  std::ofstream l_declined_file;
  std::string l_declined_name("DECLINED.txt");
  l_declined_file.open(l_declined_name.c_str());
  if(!l_declined_file)
    {
      std::cout << "ERROR : not able to open file \"" << l_declined_name << "\"" << std::endl ;
      exit(-1);
    }
  std::ofstream l_accepted_file;
  std::string l_accepted_name("ACCEPTED.txt");
  l_accepted_file.open(l_accepted_name.c_str());
  if(!l_accepted_file)
    {
      std::cout << "ERROR : not able to open file \"" << l_accepted_name << "\"" << std::endl ;
      exit(-1);
    }
  std::ofstream l_undecided_file;
  std::string l_undecided_name("UNDECIDED.txt");
  l_undecided_file.open(l_undecided_name.c_str());
  if(!l_undecided_file)
    {
      std::cout << "ERROR : not able to open file \"" << l_undecided_name << "\"" << std::endl ;
      exit(-1);
    }
  std::map<std::string,user_licence_checker::t_status>::const_iterator l_iter = m_status_users.begin();
  std::map<std::string,user_licence_checker::t_status>::const_iterator l_iter_end = m_status_users.end();
  while(l_iter!=l_iter_end)
    {
      switch(l_iter->second)
	{
	case user_licence_checker::ACCEPTED:
	  l_accepted_file << l_iter->first << std::endl ;	  
	  break ;
	case user_licence_checker::UNDECIDED:
	  l_undecided_file << l_iter->first << std::endl ;	  	  
	  break;
	case user_licence_checker::DECLINED:
	  l_declined_file << l_iter->first << std::endl ;
	  break;
	default:
	  std::cout << "ERROR status \"" << l_iter->second << "\"" <<std::endl ;
	  exit(-1);
	}
      
      ++l_iter;
    }
  l_accepted_file.close();
  l_undecided_file.close();
  l_declined_file.close();
}

int main(int argc,char ** argv)
{
  std::cout << "Nb arguments : " << argc << std::endl ;
  if(argc != 2)
    {
      std::cout << "Usage is check_licence.exe <mailbox_file>" << std::endl ;
      exit(-1);
    }

  licence_checker l_checker;
  l_checker.check_message_box(argv[1]);
  l_checker.generate_report();
}
