#include <dirent.h>
#include <cstring>
#include <vector>
#include <stdio.h>
#include <iostream>
using namespace std;


string path_separator()
{
	#ifdef _WIN32
		return "\\";

	#else
		return "/";
	#endif
}


vector<string> find_files(string current_dir)
{//поиск файлов в директории current_dir
     //открыли дискриптор данных в директории
	vector<string> files;
	vector<string> buff_files;
	string path;
	DIR *dir = opendir(current_dir.c_str());
    if (dir)
    {
        dirent *entry = NULL;
        while ((entry = readdir(dir)))
        {//чтение названий файлов в директории
        if (entry->d_type == DT_DIR)
        {
        	if (strcmp(entry->d_name,".")==0 || strcmp(entry->d_name,"..")==0)
        		continue;
        	buff_files = find_files(current_dir + path_separator() + entry->d_name);
        	unsigned buff_size = buff_files.size();
        	for (unsigned int i =0; i < buff_size; i++)
        	{
        		files.push_back(buff_files[i]);
        	}
     	}
        else
        {
        	path = current_dir + path_separator() + entry->d_name;
        	files.push_back(path);
        }
        }
        //закрыли дискриптор
        closedir(dir);
    }
    else cout<<"Directory didn't open.";
    return files;
}

int main(int n, char **argc)
{
	if (n > 1)
	{
        string mode(argc[1]);
        string dir(argc[2]);
        vector<string> files(find_files(dir));
        for(int i=0; i<files.size();i++)
            cout<<files[i]<<endl;
    }
    else 
    {
        cout<<"Few arguments"<<endl;
    }
    return 0;
}