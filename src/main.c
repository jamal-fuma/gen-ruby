/* This was written by Jamal Natour
 * Copyright 2000, and released under GPL license */

/*
   Gen-Ruby is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Gen-Ruby is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
   */
/* main.c */
#include "gen-ruby.h"

int
main(int argc, char *argv[])
{    
char   		*name = NULL;
struct var 	 vars[MAX_VARS] = {{0}};    
char  		 cls_name[MAX_NAM_LEN+1] = {0};   
int   	 	 count=0;
int   		 done=0;

	setvbuf(stdin, NULL, _IONBF,0);
	setvbuf(stdout, NULL, _IONBF,0);

	/* Stash app name */ 
	set_app_name(argv[0]);

	/* process args */
	while(--argc >= 1)
	{
		/* Grab class name from command line */
		if( argc >= 1 && (( !strcmp(argv[argc-1],"--name" ) || 
				 (!strcmp(argv[argc-1],"-n")) )) )   
			name = argv[argc];
		else
			return unknown_arg(argv[argc]);
	}

	/* process class metadata */
	if(!name)
		prompt("Class Name",cls_name,MAX_NAM_LEN);
	else
		strncpy(cls_name,name,MAX_NAM_LEN);

	if(!(strlen(cls_name) >0 ))
		return usage(ERR_NO_NAME);

	message("Variable declarations");   
	while(!done)    
	{
		prompt("Variable Name",vars[count].name, MAX_NAM_LEN);
		if( strlen(vars[count].name) >0)
		{
			if('S' == ask_user_for("Variable Type (S)tring or (Long)","SL") )    
				vars[count++].type = TYPE_STRING;
			else 
				vars[count++].type = TYPE_LONG;           
		}
		else
			done =1;

	}                           

	return class_source(cls_name,vars,count); 
}

int
unknown_arg(const char *arg)
{
	printf("%s %s %s\n",get_app_name(),"error: unknown option", arg); 
	return usage(ERR_NO_ARGS);
}

int
usage(int why)
{	
	switch(why)
	{
	case ERR_NO_ARGS:
	case ERR_NO_NAME:
	default:
		printf("%s %s %s\n","usage:",get_app_name(),"-n name-of-class");
	}
	return EXIT_FAILURE;
}
