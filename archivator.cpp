#include <dirent.h>
#include <cstring>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
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


void pack(string dir,vector<string> files)
{
    struct stat statbuf;
    char mode[6], buff[1024];
    int out1, out2;
    string path1("./"+dir+"_arch.txt");
    out1 = open(path1.c_str(), O_WRONLY|O_CREAT, S_IREAD|S_IWRITE);
    string path2("./"+dir+"_data.txt");
    out2 = open(path2.c_str(), O_WRONLY|O_CREAT, S_IREAD|S_IWRITE);
    if (out1==-1)
    {
        close(out1);
        unlink(path1.c_str());
        out1 = open(path1.c_str(), O_CREAT, S_IREAD|S_IWRITE);
    }
    if (out2==-1)
    {
        close(out2);
        unlink(path2.c_str());
        out2 = open(path2.c_str(), O_CREAT, S_IREAD|S_IWRITE);
    }
    for (unsigned int i=0; i<files.size();i++)
    {
        lstat(files[i].c_str(), &statbuf);
        path1 = files[i] + "\n";
        write(out1, path1.c_str(),path1.size());
        sprintf(mode,"%i\n",statbuf.st_mode);
        write(out1, mode, 6);
        int in = open(files[i].c_str(), O_RDONLY, S_IREAD);
        while (read(in,buff,sizeof(buff)))
        {
            write(out2,buff,sizeof(buff));
        }
        write(out2,"stop_slovo\n",11);
    }
    close(out1);
    close(out2);
}


int main(int n, char **argc)
{
	if (n > 1)
	{
        string mode(argc[1]);
        string dir(argc[2]);
        vector<string> files(find_files(dir));
        if (mode == "pack")
        {
            pack(dir,files);
        }
        if (mode == "unpack")
        {
            //unpack();
        }
    }
    else 
    {
        cout<<"Few arguments"<<endl;
    }
    return 0;
}