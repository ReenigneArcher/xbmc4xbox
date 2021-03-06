
/*
 * Get path to config dir/file.
 *
 * Return Values:
 *   Returns the pointer to the ALLOCATED buffer containing the
 *   zero terminated path string. This buffer has to be FREED
 *   by the caller.
 *
 */

#ifdef _XBOX
char *get_path(char *filename)
{
	char *buff=malloc(1024);
	sprintf(buff, "Q:\\system\\players\\mplayer\\%s", filename);
	mp_msg(MSGT_GLOBAL,MSGL_V,"get_path('%s') -> '%s'\n",filename,buff);
	return buff;
}
#else
char *get_path(char *filename){
	char *homedir;
	char *buff;
#if defined(__MINGW32__)
    static char *config_dir = "/mplayer";
#else
	static char *config_dir = "/.mplayer";
#endif
	int len;

	if ((homedir = getenv("HOME")) == NULL)
#if defined(__MINGW32__)||defined(__CYGWIN__) /*hack to get fonts etc. loaded outside of cygwin environment*/
	{
        int i,imax=0;       
        char exedir[260];       
        GetModuleFileNameA(NULL, exedir, 260);
        for(i=0; i< strlen(exedir);i++)if(exedir[i] =='\\'){exedir[i]='/';imax=i;}
        exedir[imax]='\0';
	    homedir = exedir;
	}
#else
		return NULL;
#endif       
	len = strlen(homedir) + strlen(config_dir) + 1;
	if (filename == NULL) {
		if ((buff = (char *) malloc(len)) == NULL)
			return NULL;
		sprintf(buff, "%s%s", homedir, config_dir);
	} else {
		len += strlen(filename) + 1;
		if ((buff = (char *) malloc(len)) == NULL)
			return NULL;
		sprintf(buff, "%s%s/%s", homedir, config_dir, filename);
	}
	mp_msg(MSGT_GLOBAL,MSGL_V,"get_path('%s') -> '%s'\n",filename,buff);
	return buff;
}
#endif //!_XBOX
