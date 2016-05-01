#if 0
g++ -std=c++14 -O0 -g -I../tjw twiebs-site.cpp 
exit
#endif

#define TJW_PLATFORM_IMPLEMENTATION
//#define SG_KEYWORD_COLOR "#98676A"
//#define SG_TYPE_COLOR "#E5C3C5"
//#define SG_STRING_COLOR "#218F51"

#define SG_IMPLEMENTATION
#include "site_generator.h"

#include <assert.h>
#include "tjw_platform.h"
#include "tjw_string_transform.h"

#include <stdint.h>
#include <dirent.h>
#include <string.h>
#include <string>

#define strlen_literal(literal) (sizeof(literal) - 1)
#define memcpy_lit_inc_ptr(dest, lit) memcpy(dest, lit, strlen_literal(lit)); dest+=strlen_literal(lit)
#define memcpy_and_increment(dest, src, size) memcpy(dest, src, size); dest+=size

#define sg_set_output_directory sg_set_output_directory_fmt
#define sg_set_document_description sg_set_document_description_fmt
#define sg_set_document_filename sg_set_document_filename_fmt
#define sg_set_next_class sg_set_next_class_fmt
#define sg_h sg_h_fmt
#define sg_begin_a sg_begin_a_fmt
#define sg_text sg_text_fmt
#define sg_set_document_title sg_set_document_title_fmt
#define sg_set_document_filename sg_set_document_filename_fmt

void twiebs_site_navigation() {
    sg_html("<nav class=\"navbar\">");
    sg_begin_a("index.html");
    sg_text("Posts");
    sg_end_a();
    sg_begin_a("projects.html");
    sg_text("Projects");
    sg_end_a();
    sg_begin_a("about.html");
    sg_text("About");
    sg_end_a();
    sg_begin_a("archive.html");
    sg_text("Archive");
    sg_end_a();
    sg_html("</nav>");
}

char *str_push_to_buffer_as_cstr(char **dest, const char *src, const size_t length) {
	char *result = *dest;
	memcpy(*dest, src, length);
	(*dest)[length] = 0;
	*dest += length + 1;
	return result;
}	 

inline char *
cstr_push_to_buffer(char **dest, const char *src) {
	char *result = *dest;
	size_t index = 0;	
	while (src[index] != 0) {
		(*dest)[index] = src[index];
		index++;
	}
		
	(*dest)[index] = 0;
	*dest += index + 1;
	return result;
}

static const char* MONTH_STRINGS[] {
  "NULLMONTH",
  "January",
  "Febuary",
  "March",
  "April",
  "May",
  "June",
  "July",
  "Agust",
  "Setember",
  "October",
  "November",
  "December"
};

static inline
void InsertDateTag(const sg_MarkdownProcedureCall& call) {
	int month_index = std::stoi(call.args[1].text);
	sg_h(5, "%.*s %s, %d", call.args[2].text_length, call.args[2].text, MONTH_STRINGS[month_index], std::stoi(call.args[0].text));
}
//#define sg_html sg_html_fmt

int main()
{
    sg_set_output_directory("twiebs.github.io");

		char *post_info_buffer = (char *)malloc(1024*1024*4);
    memset(post_info_buffer, 0, 1024*1024*4);
		char *buffer_write = post_info_buffer;

		struct PostInfo {
			const char *filepath;
			const char *title;
			const char *permalink;
      uint8_t day;
      uint8_t month;
      uint16_t year; 
			uint64_t dateComparator;
		} post_info[1024];
		uint32_t post_index = 0;

		{ //Generate Posts
			char temp[1024];
			foreach_file_in_directory("posts", [&](FileInfo file_info) {
				char *data = read_file_into_memory_and_null_terminate(file_info.filepath);
				cstrcpy_without_filext(temp, file_info.name);

				sg_begin_document();
				sg_set_document_title(temp);
				sg_set_document_description("Torin wiebelts blog");
				sg_add_stylesheet_cstr("style.css");
				sg_set_document_filename(file_info.name);

				twiebs_site_navigation();
				sg_set_next_class("content");
				sg_begin_div();

				post_info[post_index].filepath = cstr_push_to_buffer(
						&buffer_write, file_info.filepath);

				sg_Markdown_Result parse_info;
				sg_parser_begin(data);

				while(sg_parse_markdown(&parse_info)) {
					if (parse_info.type == SG_MARKDOWN_PROCEDURE) {

						sg_MarkdownProcedureCall& call = parse_info.procedure_call;
						if (str_match_literal(call.name, call.name_length, "title")) {
							post_info[post_index].title = str_push_to_buffer_as_cstr(
									&buffer_write, call.args[0].text, call.args[0].text_length);
							sg_set_document_title(post_info[post_index].title);
							sg_h(1, post_info[post_index].title);
						} else if (str_match_literal(call.name, call.name_length, "date")) {
              PostInfo& p = post_info[post_index];
              p.year = string_to_uint64(call.args[0].text, call.args[0].text_length);
              p.month = string_to_uint64(call.args[1].text, call.args[1].text_length);
              p.day  = string_to_uint64(call.args[2].text, call.args[2].text_length);
              p.dateComparator = (p.year * 366) + (p.month * 32) + (p.day);
							InsertDateTag(call);
						} else if (str_match_literal(call.name, call.name_length, "permalink")) {
							post_info[post_index].permalink = str_push_to_buffer_as_cstr(
									&buffer_write, call.args[0].text, call.args[0].text_length);
							sg_set_document_filename(post_info[post_index].permalink);
						}
					}
				}
        sg_parser_end();
				
				post_index++;
				sg_end_div();
				sg_end_document();
				free(data);
			});
    }

    //dumb bubble sort
    bool are_posts_sorted = false;
    while (!are_posts_sorted) {
      are_posts_sorted = true;
      for (uint32_t i = 0; i < post_index - 1; i++) {
        if (post_info[i].dateComparator < post_info[i+1].dateComparator) {
          PostInfo temp = post_info[i];
          post_info[i] = post_info[i+1];
          post_info[i+1] = temp;
          are_posts_sorted = false;
        }
      }
    }
   
    { //Generate Pages
			char temp[1024];
			foreach_file_in_directory("pages", [&temp](FileInfo file_info){
				char *fileData = read_file_into_memory_and_null_terminate(file_info.filepath);
				cstrcpy_without_filext(temp, file_info.name);

				sg_begin_document();
				sg_set_document_title_fmt(temp);
				sg_set_document_filename_fmt("%s.html", temp);
				sg_set_document_description_fmt("Blog of Torin Wiebelt");
				sg_add_stylesheet_cstr("style.css");

				twiebs_site_navigation();
				sg_parser_begin(fileData);
				sg_Markdown_Result parse_info;
				while (sg_parse_markdown(&parse_info)) {
					if (parse_info.type == SG_MARKDOWN_PROCEDURE) {
						if (str_match_literal(parse_info.procedure_call.name, 
							parse_info.procedure_call.name_length, "begin_content")) {
							sg_set_next_class_fmt("content");
							sg_begin_div();
						} else if (str_match_literal(parse_info.procedure_call.name, 
							parse_info.procedure_call.name_length, "end_content")) {
							sg_end_div();
						}
					}
				}
        sg_parser_end();
				sg_end_document();
				free(fileData);
			});
		}	

    { //@Generate @Archive
      sg_begin_document();
      sg_set_document_title("Archive");
      sg_set_document_filename("archive.html");
      sg_add_stylesheet_cstr("style.css");
      twiebs_site_navigation();

      uint64_t currentIndex = 0;

     
      uint64_t currentYear = post_info[0].year;
      sg_set_next_class("content");
      sg_begin_div();
      sg_h(1, std::to_string(currentYear).c_str());

      for (uint32_t i = 0; i < post_index; i++) {
        const PostInfo& p = post_info[i];
        if (currentYear != p.year) {
          sg_end_div();
          sg_set_next_class("content");
          sg_begin_div();
          sg_h(1, std::to_string(currentYear).c_str());
          currentYear = p.year;
        }

        sg_begin_a("%s.html", p.permalink);
        sg_h(3, "%s %s, %s", MONTH_STRINGS[p.month], std::to_string(p.day).c_str(), p.title);
        sg_end_a();
      }
      sg_end_div();
      sg_end_document();
    }
    { //@Generate @Site @Index
        sg_begin_document();
        sg_set_document_title_fmt("Torin Wiebelt's Blog");
        sg_set_document_description_fmt("Torin wiebelts awesome blog and portfolio");
        sg_set_document_filename("index.html");
        sg_add_stylesheet_cstr("style.css");

				twiebs_site_navigation();
				//sg_h(1, "Recent Posts");

				for (uint32_t i = 0; i < post_index; i++) {
					char *data = read_file_into_memory_and_null_terminate(post_info[i].filepath);
					sg_set_next_class_fmt("content");
					sg_begin_div();
					sg_begin_a_fmt("%s.html", post_info[i].permalink);
					sg_h(1,post_info[i].title);
					sg_end_a();

					sg_parser_begin(data);
          sg_Markdown_Result parse_result;
					while (sg_parse_markdown(&parse_result)) { 
						if (parse_result.type = SG_MARKDOWN_PROCEDURE) {
							if (str_match_literal(parse_result.procedure_call.name, parse_result.procedure_call.name_length, "date")) {
								InsertDateTag(parse_result.procedure_call);
							}
						}
					}
          sg_parser_end();
					sg_end_div();
					free(data);
				}
        sg_end_document();
    }
   
    //sg_CopyFiles();
    return 0;
}
