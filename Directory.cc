/*************************************************************************
	> File Name: Directory.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年05月19日 星期五 16时33分18秒
 ************************************************************************/


    Directory::Directory(const char* path) : directorypath(path),
        lock()
    {
        directorybuffer = new char[4096];
        update();
    }

    Directory::~Directory()
    {
        delete [] directorybuffer;
    }
    
    char* Directory::getDirBuffer() const 
    {
        return directorybuffer;
    }

    void Directory::update()
    {
        thread::MutexLockGuard guard(lock);
        DIR* dp = NULL;
        struct dirent* drip = NULL;

        assert((dp=::opendir(directorypath.c_str())) != NULL);

        while((drip=readdir(dp)) != NULL)
        {
            if((strcmp(drip->d_name,".")==0) || (strcmp(drip->d_name,"..")==0))
                continue;
            
            directorylist.push_back(drip->d_name);
        }
        memset(directorybuffer,'\0',4096);

        for(auto iter=directorylist.cbegin();iter!=directorylist.cend();++iter)
        {
            strcat(directorybuffer,iter->c_str());
            strcat(directorybuffer,"\t");
        }
    }

    void Directory::addInDirectoryList(const char* filename)
    {
        thread::MutexLockGuard guard(lock);
        directorylist.push_back(filename);
        strcat(directorybuffer,filename);
        strcat(directorybuffer,"\t");
    }
    
}
