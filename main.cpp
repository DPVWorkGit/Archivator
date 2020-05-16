#include <iostream>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

#define BUFF_SIZE 1024


bool ReadFile( string file_path, string arch_path, string arch_meta_path )
{
	int		inner_file, arch_file, arch_meta_file;

	inner_file	=	open( file_path.c_str(), O_RDONLY, S_IREAD|S_IWRITE );
	if ((arch_file 	=	open( arch_path.c_str(), O_WRONLY|O_APPEND, S_IREAD|S_IWRITE )) == -1)
		return false;

	if ( inner_file != -1 )
	{
		unsigned int	file_size 			=	0;
		char			reading_buffer[BUFF_SIZE];

		while ( true )
		{
			unsigned int count_bytes	=	read( inner_file, reading_buffer, BUFF_SIZE );

			if ( count_bytes )
			{
				file_size += count_bytes;
				write( arch_file, reading_buffer, count_bytes );
			}
			else
				break;
		}

		close( inner_file );
		close( arch_file );

		if ((arch_meta_file 	=	open( arch_meta_path.c_str(), O_WRONLY|O_APPEND, S_IREAD|S_IWRITE )) == -1)
			return false;

		write( arch_meta_file, ( file_path + "\n" ).c_str(), file_path.size()+1 );
		write( arch_meta_file, ( to_string( file_size ) + "\n").c_str(),( to_string( file_size ) + "\n" ).size() );

		close( arch_meta_file );

		return true;

	}
	else
		return false;
};


void CreateDirs( string dir_path )
{
	string buffer_path_str;

	while ( unsigned int find_number = dir_path.find( '/' ) )
	{
		DIR	*current_dir;

		if ( find_number == -1 )
			break;

		buffer_path_str	+=	dir_path.substr( 0, find_number + 1);
		dir_path	=	dir_path.substr( find_number +1, dir_path.size() );

		if ( ( current_dir	=	opendir( buffer_path_str.c_str() ) ) == NULL )
		{
			mkdir( buffer_path_str.c_str(), S_IRUSR|S_IWUSR|S_IXUSR );
		}
		else
		{
			closedir( current_dir );
		}

	}
}


bool ReadArchMeta( string placement_path, string arch_path, string arch_meta_path )
{
	string 	buffer_str;
	int		meta_file, arch_file, writing_file;

	meta_file =	open( arch_meta_path.c_str(), O_RDONLY, S_IREAD|S_IWRITE );
	if ((arch_file	=	open( arch_path.c_str(), O_RDONLY, S_IREAD|S_IWRITE )) == -1)
		return false;

	if ( meta_file != -1 )
	{
		unsigned int	file_size 			=	0;
		string 			path_str;
		string 			rewriting_buffer_str;
		bool			find_flag 			= false;
		char			reading_buffer[BUFF_SIZE];

		while ( true )
		{
			unsigned int 	count_bytes		=	read( meta_file, reading_buffer, BUFF_SIZE );

			if ( count_bytes )
			{
				buffer_str					+= string( reading_buffer ).substr( 0, count_bytes);

				while ( unsigned int find_number = buffer_str.find( '\n' ) )
				{
					if ( find_number == -1 )
						break;

					if ( !find_flag )
					{
						path_str			=	buffer_str.substr( 0, find_number );
						buffer_str			=	buffer_str.substr( find_number + 1 , buffer_str.size() );
						path_str 			=	placement_path + path_str.substr( path_str.find( "./" ), path_str.size() );
						find_flag 			=	true;
					}
					else
					{
						file_size			=	stoi( buffer_str.substr( 0, find_number ) );

						CreateDirs( path_str );

						if ((writing_file		=	open( path_str.c_str(), O_CREAT|O_WRONLY, S_IREAD|S_IWRITE )) == -1)
							return false;


						if (writing_file != -1)
						{
							while ( true )
							{
								unsigned int current_file_size;
								if(file_size >= BUFF_SIZE)
									current_file_size = BUFF_SIZE;
								else
									current_file_size = file_size;
								current_file_size = read(arch_file, reading_buffer, current_file_size);
								write(writing_file, reading_buffer, current_file_size);
								file_size -= current_file_size;
								if(file_size == 0)
									break;
							}

							close( writing_file );
						}

						buffer_str			=	buffer_str.substr( find_number + 1 , buffer_str.size() );
						find_flag			=	false;
					}					

				}

			}
			else
				break;

		}
		close( arch_file );
		close( meta_file );
	}
	else 
	{
		return false;
	}

	return true;
}


bool MakeDirMap( string dir, string arch_path, string arch_meta_path )
{
	dirent	*reading_dir;
	DIR 	*current_dir;

	if ( ( current_dir	=	opendir( dir.c_str() ) ) == NULL )
		return false;
	else
	{
		while (reading_dir	=	readdir( current_dir ))
		{
			
			if ( strcmp( reading_dir->d_name, "." ) == 0 || strcmp( reading_dir->d_name, ".." ) == 0 )
				continue;
			
			if ( reading_dir -> d_type == DT_DIR )
			{				
				long int saving_place	=	telldir( current_dir );
					
				MakeDirMap( dir + reading_dir->d_name + "/", arch_path, arch_meta_path );

				DIR	*current_dir2	=	opendir( dir.c_str() );
				seekdir( current_dir, saving_place );
				closedir( current_dir2 );
			}
			else
			{
				ReadFile( dir + reading_dir->d_name, arch_path, arch_meta_path );
			}
		}
		closedir( current_dir );
	}

	return true;
};


bool Testing_placement( string path )
{

	int testing_open	=	open( path.c_str(), O_WRONLY, S_IREAD );

	if ( testing_open != -1 )
	{
		close( testing_open );
		return true;
	}

	return false;

}


bool Create_file( string path )
{
	int create_file	=	open( path.c_str(), O_CREAT|O_WRONLY, S_IREAD|S_IWRITE );

	if ( create_file != -1 )
	{
		close( create_file );

		return true;
	}
	else
	{
		return false;
	}

}


bool Creating_arch_files( string arch_path, string arch_meta_path )
{
	bool creaing_flag	=	true;

	if ( Testing_placement( arch_path ) )
	{
		unlink( arch_path.c_str() );
		creaing_flag	=	Create_file( arch_path );
	}
	else
	{
		creaing_flag	=	Create_file( arch_path );
	}

	if ( ( Testing_placement( arch_meta_path ) ) & ( creaing_flag ) )
	{
		unlink( arch_meta_path.c_str() );
		creaing_flag	=	Create_file( arch_meta_path );
	}
	else
	{
		creaing_flag	=	Create_file( arch_meta_path );
	}
	
	return creaing_flag;

}


bool Pucking( string dir, string arch_path, string arch_meta_path )
{
	bool packing_falag	=	true;
	DIR	*current_dir;

	if ( ( current_dir	=	opendir( dir.c_str() ) ) != NULL )
	{
		closedir( current_dir );

		packing_falag	=	Creating_arch_files( arch_path, arch_meta_path );	//Creating arch files at path

		if ( !packing_falag )
		{
			return packing_falag;
		}

		packing_falag	=	MakeDirMap( dir, arch_path, arch_meta_path );		//Making dirmap, writing at arch files and delete dir

		return packing_falag;
	}
	else
	{
		return false;
	}
}


bool Unpucking( string placement_path, string arch_path, string arch_meta_path )
{
	if ( !( Testing_placement( arch_path ) ) )
		return false;
	if ( !( Testing_placement( arch_meta_path ) ) )
		return false;

	if ( !( ReadArchMeta( placement_path, arch_path, arch_meta_path ) ) )
		return false;

	unlink( arch_path.c_str() );
	unlink( arch_meta_path.c_str() );

	return true;
}


int main(int n, char **argc)
{
	if (n>1)
	{
		if (!strcmp(argc[1], "pack"))
		{
			string buff = string(argc[3]) + "_meta";
			if(Pucking(argc[2], argc[3], buff) == false)
				cout<<"Error"<<endl;
		}
		else if (!strcmp(argc[1], "unpack"))
		{
			string buff = string(argc[3]) + "_meta";
			if(Unpucking(argc[2], argc[3], buff) == false)
				cout<<"Error"<<endl;
		}
		else
		{
			cout<<"Error in parametrs."<<endl;
		}
	}
	else
	{
		cout<<"Error in parametrs."<<endl;
	}
	return 1;
};