#define IMWEB_IMPLEMENTATION
#define TJW_PLATFORM_IMPLEMENTATION
#define SG_KEYWORD_COLOR "#98676A"
#define SG_TYPE_COLOR "#E5C3C5"
#define SG_STRING_COLOR "#218F51"

#include "imweb.h"

#include <functional>
#include "tjw_platform.h"
#include "tjw_string_transform.h"

#include <stdint.h>
#include <dirent.h>

#define strlen_literal(literal) (sizeof(literal) - 1)
#define memcpy_lit_inc_ptr(dest, lit) memcpy(dest, lit, strlen_literal(lit)); dest+=strlen_literal(lit)
#define memcpy_and_increment(dest, src, size) memcpy(dest, src, size); dest+=size

static inline
char *ReadFileIntoMemory(const char *filename) {
    char *result = 0;
    FILE *file = fopen(filename, "rb");
    if (file) {
        fseek(file, 0, SEEK_END);
        size_t file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        result = (char *)malloc(file_size + 1);
        result[file_size] = 0;
        fread(result, 1, file_size, file);
        fclose(file);
    }
    return result;
}

void InsertPageNavigation() {
    sg::HTML("<nav class=\"navbar\">");
    sg::begin_a("index.html");
    sg::text("Posts");
    sg::end_a();
    sg::begin_a("projects.html");
    sg::text("Projects");
    sg::end_a();
    sg::begin_a("about.html");
    sg::text("About");
    sg::end_a();
    sg::begin_a("archive.html");
    sg::text("Archive");
    sg::end_a();
    sg::HTML("</nav>");
}

void InsertPageHeader(const char *title) {
  #if 0
    sg::BeginDiv("page-header");
    sg::Heading(1, title);
    sg::EndDiv();
    #endif
}

static bool hr_toggle = true;

int main()
{
    char temp[1024];
    sg::set_output_directory("twiebs.github.io");

    
    { //Generate Site Index
        sg::begin_document();
        sg::set_document_title("Torin Wiebelt's Blog");
        sg::set_document_description("Torin wiebelts awesome blog and portfolio");
        sg::set_output_filename("index.html");
        sg::add_stylesheet("style.css");
        InsertPageNavigation();
        //sg::h(1, "Current Posts");
        
        foreach_file_in_directory("posts/", [](FileInfo info) {
            char *data = ReadFileIntoMemory(info.filepath);
            sg::begin_div("content");
            sg::markdown(data, [](sg::MarkdownProcedureCall& call) {
                if (str_match_literal(call.name, call.name_length, "title")) {
                  assert(call.arg_count == 1);
                  sg::begin_a("index.html");
                  sg::h(1, "%.*s", call.args[0].text_length, call.args[0].text);
                  sg::end_a();
                } else if (str_match_literal(call.name, call.name_length, "date")) {
                  #if 0
                  assert(call.arg_count == 3);
                  sg::h(3, "%.*s, %.*s, %.*s", call.args[0].text_length, call.args[0].text,
                            call.args[1].text_length, call.args[1].text, call.args[2].text_length, call.args[2].text);
                  #endif
                }
            });
            sg::end_div();
            free(data);
        });

        sg::end_document();
    }
    { //Generate Posts

        foreach_file_in_directory("posts", [&temp](FileInfo file_info){
            char *write = temp;
            memcpy_lit_inc_ptr(write, "posts/");
            memcpy_and_increment(write, file_info.name, strlen(file_info.name));
            write[0] = 0;
            
            char *data = ReadFileIntoMemory(temp);
            write = temp;
            memcpy_and_increment(write, file_info.name, strlen(file_info.name) - 3);
            write[0] = 0;

            sg::begin_document();
            sg::set_document_title(temp);
            sg::set_document_description("Torin wiebelts blog");
            sg::add_stylesheet("style.css");
            sg::set_output_filename(file_info.name);
            InsertPageNavigation();
            sg::begin_div("content");
            //sg::Heading(file->d_name, 1);
            sg::markdown(data);
            sg::end_div();
            sg::end_document();
            free(data);
        });
    }
   
    { //Generate Pages

        sgSetHRProc([]() {
            if (hr_toggle) {
                sg::begin_div("content");
            } else {
                sg::end_div();
            }
            hr_toggle = !hr_toggle;
        });
                
        char temp[1024];
        foreach_file_in_directory("pages", [&temp](FileInfo file_info){
            char *write = temp;
            memcpy_lit_inc_ptr(write, "pages/");
            memcpy_and_increment(write, file_info.name, strlen(file_info.name));
            char *fileData = ReadFileIntoMemory(temp);
            write = temp;
            memcpy_and_increment(write, file_info.name, strlen(file_info.name) - 3);
            write[0] = 0;
            hr_toggle = true;
            sg::begin_document();
            sg::set_document_title(temp);
            sg::set_document_description("Blog of Torin Wiebelt");
            sg::add_stylesheet("style.css");
            InsertPageNavigation();
            InsertPageHeader(temp);
            sg::markdown(fileData);
	    free(fileData);
            sg::end_document();
        });
    }
    
    sg::CopyFiles();
    return 0;
}
